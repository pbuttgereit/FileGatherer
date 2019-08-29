#include "mainwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QObject>
#include <QDir>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE","files");
    QString homePath=QDir::homePath();
    db.setDatabaseName(homePath+"/FileGatherer.db");
    if(!db.open())
    {
        qDebug() << QObject::tr("could not open db: ") << db.databaseName();
        return -1;
    }
    //check/init structure

    QSqlQuery query(db);
    if(!query.exec("create table if not exists 'tableLabel' (id integer primary key autoincrement, label text,path text,ts integer)"))
    {
        qDebug() << QObject::tr("Could not check for or create tableLabel");
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
        return -2;
    }


    MainWindow w;
    a.setApplicationName("FileGatherer");
    a.setApplicationVersion("1.0");
    a.setApplicationDisplayName("FileGatherer");
    a.setOrganizationName("tbd");
    w.show();

    return a.exec();
}
