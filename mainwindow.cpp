#include "mainwindow.h"
#include "filterdialog.h"
#include "resizedialog.h"
//#include "canvas.h"
#include "graphicscanvas.h"
#include "imagemanipulator.h"
#include "filterapplyer.h"
#include "databasemanager.h"

#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>
#include <QStatusBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QVector>
#include <QSpinBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <fstream>
#include <QCryptographicHash>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_fileMenu{nullptr}
    , m_editMenu{nullptr}
    , m_undoOption{nullptr}
    , m_redoOption{nullptr}
    , m_filtersLabel{nullptr}
    , m_toolBar{nullptr}
    , m_currentColorButton{nullptr}
//    , m_canvas{new Canvas(this)}
    , m_canvas{new GraphicsCanvas}
{
    buildMenuBar();
    createMainToolBar();

    QScrollArea* scroll_area = new QScrollArea(this);
    scroll_area->setWidget(m_canvas);
    scroll_area->setWidgetResizable(true);
    this->setCentralWidget(scroll_area);

    // Optionally show a status bar
    this->statusBar()->showMessage("Ready");

    auto db = DatabaseManager::instance();
    db->openDatabase("projects_library");
    db->createTables();
    db->closeDatabase();
}

MainWindow::~MainWindow()
{
}

void MainWindow::buildMenuBar()
{
    buildFileMenu();
    buildEditMenu();

    m_undoOption = new QAction(QIcon(":/Icons/undo-alt.png"), "Undo", this);
    m_undoOption->setToolTip("Undo");
//    m_undoOption->setIcon(QIcon(":/Icons/undo_alt.png"));
    connect(m_undoOption, &QAction::triggered, this, &MainWindow::onUndoClicked);
    menuBar()->addAction(m_undoOption);

    m_redoOption = new QAction(QIcon(":/Icons/redo-alt.png"),"Redo", this);
    m_redoOption->setToolTip("Redo");
//    m_redoOption->setIcon(QIcon(":/Icons/redo_alt.png"));
    connect(m_redoOption, &QAction::triggered, this, &MainWindow::onRedoClicked);
    menuBar()->addAction(m_redoOption);
}

void MainWindow::buildFileMenu()
{
    m_fileMenu = menuBar()->addMenu("File");
    QMenu* newMenu = new QMenu("New", this);
    newMenu->addAction("Project", this, &MainWindow::onNewProjectClicked);
    m_newFileAction = newMenu->addAction("File", this, &MainWindow::onNewFileClicked);
    m_newFileAction->setEnabled(false);
    m_fileMenu->addMenu(newMenu);

    QMenu* openMenu = new QMenu("Open", this);
    openMenu->addAction("Project", this, &MainWindow::onOpenProjectCLicked);
    m_openFileAction = openMenu->addAction("File", this, &MainWindow::onOpenFileClicked);
    m_openFileAction->setEnabled(false);
    m_fileMenu->addMenu(openMenu);

    m_fileMenu->addAction("Save",      this, &MainWindow::onSaveClicked);
    m_fileMenu->addAction("Save As",   this, &MainWindow::onSaveAsClicked);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("Exit",      this, &MainWindow::onExitClicked);

}

void MainWindow::buildEditMenu()
{
    m_editMenu = menuBar()->addMenu("Edit");
    m_editMenu->addAction("Cut",   this, &MainWindow::onCutClicked);
    m_editMenu->addAction("Copy",  this, &MainWindow::onCopyClicked);
    m_editMenu->addAction("Paste", this, &MainWindow::onPasteClicked);
}

void MainWindow::createMainToolBar(){
    m_toolBar = addToolBar("MainToolBar");
    m_toolBar->setMovable(false);

    buildGeometricOperationsSection();
    buildSizeSelector();
    buildToolsSection();
    buildBrushMenu();
    buildColorMenu();
    buildFilterMenu();
    buildAdvancedOperationsSection();
}

