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
        "Vignette"
    };
    // Populate list
    m_listWidget->addItems(filters);

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
