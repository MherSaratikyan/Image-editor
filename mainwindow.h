#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QToolButton>

//#include "canvas.h"
#include "graphicscanvas.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    const QByteArray PROJECT_MAGIC_NUMBER = QByteArrayLiteral("\x1A\xBC\xDE\xF0MyProj");
    //const int PROJECT_FILE_VERSION = 1;
    QMenu* m_fileMenu;
    QAction* m_newFileAction;
    QAction* m_openFileAction;
    QMenu* m_editMenu;
    QAction* m_undoOption;
    QAction* m_redoOption;
    QLabel* m_filtersLabel;
    QToolBar* m_toolBar;
    QToolButton* m_currentColorButton;
//    Canvas* m_canvas;
    GraphicsCanvas* m_canvas;
    QString m_currentFilePath;
    QString m_currentProjectPath;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //Utilities for main window initialization
    void buildMenuBar();
    void buildFileMenu();
    void buildEditMenu();

    void createMainToolBar();
    void buildGeometricOperationsSection();
    void buildSizeSelector();
    void buildToolsSection();
    void buildBrushMenu();
    void buildColorMenu();
    void buildFilterMenu();
    void buildAdvancedOperationsSection();

    //Dialog builders
    void showResizeDialog();
    void showFiltersDialog();

private:
   //Stylization utilities
    void setGlobalStyles();
private slots:
    //File menu options' slots
    //void onNewClicked();
    void onNewFileClicked();
    void onNewProjectClicked();

    //void onOpenClicked();
    void onOpenFileClicked();
    void onOpenProjectCLicked();

    void onSaveClicked();
    void onSaveAsClicked();
    void onExitClicked();

    //Edit menu options' slots
    void onCopyClicked();
    void onCutClicked();
    void onPasteClicked();

    //Utilities' slots
    void onUndoClicked();
    void onRedoClicked();

    //Geometric operations' slots
    void onRotateLeftCLicked();
    void onRotateRightClicked();
    void onRotate180Clicked();
    void onHorizontalFlipClicked();
    void onVerticalFlipClicked();
    void onCropClicked();
    void onResizeClicked();
    void onApplyResizeClicked();

    //Tools' slots
    void onSelectClicked();
    void onPenClicked();
    void onFillClicked();
    void onEraseClicked();
    void onPickClicked();
    void onMagnifyClicked();

    //Brushes' slots
    void onSelectBrushClicked(int index);
    void onWidthChanged(int width);

    //Color panel's slot
    void onColorPanelClicked();
    void onColorPicked(QColor col);
    void updateCurrentColorSwatch(const QColor &color);
    //Filters' slots
    void onFiltersClicked();
    void onFilterChosen(const QString& filterName);

    //Advanced options' slots
    void onNoiseReductionClicked();
    void onEdgeDetectionClicked();
//    void onCreateProcessClicked();
//    void onExecuteProcessClicked();
};
#endif // MAINWINDOW_H
