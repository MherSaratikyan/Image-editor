#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <QString>
#include <QImage>

class Procedure
{
public:
    explicit Procedure(int procId = -1);

    bool loadFromDatabase();
    bool createProcedure(int projectId, const QString &name, const QString &desc, const QString &seq);
    bool executeOnImage(QImage &img);

    int procedureId() const;
    int projectId() const;
    QString name() const;
    QString description() const;
    QString sequence() const;

private:
    int m_procedureId;
    int m_projectId;
    QString m_name;
    QString m_description;
    QString m_sequence;
};

#endif // PROCEDURE_H
