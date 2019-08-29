#ifndef CGATHERFILENAMES_H
#define CGATHERFILENAMES_H

#include <QObject>
#include <QRegularExpression>
#include <QList>

class CGatherFileNames : public QObject
{
    Q_OBJECT
public:
    explicit CGatherFileNames(QObject *parent = nullptr);

signals:
    void started();
    void newFileTableAdded(int tableId,const QString &tableLabel);

public slots:
    void getAllFilesInDirectoryRecursively();

private:
    QString fileRevision(const QString &fn);
    QList<QRegularExpression> regexp;
    QRegularExpression regexRevisionClean1,regexRevisionClean2,regexRevisionClean3,regexRevisionClean4;
};

#endif // CGATHERFILENAMES_H
