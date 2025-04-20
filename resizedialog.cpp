#include "resizedialog.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ResizeDialog::ResizeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Resize Image");

    // 1) Pixel-based section
    m_pixelCheck      = new QCheckBox("Pixel-based resize", this);
    m_pixelWidthSpin  = new QSpinBox(this);
    m_pixelHeightSpin = new QSpinBox(this);

    // Set some ranges
    m_pixelWidthSpin->setRange(1, 99999);
    m_pixelHeightSpin->setRange(1, 99999);

    // Default values
    m_pixelWidthSpin->setValue(800);
    m_pixelHeightSpin->setValue(600);

    // 2) Percentage-based section
    m_percentCheck      = new QCheckBox("Percentage-based resize", this);
    m_percentWidthSpin  = new QSpinBox(this);
    m_percentHeightSpin = new QSpinBox(this);

    // Ranges for percentage
    m_percentWidthSpin->setRange(1, 1000);
    m_percentHeightSpin->setRange(1, 1000);

    // Default 100% => same size
    m_percentWidthSpin->setValue(100);
    m_percentHeightSpin->setValue(100);

    // By default, let's say we check pixel-based
    m_pixelCheck->setChecked(true);
    m_pixelWidthSpin->setEnabled(true);
    m_pixelHeightSpin->setEnabled(true);

    m_percentCheck->setChecked(false);
    m_percentWidthSpin->setEnabled(false);
    m_percentHeightSpin->setEnabled(false);

    // Layout for pixel-based
    QHBoxLayout *pixelLayout = new QHBoxLayout();
    pixelLayout->addWidget(new QLabel("Width:", this));
    pixelLayout->addWidget(m_pixelWidthSpin);
    pixelLayout->addSpacing(10);
    pixelLayout->addWidget(new QLabel("Height:", this));
    pixelLayout->addWidget(m_pixelHeightSpin);

    // Layout for percentage-based
    QHBoxLayout *percentLayout = new QHBoxLayout();
    percentLayout->addWidget(new QLabel("Width (%):", this));
    percentLayout->addWidget(m_percentWidthSpin);
    percentLayout->addSpacing(10);
    percentLayout->addWidget(new QLabel("Height (%):", this));
    percentLayout->addWidget(m_percentHeightSpin);

    // Connect toggles
    connect(m_pixelCheck,  &QCheckBox::toggled, this, &ResizeDialog::onPixelCheckToggled);
    connect(m_percentCheck,&QCheckBox::toggled, this, &ResizeDialog::onPercentCheckToggled);

    // OK/Cancel buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                       Qt::Horizontal, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Overall layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pixelCheck);
    mainLayout->addLayout(pixelLayout);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_percentCheck);
    mainLayout->addLayout(percentLayout);

    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_buttonBox);
}

ResizeDialog::~ResizeDialog()
{
    // nothing special
}

// If pixel-based is toggled => disable percentage-based
void ResizeDialog::onPixelCheckToggled(bool checked)
{
    // If pixel-based is on, turn percentage off
    if (checked) {
        m_percentCheck->setChecked(false);
    }

    m_pixelWidthSpin->setEnabled(checked);
    m_pixelHeightSpin->setEnabled(checked);
}

// If percentage-based is toggled => disable pixel-based
void ResizeDialog::onPercentCheckToggled(bool checked)
{
    // If percentage-based is on, turn pixels off
    if (checked) {
        m_pixelCheck->setChecked(false);
    }

    m_percentWidthSpin->setEnabled(checked);
    m_percentHeightSpin->setEnabled(checked);
}

// Check which mode is active
bool ResizeDialog::isPixelBased() const
{
    return m_pixelCheck->isChecked();
}

int ResizeDialog::pixelWidth() const
{
    return m_pixelWidthSpin->value();
}

int ResizeDialog::pixelHeight() const
{
    return m_pixelHeightSpin->value();
}

bool ResizeDialog::isPercentageBased() const
{
    return m_percentCheck->isChecked();
}

int ResizeDialog::percentageWidth() const
{
    return m_percentWidthSpin->value();
}

int ResizeDialog::percentageHeight() const
{
    return m_percentHeightSpin->value();
}
