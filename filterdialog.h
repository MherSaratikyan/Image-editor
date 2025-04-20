#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QListWidget>

class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FilterDialog(QWidget *parent = nullptr);
    ~FilterDialog();

    QString selectedFilter() const;

private:
    QListWidget *m_listWidget;
};

#endif // FILTERDIALOG_H