void MainWindow::buildGeometricOperationsSection()
{
    QToolButton *rotateBtn = new QToolButton(this);
    rotateBtn->setIcon(QIcon(":/Icons/refresh.png"));
    rotateBtn->setToolTip("Rotate");
    QMenu *rotateMenu = new QMenu(rotateBtn);

    QAction *rotateLeft  = new QAction(QIcon(":/Icons/rotate-left.png"),"90° Left",  rotateMenu);
    rotateLeft->setCheckable(true);
    connect(rotateLeft, &QAction::triggered, this, &MainWindow::onRotateLeftCLicked);

    QAction *rotateRight = new QAction(QIcon(":/Icons/rotate-right.png"),"90° Right", rotateMenu);
    rotateRight->setCheckable(true);
    connect(rotateRight, &QAction::triggered, this, &MainWindow::onRotateRightClicked);

    QAction *rotate180   = new QAction(QIcon(":/Icons/refresh.png"), "180°", rotateMenu);
    rotate180->setCheckable(true);
    connect(rotate180, &QAction::triggered, this, &MainWindow::onRotate180Clicked);

    rotateMenu->addAction(rotateLeft);
    rotateMenu->addAction(rotateRight);
    rotateMenu->addAction(rotate180);
    rotateBtn->setMenu(rotateMenu);
    rotateBtn->setPopupMode(QToolButton::MenuButtonPopup);

    QToolButton *flipBtn = new QToolButton(this);
    flipBtn->setIcon(QIcon(":/Icons/mirror.png"));
    flipBtn->setToolTip("Flip");
    QMenu *flipMenu = new QMenu(flipBtn);

    QAction *flipH = new QAction("Horizontal", flipMenu);
    flipH->setCheckable(true);
    connect(flipH, &QAction::triggered, this, &MainWindow::onHorizontalFlipClicked);

    QAction *flipV = new QAction("Vertical",   flipMenu);
    flipV->setCheckable(true);
    connect(flipV, &QAction::triggered, this, &MainWindow::onVerticalFlipClicked);

    flipMenu->addAction(flipH);
    flipMenu->addAction(flipV);
    flipBtn->setMenu(flipMenu);
    flipBtn->setPopupMode(QToolButton::MenuButtonPopup);

    QAction *cropAction = new QAction(QIcon(":/Icons/tool-crop.png"), "Crop", this);
    connect(cropAction, &QAction::triggered, this, &MainWindow::onCropClicked);

    QAction *resizeAction = new QAction(QIcon(":/Icons/resize.png"),"Resize", this);
    connect(resizeAction, &QAction::triggered, this, &MainWindow::onResizeClicked);

    m_toolBar->addWidget(rotateBtn);
    m_toolBar->addWidget(flipBtn);
    m_toolBar->addAction(cropAction);
    m_toolBar->addAction(resizeAction);

    m_toolBar->addSeparator();
}

void MainWindow::buildSizeSelector(){
    QSpinBox *brushSizeSelector = new QSpinBox(this);
    brushSizeSelector->setRange(1, 50);
    brushSizeSelector->setValue(3);

    connect(brushSizeSelector, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onWidthChanged);

    m_toolBar->addWidget(brushSizeSelector);
    m_toolBar->addSeparator();
}

