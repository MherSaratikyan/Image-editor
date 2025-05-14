#include "filterdialog.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent)
    , m_listWidget(new QListWidget(this))
{
    setWindowTitle("Choose a filter");

    QStringList filters = {
        "Grayscale",
        "Sepia",
        "Invert",
        "Blur",
        "Brightness",
        "Contrast",
        "Saturation",
        "Desaturation",
        "Hue",
        "Posterize",
        "Solarize",
        "Pixelate",
        "Vignette",
        "Sharpen"
    };

    QStringList tooltips = {
        "Converts the image to shades of gray by removing all color, commonly used for a classic or documentary look.",
        "Applies a warm brown tone reminiscent of old photographs, often used to create a vintage or nostalgic feel.",
        "Inverts all pixel colors, turning light to dark and vice versa, useful for creative or negative-effect edits.",
        "Softens the image by blending neighboring pixels, reducing sharpness and detail for a dreamy appearance.",
        "Increases or decreases the brightness across the image, useful for correcting underexposed or overexposed photos.",
        "Modifies the contrast between light and dark areas to enhance image depth and definition.",
        "Boosts color intensity, making colors more vivid and lively, useful for vibrant scene enhancement.",
        "Reduces color intensity for a more muted, subdued look—great for soft tones or minimalist styles.",
        "Shifts the overall hue (color angle on the color wheel), altering the image’s color palette artistically.",
        "Reduces the number of colors used by mapping them to specific bands—creates a cartoonish or graphic effect.",
        "Bright areas remain bright, but darker areas become inverted—adds a surreal look useful for stylized photos.",
        "Divides the image into large blocks of solid color, mimicking a low-resolution pixel effect for creative purposes.",
        "Darkens corners and edges while keeping the center bright, drawing focus to the center—ideal for portraits.",
        ""
    };

    for (int i = 0; i < filters.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(filters[i]);
        item->setToolTip(tooltips[i]);
        m_listWidget->addItem(item);
    }

    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_listWidget);

    // OK/Cancel buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this
    );
    mainLayout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

FilterDialog::~FilterDialog()
{
    // nothing special needed
}

QString FilterDialog::selectedFilter() const
{
    // Return the text of the currently selected item in the list,
    // or an empty string if nothing is selected
    QListWidgetItem *current = m_listWidget->currentItem();
    return current ? current->text() : QString();
}
