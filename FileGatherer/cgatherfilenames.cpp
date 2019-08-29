#include "cgatherfilenames.h"
#include <QSqlDatabase>
#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QFile>
#include <QByteArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QList>
#include <QDebug>

CGatherFileNames::CGatherFileNames(QObject *parent) : QObject(parent)
{
    //extract revision information: stated as revxyz, date or just a number pattern
    regexp << QRegularExpression("(re)?v(\\.|_|\\-| )? ?(\\d(\\.|_)?)+",QRegularExpression::CaseInsensitiveOption)
           << QRegularExpression("_(\\d{2,4}(\\.|_| \\-)){2}\\d{2,4}(\\-|_|\\.)",QRegularExpression::CaseInsensitiveOption)
           << QRegularExpression("_(\\d+\\.){2}\\d+_",QRegularExpression::CaseInsensitiveOption);

    //used to clean extracted revision string (remove characters, unify use of "." and "_" ...)
    regexRevisionClean1.setPattern("[a-z]");
    regexRevisionClean1.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    regexRevisionClean2.setPattern("(\\-|_| )");
    regexRevisionClean2.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    regexRevisionClean3.setPattern("\\.+");
    regexRevisionClean3.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    regexRevisionClean4.setPattern("(^\\.|\\.$)");
    regexRevisionClean4.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
}

void CGatherFileNames::getAllFilesInDirectoryRecursively()
{
    QSettings s;
    QString dir=s.value("TopDir",QDir::homePath()).toString();
    QString topDir=QFileDialog::getExistingDirectory(nullptr,tr("Select top directory"),dir);
    if(topDir.isNull())return;

    emit started();

    s.setValue("TopDir",topDir);

    QStringList fileList;

    //recursively get all files in dir and append to fileList (skip sym-links and .*/ ..*)
    QDir d(topDir);
    QDirIterator it(topDir,QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,QDirIterator::Subdirectories);

    //create table for file names
    QSqlDatabase db=QSqlDatabase::database("files",true);
    if(!db.isOpen())
    {
        QMessageBox::warning(nullptr,tr("DB Error"),tr("Could not open files db"));
        return;
    }
    QString tableLabel=d.dirName();
    QString tablePath=d.absolutePath();

    QSqlQuery query(db);
    if(!query.exec(QString("insert into tableLabel (label,path,ts) values('%1','%2',%3)").arg(tableLabel).arg(tablePath).arg(QDateTime::currentDateTime().toSecsSinceEpoch())))
    {
        QMessageBox::warning(nullptr,tr("DB Error"),tr("Could not insert into tableLabel"));
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
        return;
    }
    if(!query.exec("select max(id) as id from tableLabel"))
    {
            QMessageBox::warning(nullptr,tr("DB Error"),tr("Could not get max(id) from tableLabel"));
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
            return;
    }
    int tableId=-1;
    if(query.next())
    {
        tableId=query.value("id").toInt();
    }
    else
    {
        QMessageBox::warning(nullptr,tr("DB Error"),tr("No entry in tableLabel"));
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
        return;
    }

    if(!query.exec(QString("create table 'fn%1' ("
                           "id integer primary key autoincrement, "
                           "fileName text, "
                           "filePath text, "
                           "fileDate integer, "
                           "fileHash text, "
                           "fileType text, "
                           "fileRevision text)").arg(tableId)))
    {
        QMessageBox::warning(nullptr,tr("DB Error"),QString(tr("Could not create table '%1' for %2")).arg(tableId).arg(tableLabel));
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
        return;
    }

    QCryptographicHash hashme(QCryptographicHash::Sha512);
    db.transaction();
    while(it.hasNext())
    {
        QString f=it.next();
        QFileInfo fi(f);
        QFile *file=new QFile(f);
        file->open(QIODevice::ReadOnly);
        hashme.reset();
        while(!file->atEnd())
        {
            hashme.addData(file->read(8192));
        }

        if(!query.exec(
                    QString("insert into 'fn%1' (fileName,filePath,fileDate,fileHash,fileType,fileRevision) values ('%2','%3','%4','%5','%6','%7')")
                       .arg(tableId)
                       .arg(fi.fileName())
                       .arg(fi.filePath())
                       .arg(fi.fileTime(QFileDevice::FileModificationTime).toString("yyyy-MM-dd hh:mm:ss"))
                       .arg(QString::fromUtf8(hashme.result().toHex()))
                       .arg(fi.suffix().toLower())
                       .arg(fileRevision(fi.fileName()))
                       )
                )
        {
            QMessageBox::warning(nullptr,tr("DB Error"),QString(tr("Could not insert into table %1 (%2)")).arg(tableId).arg(tableLabel));
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
            break;
        }
    }
    db.commit();

    emit newFileTableAdded(tableId,tableLabel);
}

QString CGatherFileNames::fileRevision(const QString &fn)
{
    QRegularExpressionMatch match;
    QString revision;

    for(int i=0;i<regexp.length();++i)
    {
        match=regexp.at(i).match(fn);
        if(match.hasMatch())
        {
            revision=match.captured();
            break;
        }
    }

    /*
    //unify revision string
    qDebug() << "-------------";
    qDebug() << revision;
    revision=revision.remove(regexRevisionClean1); //remove characters
    qDebug() << revision;
    revision=revision.replace(regexRevisionClean2,"."); //replace _-  with .
    qDebug() << revision;
    revision=revision.replace(regexRevisionClean3,"."); //replace multiple . with single .
    qDebug() << revision;
    revision=revision.remove(regexRevisionClean4); //remove leading/trailing dot
    qDebug() << revision;
    */
    return revision;
}