void MainWindow::buildToolsSection()
{
    QWidget *toolsWidget = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(toolsWidget);
    vLayout->setContentsMargins(0,0,0,0);

    // first row
    QHBoxLayout *firstRow = new QHBoxLayout();
    QPushButton *selectorBtn = new QPushButton("");
    selectorBtn->setIcon(QIcon(":/Icons/square-dashed (1).png"));
    selectorBtn->setToolTip("Select");
    QPushButton *penBtn      = new QPushButton("");
    penBtn->setIcon(QIcon(":/Icons/pencil (1).png"));
    penBtn->setToolTip("Pencil");
    QPushButton *fillBtn = new QPushButton("");
    fillBtn->setIcon(QIcon(":/Icons/fill.png"));
    fillBtn->setToolTip("Fill");

    connect(selectorBtn, &QPushButton::clicked, this, &MainWindow::onSelectClicked);
    connect(penBtn,      &QPushButton::clicked, this, &MainWindow::onPenClicked);
    connect(fillBtn,     &QPushButton::clicked, this, &MainWindow::onFillClicked);

    firstRow->addWidget(selectorBtn);
    firstRow->addWidget(penBtn);
    firstRow->addWidget(fillBtn);

    // second row
    QHBoxLayout *secondRow = new QHBoxLayout();
    QPushButton *eraserBtn  = new QPushButton("");
    eraserBtn->setIcon(QIcon(":/Icons/eraser.png"));
    eraserBtn->setToolTip("Eraser");
    QPushButton *pickBtn    = new QPushButton("");
    pickBtn->setIcon(QIcon(":/Icons/eye-dropper-half.png"));
    pickBtn->setToolTip("Color pick");
    QPushButton *magnifyBtn = new QPushButton("");
    magnifyBtn->setIcon(QIcon(":/Icons/search.png"));
    magnifyBtn->setToolTip("Magnify");

    connect(eraserBtn,  &QPushButton::clicked, this, &MainWindow::onEraseClicked);
    connect(pickBtn,    &QPushButton::clicked, this, &MainWindow::onPickClicked);
    connect(magnifyBtn, &QPushButton::clicked, this, &MainWindow::onMagnifyClicked);

    secondRow->addWidget(eraserBtn);
    secondRow->addWidget(pickBtn);
    secondRow->addWidget(magnifyBtn);

    vLayout->addLayout(firstRow);
    vLayout->addLayout(secondRow);

    m_toolBar->addWidget(toolsWidget);

    m_toolBar->addSeparator();
}

void MainWindow::buildBrushMenu()
{
    QComboBox *brushSelector = new QComboBox(this);
    brushSelector->addItem(QIcon(":/Icons/makeup-brush.png"), "");
    brushSelector->addItem("Solid");
    brushSelector->addItem("Dense1");
    brushSelector->addItem("Dense2");
    brushSelector->addItem("Dense3");
    brushSelector->addItem("Dense4");
    brushSelector->addItem("Dense5");
    brushSelector->addItem("Dense6");
    brushSelector->addItem("Dense7");
    brushSelector->addItem("Horizontal");
    brushSelector->addItem("Vertical");
    brushSelector->addItem("Cross");
    brushSelector->addItem("BDiag");
    brushSelector->addItem("FDiag");
    brushSelector->addItem("DiagCross");

    connect(brushSelector, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::onSelectBrushClicked);

    m_toolBar->addWidget(brushSelector);
}

void MainWindow::buildColorMenu()
{
    QVector<QColor> quickColors = {
          Qt::black, Qt::white, Qt::red, Qt::green, Qt::blue,
          Qt::yellow, Qt::cyan, Qt::magenta
      };

      for (const QColor &col : quickColors) {
          QToolButton *colorBtn = new QToolButton(m_toolBar);
          colorBtn->setAutoRaise(true);  // flat look
          colorBtn->setToolTip(QString("Choose %1").arg(col.name()));
          colorBtn->setFixedSize(QSize(24,24));

          // Create a pixmap of 24x24 filled with this color
          QPixmap pix(24,24);
          pix.fill(col);
          colorBtn->setIcon(QIcon(pix));

          // Connect: when clicked, set this color as the current color
          connect(colorBtn, &QToolButton::clicked, this, [this, col]() {
              onColorPicked(col);
          });

          m_toolBar->addWidget(colorBtn);
      }

      m_toolBar->addSeparator();

     m_currentColorButton = new QToolButton(m_toolBar);
     m_currentColorButton->setAutoRaise(true);
     m_currentColorButton->setFixedSize(QSize(24,24));
     m_currentColorButton->setToolTip("Current Color");
     m_toolBar->addWidget(m_currentColorButton);

     updateCurrentColorSwatch(Qt::black);

     QAction *pickAnyColorAction = new QAction(QIcon(":/Icons/palette.png"),"Pick Color...", this);
     connect(pickAnyColorAction, &QAction::triggered, this, &MainWindow::onColorPanelClicked);

     m_toolBar->addAction(pickAnyColorAction);
//    QToolButton *colorBtn = new QToolButton(this);
//    colorBtn->setText("Colors");
//    connect(colorBtn, &QToolButton::clicked, this, &MainWindow::onColorPanelClicked);
//    m_toolBar->addWidget(colorBtn);

//    m_toolBar->addSeparator();
}

