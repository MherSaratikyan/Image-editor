
#include "graphicscanvas.h"
#include <QQueue>
#include <QtMath>

GraphicsCanvas::GraphicsCanvas(QWidget* parent)
    : QGraphicsView(parent),
      m_scene(new QGraphicsScene(this)),
      m_backgroundItem(nullptr),
      m_currentTool(Tool::None),
      m_penSize(3),
      m_eraserSize(3),
      m_drawingInProgress(false),
      m_rubberBand(nullptr),
      m_selecting(false),
      m_zoomFactor(1.0)
{
    setScene(m_scene);
    createBlank();
}

void GraphicsCanvas::createBlank(){
    m_filename.clear();
    m_image = QImage(800, 600, QImage::Format::Format_ARGB32);
    m_image.fill(Qt::white);
    this->updateBackground();
}

void GraphicsCanvas::loadImage(const QString &filePath)
{
    QImage temp;
    if (!temp.load(filePath)) {
        return;
    }
    m_image = temp.convertToFormat(QImage::Format_ARGB32);
    setMinimumSize(m_image.size());
    m_backgroundItem->update();
    this->updateBackground();
}

void GraphicsCanvas::saveImage()
{
    m_image.save(m_filename);
}

QImage GraphicsCanvas::getImage() const
{
    return m_image;
}

void GraphicsCanvas::setImage(const QImage &image)
{
    pushUndoState();
    m_image = image;
    m_backgroundItem->update();
    updateBackground();
}

QString GraphicsCanvas::getFilePath() const
{
    return m_filename;
}

void GraphicsCanvas::setFilePath(const QString &filePath)
{
    if(filePath.isEmpty()) { return; }
    m_filename = filePath;
}

void GraphicsCanvas::setColor(const QColor &color)
{
    m_currentColor = color;
}

void GraphicsCanvas::setPenWidth(int width)
{
    m_penSize = width;
}

void GraphicsCanvas::setEraserWidth(int width)
{
    m_eraserSize = width;
}

void GraphicsCanvas::setCurrentTool(Tool tool)
{
    if (m_currentTool == Tool::Select && m_rubberBand) {
        m_rubberBand->hide();
        m_selecting = false;
    }
    m_currentTool = tool;
}

void GraphicsCanvas::setZoomFactor(double factor)
{
    if (factor < 0.1) factor = 0.1;
    if (factor > 10.0) factor = 10.0;
    m_zoomFactor = factor;

    this->updateBackground();
}

double GraphicsCanvas::getZoomFactor() const
{
    return m_zoomFactor;
}

void GraphicsCanvas::cropSelection()
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
    this->updateBackground();
}

void GraphicsCanvas::resizeImage(int width, int height){
    pushUndoState();
    m_image = m_image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    this->updateBackground();
}

void GraphicsCanvas::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event); // no manual painting here
    m_backgroundItem->update();
    updateBackground();
}

void GraphicsCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        switch (m_currentTool)
        {
        case GraphicsCanvas::Tool::Select:
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
        case GraphicsCanvas::Tool::Fill:
        {
            pushUndoState();
            QPoint imgPt = widgetToImage(event->pos());
            if (m_image.rect().contains(imgPt)) {
                floodFill(imgPt, m_currentColor);
            }
            break;
        }
        case GraphicsCanvas::Tool::Erase:
        {
            if(!m_drawingInProgress){
                pushUndoState();
            }
            m_drawingInProgress = true;
            m_lastPoint = widgetToImage(event->pos());
            break;
        }
        case GraphicsCanvas::Tool::Pick:
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
        case GraphicsCanvas::Tool::None:
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

void GraphicsCanvas::mouseMoveEvent(QMouseEvent *event)
{
    switch (m_currentTool)
    {
    case GraphicsCanvas::Tool::Select:
        if (m_selecting && m_rubberBand) {
            QRect rect = QRect(m_selectionStart, event->pos()).normalized();
            m_rubberBand->setGeometry(rect);
        }
        break;

    case GraphicsCanvas::Tool::Pen:
        if (m_drawingInProgress && (event->buttons() & Qt::LeftButton)) {
            QPoint pt = widgetToImage(event->pos());
            drawLineTo(pt, /*eraser=*/false);
        }
        break;

    case GraphicsCanvas::Tool::Erase:
        if (m_drawingInProgress && (event->buttons() & Qt::LeftButton)) {
            QPoint pt = widgetToImage(event->pos());
            drawLineTo(pt, /*eraser=*/true);
        }
        break;

    case GraphicsCanvas::Tool::None:
        if (m_pastingInProgress && (event->buttons() & Qt::LeftButton)) {
            // Move pasted image
            m_pastePosition = widgetToImage(event->pos()) - m_pasteOffset;
            this->updateBackground();
        }
        break;

    default:
        break;
    }

    QWidget::mouseMoveEvent(event);
}

void GraphicsCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        switch (m_currentTool)
        {
        case GraphicsCanvas::Tool::Select:
            if (m_selecting) {
                m_selecting = false;
                if (m_rubberBand) {
                    m_selectionRect = m_rubberBand->geometry();
                }
            }
            break;

        case GraphicsCanvas::Tool::Pen:
            if (m_drawingInProgress) {
                QPoint pt = widgetToImage(event->pos());
                drawLineTo(pt, (m_currentTool == GraphicsCanvas::Tool::Erase));
                m_drawingInProgress = false;
            }
            break;
        case GraphicsCanvas::Tool::Erase:
            if (m_drawingInProgress) {
                QPoint pt = widgetToImage(event->pos());
                drawLineTo(pt, (m_currentTool == GraphicsCanvas::Tool::Erase));
                m_drawingInProgress = false;
            }
            break;
        case GraphicsCanvas::Tool::None:
            if (m_pastingInProgress) {
                m_pastingInProgress = false;
                this->updateBackground();
            }
            break;

        default:
            break;
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void GraphicsCanvas::drawLineTo(const QPoint &endPoint, bool eraser)
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
    m_lastPoint = endPoint;

    int rad = (eraser ? m_eraserSize : m_penSize) / 2 + 2;
    QRect updateRect = QRect(m_lastPoint, endPoint).normalized().adjusted(-rad, -rad, rad, rad);

    m_lastPoint = endPoint;
    update(updateRect);
}

void GraphicsCanvas::floodFill(const QPoint &start, const QColor &fillColor)
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

    this->updateBackground();
}

