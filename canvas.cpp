#include "canvas.h"

#include <QPainter>
#include <QMouseEvent>
#include <QQueue>
#include <QtMath>
#include <QBrush>

Canvas::Canvas(QWidget *parent)
    : QWidget(parent)
    , m_image{nullptr}
    , m_filename{}
    , m_currentTool{Canvas::Tool::None}
    , m_currentColor{Qt::black}
    , m_pickedColor{Qt::black}
    , m_penSize{3}
    , m_eraserSize{3}
    , m_drawingInProgress{false}
    , m_lastPoint{}
    , m_rubberBand{nullptr}
    , m_selecting{false}
    , m_selectionStart{}
    , m_selectionRect{}
    , m_zoomFactor{1.0}
{
    createBlank();
}

void Canvas::createBlank(){
    m_filename.clear();
    m_image = QImage(800, 600, QImage::Format_ARGB32);
    m_image.fill(Qt::white);
    setMinimumSize(m_image.size());
}

void Canvas::loadImage(const QString &filePath)
{
    QImage temp;
    if (!temp.load(filePath)) {
        return;
    }
    m_image = temp.convertToFormat(QImage::Format_ARGB32);
    setMinimumSize(m_image.size());
    update();
}

void Canvas::saveImage()
{
    m_image.save(m_filename);
}

QImage Canvas::getImage() const
{
    return m_image;
}

void Canvas::setImage(const QImage &image)
{
    m_image = image;
}

QString Canvas::getFilePath() const
{
    return m_filename;
}

void Canvas::setFilePath(const QString &filePath)
{
    if(filePath.isEmpty()) { return; }
    m_filename = filePath;
}

void Canvas::setColor(const QColor &color)
{
    m_currentColor = color;
}

void Canvas::setPenWidth(int width)
{
    m_penSize = width;
}

void Canvas::setEraserWidth(int width)
{
    m_eraserSize = width;
}

void Canvas::setCurrentTool(Tool tool)
{
    if (m_currentTool == Tool::Select && m_rubberBand) {
        m_rubberBand->hide();
        m_selecting = false;
    }
    m_currentTool = tool;
}

void Canvas::setZoomFactor(double factor)
{
    if (factor < 0.1) factor = 0.1;
    if (factor > 10.0) factor = 10.0;
    m_zoomFactor = factor;

    update();
}

double Canvas::getZoomFactor() const
{
    return m_zoomFactor;
}

void Canvas::cropSelection()
{
    if (!m_selectionRect.isValid() || m_selectionRect.isEmpty()) {
        //qDebug() << "No valid selection to crop.";
        return;
    }
    QRect imageRect;
    imageRect.setTopLeft(widgetToImage(m_selectionRect.topLeft()));
    imageRect.setBottomRight(widgetToImage(m_selectionRect.bottomRight()));
    imageRect = imageRect.normalized();

    QRect validRect = imageRect.intersected(QRect(0, 0, m_image.width(), m_image.height()));
    if (validRect.isEmpty()) {
        //qDebug() << "Selection is out of image bounds.";
        return;
    }
    pushUndoState();
    QImage newImg = m_image.copy(validRect);
    m_image = newImg;
    setMinimumSize(m_image.size());

    if (m_rubberBand) {
        m_rubberBand->hide();
    }
    m_selectionRect = QRect(); // reset
    update();
}