void MainWindow::buildFilterMenu()
{
    QToolButton *filterBtn = new QToolButton(this);
    filterBtn->setText("Filters");
    connect(filterBtn, &QToolButton::clicked, this, &MainWindow::onFiltersClicked);
    m_toolBar->addWidget(filterBtn);

    m_filtersLabel = new QLabel("No Filter", this);
    m_toolBar->addWidget(m_filtersLabel);

    m_toolBar->addSeparator();
}

void MainWindow::buildAdvancedOperationsSection()
{
    QToolButton *advancedBtn = new QToolButton(this);
    advancedBtn->setText("Advanced");
    QMenu *advancedMenu = new QMenu(advancedBtn);

    QAction *noiseReductionAction = new QAction("Noise Reduction", advancedMenu);
    connect(noiseReductionAction, &QAction::triggered, this, &MainWindow::onNoiseReductionClicked);

    QAction *edgeDetectionAction  = new QAction("Edge Detection", advancedMenu);
    connect(edgeDetectionAction, &QAction::triggered, this, &MainWindow::onEdgeDetectionClicked);


//    QMenu *processesSubMenu = new QMenu("Processes", advancedMenu);
//    QAction *createProcessAction  = new QAction("Create Process", processesSubMenu);
//    QAction *executeProcessAction = new QAction("Execute Process", processesSubMenu);
//    connect(createProcessAction,  &QAction::triggered, this, &MainWindow::onCreateProcessClicked);
//    connect(executeProcessAction, &QAction::triggered, this, &MainWindow::onExecuteProcessClicked);
//    processesSubMenu->addAction(createProcessAction);
//    processesSubMenu->addAction(executeProcessAction);

    advancedMenu->addAction(noiseReductionAction);
    advancedMenu->addAction(edgeDetectionAction);
//    advancedMenu->addMenu(processesSubMenu);

    advancedBtn->setMenu(advancedMenu);
    advancedBtn->setPopupMode(QToolButton::InstantPopup);

    m_toolBar->addWidget(advancedBtn);
}

void MainWindow::showResizeDialog(){
    ResizeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        bool pixelBased = dialog.isPixelBased();
        bool percentBased = dialog.isPercentageBased();

        if (pixelBased) {
            int w = dialog.pixelWidth();
            int h = dialog.pixelHeight();
            m_canvas->resizeImage(w, h);
        }
        else if (percentBased) {
            int wp = dialog.percentageWidth();
            int hp = dialog.percentageHeight();
            auto image = m_canvas->getImage();
            m_canvas->resizeImage(image.width() * wp / 100, image.height() * hp / 100);
        }
        else {
            return;
        }
    } else {
        return;
    }
}

void MainWindow::showFiltersDialog()
{
    FilterDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        // User clicked OK
        QString chosenFilter = dialog.selectedFilter();
        if (!chosenFilter.isEmpty()) {
            onFilterChosen(chosenFilter);
        } else {
            //QMessageBox::information(this, "Filter Dialog", "No filter was selected.");
        }
    } else {
        // User cancelled
        //QMessageBox::information(this, "Filter Dialog", "Filter selection canceled.");
    }
}

