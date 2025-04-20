#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QDialogButtonBox>

class ResizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResizeDialog(QWidget *parent = nullptr);
    ~ResizeDialog();

    // Returns whether the user chose pixel-based resizing
    bool isPixelBased() const;

    // Returns the width/height in pixels from the pixel-based UI
    int pixelWidth() const;
    int pixelHeight() const;

    // Returns whether the user chose percentage-based resizing
    bool isPercentageBased() const;

    // Returns the width/height in percentages from the percentage-based UI
    int percentageWidth() const;
    int percentageHeight() const;

private slots:
    // Toggle which section is active when checkboxes are toggled
    void onPixelCheckToggled(bool checked);
    void onPercentCheckToggled(bool checked);

private:
    QCheckBox *m_pixelCheck;
    QSpinBox  *m_pixelWidthSpin;
    QSpinBox  *m_pixelHeightSpin;

    QCheckBox *m_percentCheck;
    QSpinBox  *m_percentWidthSpin;
    QSpinBox  *m_percentHeightSpin;

    QDialogButtonBox *m_buttonBox;
};

#endif // RESIZEDIALOG_H