QPoint GraphicsCanvas::widgetToImage(const QPoint &widgetPos) const
{
    if (!m_backgroundItem) {
        return QPoint(-1, -1);
    }
    QPointF scenePt = mapToScene(widgetPos);
    QPointF itemPt = m_backgroundItem->mapFromScene(scenePt);
    return QPoint(qFloor(itemPt.x()), qFloor(itemPt.y()));
}

void GraphicsCanvas::copy(){
    if (!m_selectionRect.isValid() || m_selectionRect.isEmpty()) {
        return;
    }

    QRect imageRect;
    imageRect.setTopLeft(widgetToImage(m_selectionRect.topLeft()));
    imageRect.setBottomRight(widgetToImage(m_selectionRect.bottomRight()));
    imageRect = imageRect.normalized();

    QRect validRect = imageRect.intersected(QRect(0, 0, m_image.width(), m_image.height()));
    if (validRect.isEmpty()) {
        return;
    }

    m_clipboardImage = m_image.copy(validRect);
}

void GraphicsCanvas::cut() {
    if (!m_selectionRect.isValid() || m_selectionRect.isEmpty()) {
        return;
    }

    QRect imageRect;
    imageRect.setTopLeft(widgetToImage(m_selectionRect.topLeft()));
    imageRect.setBottomRight(widgetToImage(m_selectionRect.bottomRight()));
    imageRect = imageRect.normalized();

    QRect validRect = imageRect.intersected(QRect(0, 0, m_image.width(), m_image.height()));
    if (validRect.isEmpty()) {
        return;
    }

    pushUndoState();
    m_clipboardImage = m_image.copy(validRect);

    QPainter painter(&m_image);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(validRect, Qt::transparent);
    painter.end();

    if (m_rubberBand) {
        m_rubberBand->hide();
    }
    m_selectionRect = QRect(); // reset
    update();
}

void GraphicsCanvas::paste(){
    if (m_clipboardImage.isNull())
        return;

    pushUndoState();
    QPainter painter(&m_image);
    painter.drawImage(0, 0, m_clipboardImage);
    update();
}

void GraphicsCanvas::pushUndoState(){
    m_undoStack.push(m_image);
    m_redoStack.clear();
}

void GraphicsCanvas::undo(){
    if (m_undoStack.isEmpty()) {
        return;
    }

    m_redoStack.push(m_image);
    m_image = m_undoStack.pop();
    this->updateBackground();
}

void GraphicsCanvas::redo(){
    if (m_redoStack.isEmpty()) {
        return;
    }

    m_undoStack.push(m_image);
    m_image = m_redoStack.pop();
    this->updateBackground();
}

bool GraphicsCanvas::canUndo() const{
    return !m_undoStack.isEmpty();
}

bool GraphicsCanvas::canRedo() const{
    return !m_redoStack.isEmpty();
}

void GraphicsCanvas::setBrushStyle(BrushStyle style)
{
    m_brushStyle = style;
}

GraphicsCanvas::BrushStyle GraphicsCanvas::getBrushStyle() const
{
    return m_brushStyle;
}

void GraphicsCanvas::updateBackground(){
    if (!m_backgroundItem) {
        m_backgroundItem = new QGraphicsPixmapItem(QPixmap::fromImage(m_image));
        m_scene->addItem(m_backgroundItem);
    } else {
        m_backgroundItem->setPixmap(QPixmap::fromImage(m_image));
    }
    m_scene->setSceneRect(0, 0, m_image.width(), m_image.height());
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}