void MainWindow::onFilterChosen(const QString& name){
    m_canvas->pushUndoState();
    if(name == "Grayscale"){
        m_canvas->setImage(FilterApplyer::applyGrayscale(m_canvas->getImage()));
    } else if(name == "Sepia"){
        m_canvas->setImage(FilterApplyer::applySepia(m_canvas->getImage()));
    } else if(name == "Invert"){
        m_canvas->setImage(FilterApplyer::applyInvert(m_canvas->getImage()));
    } else if(name == "Blur"){
        m_canvas->setImage(FilterApplyer::applyBlur(m_canvas->getImage()));
    } else if(name == "Brightness"){
        m_canvas->setImage(FilterApplyer::applyBrightnessFilter(m_canvas->getImage(), 30));
    } else if(name == "Contrast"){
        m_canvas->setImage(FilterApplyer::applyContrast(m_canvas->getImage(), 1.2));
    } else if(name == "Saturation"){
        m_canvas->setImage(FilterApplyer::applySaturation(m_canvas->getImage(), true));
    } else if(name == "Desaturation"){
        m_canvas->setImage(FilterApplyer::applySaturation(m_canvas->getImage(), false));
    } else if(name == "Hue"){
        m_canvas->setImage(FilterApplyer::applyHue(m_canvas->getImage(), 30));
    } else if(name == "Posterize"){
        m_canvas->setImage(FilterApplyer::applyPosterize(m_canvas->getImage(), 5));
    } else if(name == "Solarize"){
        m_canvas->setImage(FilterApplyer::applySolarize(m_canvas->getImage(), 128));
    } else if(name == "Pixelate"){
        m_canvas->setImage(FilterApplyer::applyPixelate(m_canvas->getImage(), 30));
    } else if(name == "Vignette"){
        m_canvas->setImage(FilterApplyer::applyVignete(m_canvas->getImage()));
    } else if(name == "Sharpen"){
        m_canvas->setImage(FilterApplyer::applyDeBlur(m_canvas->getImage()));
    }
}

void MainWindow::onNewFileClicked()
{
    m_canvas->createBlank();
}

void MainWindow::onNewProjectClicked(){
    QString projectName = QInputDialog::getText(this, "New Project", "Enter project name:");
    if (projectName.isEmpty()) return;

    if (!projectName.endsWith(".proj"))
        projectName += ".proj";

    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Project Folder");
    if (dirPath.isEmpty()) return;

    QDir projectDir(dirPath);
    projectDir.mkdir(projectName);
    QString fullPath = projectDir.filePath(projectName);

    QDir(fullPath).mkdir("assets");
    QString signature_file_path = fullPath + "/.signature_proj";
    QFile signature_file(signature_file_path);
    if(signature_file.open(QFile::WriteOnly)){
//        QDataStream out(&signature_file);
//        out.setByteOrder(QDataStream::LittleEndian);
//        out<<PROJECT_MAGIC_NUMBER;
//        signature_file.close();

        signature_file.write(PROJECT_MAGIC_NUMBER);
        signature_file.close();
    }else{
        QMessageBox::critical(this, "Error", "Could not create signature file.");
        return;
    }

    auto db = DatabaseManager::instance();
    db->openDatabase("projects_library");
    //QMessageBox::information(this, "Info", projectName + " " + fullPath);
    db->createProject(projectName, fullPath);

    m_currentProjectPath = fullPath;
    int currentProjectId = -1;
    m_canvas->createBlank();
    m_currentFilePath.clear();
    setWindowTitle("Untitled - " + projectName);
    m_openFileAction->setEnabled(true);
    m_newFileAction->setEnabled(true);
}

void MainWindow::onOpenFileClicked()
{
    QString assetsDir = m_currentFilePath + "/assets";
    QString file_path{QFileDialog::getOpenFileName(this, "Open image", assetsDir,"*.png *.jpeg *jpg *bmp")};
    if(!file_path.isEmpty()) {
        m_canvas->loadImage(file_path);
        statusBar()->showMessage("Image " + file_path + " loaded", 5000);
        m_canvas->setFilePath(file_path);
    }
}

