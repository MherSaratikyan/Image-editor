#include "procedure.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QDebug>
#include <QtMath>

Procedure::Procedure(int procId)
    : m_procedureId(procId), m_projectId(-1)
{
}

bool Procedure::loadFromDatabase()
{
    if (m_procedureId < 0)
        return false;
    QSqlQuery query(DatabaseManager::instance()->db());
    query.prepare("SELECT project_id, procedure_name, description, procedure_sequence "
                  "FROM procedures WHERE procedure_id = :id");
    query.bindValue(":id", m_procedureId);
    if (!query.exec() || !query.next()) {
        qDebug() << "Procedure load error:" << query.lastError().text();
        return false;
    }
    m_projectId = query.value(0).toInt();
    m_name = query.value(1).toString();
    m_description = query.value(2).toString();
    m_sequence = query.value(3).toString();
    return true;
}

bool Procedure::createProcedure(int projectId, const QString &name, const QString &desc, const QString &seq)
{
    QSqlQuery query(DatabaseManager::instance()->db());
    query.prepare("INSERT INTO procedures (procedure_name, description, procedure_sequence, pid) "
                  "VALUES (:pid, :name, :desc, :seq)");
    query.bindValue(":pid", projectId);
    query.bindValue(":name", name);
    query.bindValue(":desc", desc);
    query.bindValue(":seq", seq);
    if (!query.exec()) {
        qDebug() << "Procedure creation error:" << query.lastError().text();
        return false;
    }
    QSqlQuery q(DatabaseManager::instance()->db());
    if (q.exec("SELECT last_insert_rowid()")) {
        if (q.next()) {
            m_procedureId = q.value(0).toInt();
            m_projectId = projectId;
            m_name = name;
            m_description = desc;
            m_sequence = seq;
        }
    }
    return (m_procedureId >= 0);
}

bool Procedure::executeOnImage(QImage &img)
{
    if (m_sequence.isEmpty())
        return false;
    QStringList steps = m_sequence.split(",", QString::SkipEmptyParts);
    for (const QString &step : steps) {
        QString s = step.trimmed();
        if (s.compare("Grayscale", Qt::CaseInsensitive) == 0) {
            for (int y = 0; y < img.height(); ++y) {
                for (int x = 0; x < img.width(); ++x) {
                    QColor c = QColor(img.pixel(x, y));
                    int gray = qGray(c.rgb());
                    img.setPixel(x, y, qRgb(gray, gray, gray));
                }
            }
        }
        else if (s.compare("Invert", Qt::CaseInsensitive) == 0) {
            for (int y = 0; y < img.height(); ++y) {
                for (int x = 0; x < img.width(); ++x) {
                    QColor c = QColor(img.pixel(x, y));
                    c.setRgb(255 - c.red(), 255 - c.green(), 255 - c.blue());
                    img.setPixel(x, y, c.rgb());
                }
            }
        }
        else if (s.startsWith("Contrast", Qt::CaseInsensitive)) {
            int start = s.indexOf('(');
            int end = s.indexOf(')');
            double factor = 1.0;
            if (start != -1 && end != -1 && end > start) {
                factor = s.mid(start + 1, end - start - 1).toDouble();
            }
            for (int y = 0; y < img.height(); ++y) {
                for (int x = 0; x < img.width(); ++x) {
                    QColor c = QColor(img.pixel(x, y));
                    int r = qBound(0, int((c.red() - 128) * factor + 128), 255);
                    int g = qBound(0, int((c.green() - 128) * factor + 128), 255);
                    int b = qBound(0, int((c.blue() - 128) * factor + 128), 255);
                    img.setPixel(x, y, qRgb(r, g, b));
                }
            }
        }
        else {
            qDebug() << "Unknown procedure step:" << s;
        }
    }
    return true;
}

int Procedure::procedureId() const { return m_procedureId; }
int Procedure::projectId() const { return m_projectId; }
QString Procedure::name() const { return m_name; }
QString Procedure::description() const { return m_description; }
QString Procedure::sequence() const { return m_sequence; }
