//#ifndef CANVAS_H
//#define CANVAS_H

//#include <QWidget>
//#include <QString>
//#include <QColor>
//#include <QImage>
//#include <QRubberBand>
//#include <QStack>
//#include <QGraphicsView>
//#include <QGraphicsScene>
//#include <QGraphicsPixmapItem>

//class Canvas : public QGraphicsView
//{
//    Q_OBJECT
//public:
//    enum class Tool{
//        None,
//        Select,
//        Pen,
//        Fill,
//        Erase,
//        Pick,
//        Magnify,
//    };

//    enum class BrushStyle {
//        Solid,
//        Dense1,
//        Dense2,
//        Dense3,
//        Dense4,
//        Dense5,
//        Dense6,
//        Dense7,
//        Hor,
//        Ver,
//        Cross,
//        BDiag,
//        FDiag,
//        DiagCross
//    };
//public:
//    explicit Canvas(QWidget *parent = nullptr);

//    void loadImage(const QString& filePath);
//    void saveImage();
//    void updateBackground();
//public:
//    void createBlank();
//    QImage getImage() const;
//    void setImage(const QImage& image);

//    QString getFilePath() const;
//    void setFilePath(const QString& filePath);

//    void setColor(const QColor& color);
//    void setPenWidth(int width);
//    void setEraserWidth(int width);
//    void setBrushStyle(BrushStyle style);
//    BrushStyle getBrushStyle() const;

//    void setCurrentTool(Canvas::Tool tool);

//    void setZoomFactor(double factor);
//    double getZoomFactor() const;

//    void cropSelection();
//    void resizeImage(int width, int heigth);

//    void copy();
//    void cut();
//    void paste();

//    void pushUndoState();
//    void undo();
//    void redo();

//    bool canUndo() const;
//    bool canRedo() const;
//protected:
//    void paintEvent(QPaintEvent *event) override;
//    void mousePressEvent(QMouseEvent *event) override;
//    void mouseMoveEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;

//private:
//    void drawLineTo(const QPoint &endPoint, bool eraser);
//    void floodFill(const QPoint &p, const QColor &targetColor);
//    QPoint widgetToImage(const QPoint &widgetPos) const;

//private:
//    QGraphicsScene* m_scene;
//    QGraphicsPixmapItem* m_backgroundItem;
//    QImage m_image;
//    QString m_filename;
//    Canvas::Tool m_currentTool;
//    QColor    m_currentColor;
//    QColor    m_pickedColor;
//    int       m_penSize;
//    int       m_eraserSize;
//    BrushStyle m_brushStyle;  // Stores selected brush style
//    bool   m_drawingInProgress;
//    QPoint m_lastPoint;

//    QRubberBand *m_rubberBand;
//    bool         m_selecting;
//    QPoint       m_selectionStart;
//    QRect        m_selectionRect;

//    double m_zoomFactor;

//    QImage m_clipboardImage;
//    QPoint m_clipboardPosition;
//    bool m_clipboardAvailable = false;

//    QPoint m_pastePosition;
//    QImage m_pastedImage;
//    QPoint m_pasteOffset;
//    bool m_pastingInProgress;

//    QStack<QImage> m_undoStack;
//    QStack<QImage> m_redoStack;
//};

//#endif // CANVAS_H