void MainWindow::onOpenProjectCLicked(){
    QString dirPath = QFileDialog::getExistingDirectory(this, "Open Project Folder");
    if (dirPath.isEmpty()) return;

    QFile sigFile(dirPath + "/.signature_proj");
    if (!sigFile.exists()){
        QMessageBox::critical(this, "Error", "The project" + dirPath + " is invalid or has been corrupted");
    }

    if (!sigFile.open(QFile::ReadOnly)){
        QMessageBox::critical(this, "Error", "Unable to access files of the directory" + dirPath + ". Permission denied");
        return;
    }

    QByteArray storedHash = sigFile.readAll();
    sigFile.close();

//    QByteArray raw(reinterpret_cast<const char*>(&PROJECT_MAGIC_NUMBER), sizeof(PROJECT_MAGIC_NUMBER));
//    QByteArray computedHash = QCryptographicHash::hash(raw, QCryptographicHash::Sha256);

    if(storedHash != PROJECT_MAGIC_NUMBER){
        QMessageBox::critical(this, "Error", "The project" + dirPath + " is invalid or has been corrupted");
        return;
    }

    m_currentProjectPath = dirPath;
    //m_currentProjectFile = projectFilePath;
    auto db = DatabaseManager::instance();
    db->openDatabase("projects_library");
    QString imagePath = db->getImage(m_currentProjectPath);
    QMessageBox::information(this, "Info", imagePath);
    if (m_canvas) {
        if(!imagePath.isEmpty()) {
            m_canvas->loadImage(imagePath);
        } else {
            m_canvas->createBlank();
        }
    }

    QFileInfo fileInfo(dirPath);
    setWindowTitle(fileInfo.fileName() + " - " + QCoreApplication::applicationName());

    m_newFileAction->setEnabled(true);
    m_openFileAction->setEnabled(true);
}

void MainWindow::onSaveClicked()
{
    if (m_canvas->getFilePath().isEmpty()) {
        onSaveAsClicked();
        return;
    }

    m_canvas->saveImage();

    statusBar()->showMessage("Saved to " + m_canvas->getFilePath(), 3000);
}

void MainWindow::onSaveAsClicked()
{
    if (m_currentProjectPath.isEmpty()) {
        QMessageBox::warning(this, "No Project", "Please open or create a project first.");
        return;
    }

    QString assetsDir = m_currentProjectPath + "/assets";
    QString selectedFilter;
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Image As",
        assetsDir + "/Untitled.png",
        "PNG Image (*.png);;JPEG Image (*.jpg);;BMP Image (*.bmp);;All Files (*)",
        &selectedFilter
    );

    if (filePath.isEmpty()) return;

    if (QFileInfo(filePath).absoluteFilePath().startsWith(assetsDir) == false) {
        QMessageBox::critical(this, "Invalid Location", "Please save the image inside the project assets folder.");
        return;
    }

    QString desiredExtension;
    if (selectedFilter.contains("PNG", Qt::CaseInsensitive)) {
        desiredExtension = "png";
    } else if (selectedFilter.contains("JPEG", Qt::CaseInsensitive)) {
        desiredExtension = "jpg";
    } else if (selectedFilter.contains("BMP", Qt::CaseInsensitive)) {
        desiredExtension = "bmp";
    } else {
        desiredExtension = "png";
    }

    QFileInfo fi(filePath);
    if (fi.suffix().isEmpty()) {
        filePath += "." + desiredExtension;
    }

    m_canvas->setFilePath(filePath);
    m_canvas->saveImage();

    auto db = DatabaseManager::instance();
    db->openDatabase("projects_library");

    db->createImage(m_currentProjectPath, (QFileInfo(filePath)).fileName());

    statusBar()->showMessage("Saved to " + filePath, 3000);
    setWindowTitle(QFileInfo(filePath).fileName() + " - My Image Editor");
}

void MainWindow::onExitClicked()
{
    close();
}