void Canvas::resizeImage(int width, int height){
    pushUndoState();
    m_image = m_image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    update();
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Option 1: scale the painter
    painter.scale(m_zoomFactor, m_zoomFactor);


    painter.drawImage(0, 0, m_image);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        switch (m_currentTool)
        {
        case Canvas::Tool::Select:
        {
            m_selecting = true;
            m_selectionStart = event->pos();
            if (!m_rubberBand) {
                m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            }
            m_rubberBand->setGeometry(QRect(m_selectionStart, QSize()));
            m_rubberBand->show();
            break;
        }
        case Tool::Pen:
        {
            if(!m_drawingInProgress){
                pushUndoState();
            }
            m_drawingInProgress = true;
            m_lastPoint = widgetToImage(event->pos());
            break;
        }
        case Canvas::Tool::Fill:
        {
            pushUndoState();
            QPoint imgPt = widgetToImage(event->pos());
            if (m_image.rect().contains(imgPt)) {
                floodFill(imgPt, m_currentColor);
            }
            break;
        }
        case Canvas::Tool::Erase:
        {
            m_drawingInProgress = true;
            m_lastPoint = widgetToImage(event->pos());
            break;
        }
        case Canvas::Tool::Pick:
        {
            QPoint imgPt = widgetToImage(event->pos());
            if (m_image.rect().contains(imgPt)) {
                QRgb pix = m_image.pixel(imgPt);
                m_pickedColor = QColor::fromRgba(pix);
            }
            break;
        }
        case Tool::Magnify:
        {
            pushUndoState();
            setZoomFactor(m_zoomFactor * 1.25);
            break;
        }
        case Canvas::Tool::None:
        {
            // Check if user clicked on the pasted object
            if (!m_pastedImage.isNull()) {
                QRect pasteRect(m_pastePosition, m_pastedImage.size());
                if (pasteRect.contains(widgetToImage(event->pos()))) {
                    m_pastingInProgress = true;
                    m_pasteOffset = widgetToImage(event->pos()) - m_pastePosition;
                }
            }
            break;
        }
        default:
            break;
        }
    }

    QWidget::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    switch (m_currentTool)
    {
    case Canvas::Tool::Select:
        if (m_selecting && m_rubberBand) {
            QRect rect = QRect(m_selectionStart, event->pos()).normalized();
            m_rubberBand->setGeometry(rect);
        }
        break;

    case Canvas::Tool::Pen:
        if (m_drawingInProgress && (event->buttons() & Qt::LeftButton)) {
            QPoint pt = widgetToImage(event->pos());
            drawLineTo(pt, /*eraser=*/false);
        }
        break;

    case Canvas::Tool::Erase:
        if (m_drawingInProgress && (event->buttons() & Qt::LeftButton)) {
            QPoint pt = widgetToImage(event->pos());
            drawLineTo(pt, /*eraser=*/true);
        }
        break;

    case Canvas::Tool::None:
        if (m_pastingInProgress && (event->buttons() & Qt::LeftButton)) {
            // Move pasted image
            m_pastePosition = widgetToImage(event->pos()) - m_pasteOffset;
            update();
        }
        break;

    default:
        break;
    }

    QWidget::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        switch (m_currentTool)
        {
        case Canvas::Tool::Select:
            if (m_selecting) {
                m_selecting = false;
                if (m_rubberBand) {
                    m_selectionRect = m_rubberBand->geometry();
                }
            }
            break;

        case Canvas::Tool::Pen:
            if (m_drawingInProgress) {
                QPoint pt = widgetToImage(event->pos());
                drawLineTo(pt, (m_currentTool == Canvas::Tool::Erase));
                m_drawingInProgress = false;
            }
            break;
        case Canvas::Tool::Erase:
            if (m_drawingInProgress) {
                QPoint pt = widgetToImage(event->pos());
                drawLineTo(pt, (m_currentTool == Canvas::Tool::Erase));
                m_drawingInProgress = false;
            }
            break;
        case Canvas::Tool::None:
            if (m_pastingInProgress) {
                m_pastingInProgress = false;
                update();
            }
            break;

        default:
            break;
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void Canvas::drawLineTo(const QPoint &endPoint, bool eraser)
{
    if (!m_image.rect().contains(endPoint)) {
        m_lastPoint = endPoint;
        return;
    }

    QPainter painter(&m_image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    QBrush brush;
    brush.setColor(eraser ? Qt::white : m_currentColor);
    pen.setWidth(eraser ? m_eraserSize : m_penSize);
    pen.setColor(eraser ? Qt::white : m_currentColor);

    // Apply selected brush style
    switch (m_brushStyle)
    {
    case BrushStyle::Solid:
        pen.setStyle(Qt::SolidLine);
        break;
    case BrushStyle::Dense1:
        brush.setStyle(Qt::Dense1Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Dense2:
        brush.setStyle(Qt::Dense2Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Dense3:
        brush.setStyle(Qt::Dense3Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Dense4:
        brush.setStyle(Qt::Dense4Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Dense5:
        brush.setStyle(Qt::Dense5Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Dense6:
        brush.setStyle(Qt::Dense6Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Dense7:
        brush.setStyle(Qt::Dense7Pattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Hor:
        brush.setStyle(Qt::HorPattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Ver:
        brush.setStyle(Qt::VerPattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::Cross:
        brush.setStyle(Qt::CrossPattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::BDiag:
        brush.setStyle(Qt::BDiagPattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::FDiag:
        brush.setStyle(Qt::FDiagPattern);
        pen.setBrush(brush);
        break;
    case BrushStyle::DiagCross:
        brush.setStyle(Qt::DiagCrossPattern);
        pen.setBrush(brush);
        break;
    }

    painter.setPen(pen);
    painter.drawLine(m_lastPoint, endPoint);

    int rad = (eraser ? m_eraserSize : m_penSize) / 2 + 2;
    QRect updateRect = QRect(m_lastPoint, endPoint).normalized().adjusted(-rad, -rad, rad, rad);

    m_lastPoint = endPoint;
    update(updateRect);
}

void Canvas::floodFill(const QPoint &start, const QColor &fillColor)
{
    if (!m_image.rect().contains(start)) return;

    QRgb oldColor = m_image.pixel(start);
    QRgb newColor = fillColor.rgba();
    if (oldColor == newColor) {
        return;
    }

    QQueue<QPoint> queue;
    queue.enqueue(start);

    while (!queue.isEmpty()) {
        QPoint p = queue.dequeue();
        if (!m_image.rect().contains(p)) continue;

        if (m_image.pixel(p) == oldColor) {
            // set new color
            m_image.setPixel(p, newColor);

            // enqueue neighbors (4-direction)
            queue.enqueue(QPoint(p.x() + 1, p.y()));
            queue.enqueue(QPoint(p.x() - 1, p.y()));
            queue.enqueue(QPoint(p.x(), p.y() + 1));
            queue.enqueue(QPoint(p.x(), p.y() - 1));
        }
    }

    this->update();
}

QPoint Canvas::widgetToImage(const QPoint &widgetPos) const
{
    double x = widgetPos.x() / m_zoomFactor;
    double y = widgetPos.y() / m_zoomFactor;
    return QPoint(qFloor(x), qFloor(y));
}

void Canvas::copy(){
    if (!m_selectionRect.isValid() || m_selectionRect.isEmpty()) {
        return;
    }

    QRect validRect = m_selectionRect.intersected(m_image.rect());
    if (validRect.isEmpty()) {
        return;
    }

    m_clipboardImage = m_image.copy(validRect);
    m_clipboardPosition = validRect.topLeft();
    m_clipboardAvailable = true;
}

void Canvas::cut() {
    copy();

    if (!m_clipboardAvailable) {
        return;
    }

    QPainter painter(&m_image);
    painter.fillRect(m_selectionRect, Qt::white);
    update();
}

void Canvas::paste(){
    if (!m_clipboardAvailable) {
        return;
    }

    m_pastePosition = QPoint(50, 50);
    m_pastedImage = m_clipboardImage;
    m_pastingInProgress = false;

    update();
}

void Canvas::pushUndoState(){
    m_undoStack.push(m_image);
    m_redoStack.clear();
}

void Canvas::undo(){
    if (m_undoStack.isEmpty()) {
        return;
    }

    m_redoStack.push(m_image);
    m_image = m_undoStack.pop();
    update();
}

void Canvas::redo(){
    if (m_redoStack.isEmpty()) {
        return;
    }

    m_undoStack.push(m_image);
    m_image = m_redoStack.pop();
    update();
}

bool Canvas::canUndo() const{
    return !m_undoStack.isEmpty();
}

bool Canvas::canRedo() const{
    return !m_redoStack.isEmpty();
}

void Canvas::setBrushStyle(BrushStyle style)
{
    m_brushStyle = style;
}

Canvas::BrushStyle Canvas::getBrushStyle() const
{
    return m_brushStyle;
}