void MainWindow::onCopyClicked()
{
    m_canvas->copy();
}

void MainWindow::onCutClicked()
{
    m_canvas->pushUndoState();
    m_canvas->cut();
}

void MainWindow::onPasteClicked()
{
    m_canvas->pushUndoState();
    m_canvas->paste();
}

void MainWindow::onUndoClicked()
{
    m_canvas->undo();
}

void MainWindow::onRedoClicked()
{
    m_canvas->redo();
}

void MainWindow::onRotateLeftCLicked()
{
    QImage src{m_canvas->getImage()};
    if(src.isNull()) { return; }
    m_canvas->pushUndoState();
    m_canvas->setImage(ImageManipulator::rotateLeft(src));
    m_canvas->update();
}

void MainWindow::onRotateRightClicked()
{
    QImage src{m_canvas->getImage()};
    if(src.isNull()) { return; }
    m_canvas->pushUndoState();
    m_canvas->setImage(ImageManipulator::rotateRight(src));
    m_canvas->update();
}

void MainWindow::onRotate180Clicked()
{
    QImage src{m_canvas->getImage()};
    if(src.isNull()) { return; }
    m_canvas->pushUndoState();
    m_canvas->setImage(ImageManipulator::rotate180(src));
    m_canvas->update();
}

void MainWindow::onHorizontalFlipClicked()
{
    QImage src{m_canvas->getImage()};
    if(src.isNull()) { return; }
    m_canvas->pushUndoState();
    m_canvas->setImage(ImageManipulator::flipHorizontally(src));
    m_canvas->update();
}

void MainWindow::onVerticalFlipClicked()
{
    QImage src{m_canvas->getImage()};
    if(src.isNull()) { return; }
    m_canvas->pushUndoState();
    m_canvas->setImage(ImageManipulator::flipVertically(src));
    m_canvas->update();
}

void MainWindow::onCropClicked()
{
    m_canvas->cropSelection();
    m_canvas->update();
}

void MainWindow::onResizeClicked()
{
    showResizeDialog();
}

void MainWindow::onApplyResizeClicked()
{

}

void MainWindow::onSelectClicked()
{
    //m_canvas->setCurrentTool(Canvas::Tool::Select);
    m_canvas->setCurrentTool(GraphicsCanvas::Tool::Select);
}

void MainWindow::onPenClicked()
{
    //m_canvas->setCurrentTool(Canvas::Tool::Pen);
    m_canvas->setCurrentTool(GraphicsCanvas::Tool::Pen);
}

void MainWindow::onFillClicked()
{
    //m_canvas->setCurrentTool(Canvas::Tool::Fill);
    m_canvas->setCurrentTool(GraphicsCanvas::Tool::Fill);
}

void MainWindow::onEraseClicked()
{
    //m_canvas->setCurrentTool(Canvas::Tool::Erase);
    m_canvas->setCurrentTool(GraphicsCanvas::Tool::Erase);
}

void MainWindow::onPickClicked()
{
    //m_canvas->setCurrentTool(Canvas::Tool::Pick);
    m_canvas->setCurrentTool(GraphicsCanvas::Tool::Pick);
}

void MainWindow::onMagnifyClicked()
{
    //m_canvas->setCurrentTool(Canvas::Tool::Magnify);
    m_canvas->setCurrentTool(GraphicsCanvas::Tool::Magnify);
}

void MainWindow::onSelectBrushClicked(int index)
{
    //Canvas::BrushStyle selectedBrush = static_cast<Canvas::BrushStyle>(index);
    GraphicsCanvas::BrushStyle selectedBrush = static_cast<GraphicsCanvas::BrushStyle>(index);
    m_canvas->setBrushStyle(selectedBrush);
}

void MainWindow::onWidthChanged(int width){
    m_canvas->setPenWidth(width);
    m_canvas->setEraserWidth(width);
}

void MainWindow::onColorPanelClicked()
{
    QColor defaultColor = Qt::black;
    QColor chosenColor = QColorDialog::getColor(defaultColor, this, "Select color");
    if(chosenColor.isValid()){
        updateCurrentColorSwatch(chosenColor);
        m_canvas->setColor(chosenColor);
        statusBar()->showMessage(QString("Color set to R=%1, G=%2, B=%3")
                    .arg(chosenColor.red())
                    .arg(chosenColor.green())
                    .arg(chosenColor.blue()), 3000);
    }
}

void MainWindow::updateCurrentColorSwatch(const QColor &color)
{
    if (!m_currentColorButton) return;

    QPixmap pix(24,24);
    pix.fill(color);
    m_currentColorButton->setIcon(QIcon(pix));
}

void MainWindow::onColorPicked(QColor color){
    updateCurrentColorSwatch(color);
    m_canvas->setColor(color);
}

void MainWindow::onFiltersClicked()
{
    showFiltersDialog();
}

void MainWindow::onNoiseReductionClicked()
{
    m_canvas->pushUndoState();
    m_canvas->setImage(FilterApplyer::applyNoiseReduction(m_canvas->getImage()));
}

void MainWindow::onEdgeDetectionClicked()
{
    m_canvas->pushUndoState();
    m_canvas->setImage(FilterApplyer::applyEdgeDetection(m_canvas->getImage()));
}


void MainWindow::setGlobalStyles() {
    QString globalStyle = R"(
        * {
            background: none;
            border: none;
            margin: 0;
            padding: 0;
            color: #333;
            font-family: Arial;
            font-size: 10pt;
        }

        QMainWindow {
            background-color: #f0f0f0;
        }
        /* Menu Bar */
        QMenuBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
              stop:0 #d2d2d2, stop:1 #aaaaaa);
            spacing: 8px;
            padding: 2px 10px;
            font-family: Arial;
            font-size: 14px;
        }
        QMenuBar::item {
            background: transparent;
            padding: 4px 8px;
            margin: 2px;
            color: #333333;
        }
        QMenuBar::item:selected {
            background: #ffffff;
            color: #000000;
        }
        /* Drop-down menus */
        QMenu {
            background-color: #fdfdfd;
            border: 1px solid #b0b0b0;
            margin: 2px;
        }
        QMenu::item:selected {
            background-color: #e0e0e0;
            color: #000000;
        }

        /* ToolBar */
        QToolBar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
              stop:0 #e6e6e6, stop:1 #cccccc);
            border: 1px solid #999999;
        }
        /* QToolButton and QPushButton in the toolbar */
        QToolButton, QPushButton {
            background-color: #ffffff;
            border: 1px solid #999999;
            border-radius: 4px;
            padding: 4px 8px;
            font-family: Arial;
            font-size: 14px;
            color: #333333;
        }
        QToolButton:hover, QPushButton:hover {
            background-color: #f9f9f9;
            border-color: #666666;
        }
        QToolButton:pressed, QPushButton:pressed {
            background-color: #dadada;
        }
        /* QComboBox (brushes) */
        QComboBox {
            background-color: #ffffff;
            border: 1px solid #999999;
            border-radius: 4px;
            padding: 2px 4px;
            font-family: Arial;
            font-size: 14px;
            color: #333333;
        }
        QComboBox:hover {
            border-color: #666666;
        }
        QComboBox QAbstractItemView {
            background-color: #fdfdfd;
            selection-background-color: #e0e0e0;
            border: 1px solid #999999;
        }
        /* Dialog styling */
        QDialog {
            background-color: #fdfdfd;
            border: 1px solid #999999;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #999999;
            border-radius: 6px;
            margin-top: 6px;
            background-color: #fcfcfc;
        }
        QCheckBox, QLabel {
            font-family: Arial;
            font-size: 14px;
        }
        QSpinBox {
            font-family: Arial;
            font-size: 14px;
            color: #333333;
        }
    )";

    this->setStyleSheet(globalStyle);
}

