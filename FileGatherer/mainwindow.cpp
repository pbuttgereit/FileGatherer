#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cgatherfilenames.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QCursor>
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QComboBox>
#include <QDebug>

#include "cdialogmanagefilelist.h"
#include "cdialogfilterfiletable.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),gatherFileNames(nullptr),tableModel(nullptr),dialogManageFileList(nullptr),dialogFilterFileTable(nullptr)
{
    ui->setupUi(this);
    gatherFileNames=new CGatherFileNames(this);
    connect(gatherFileNames,SIGNAL(newFileTableAdded(int,const QString &)),this,SLOT(newFileTableAdded(int, const QString &)));
    connect(gatherFileNames,SIGNAL(started()),this,SLOT(fileGatheringStarted()));
    tableModel=new QSqlTableModel(this,QSqlDatabase::database("files"));
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableViewFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewFiles->setModel(tableModel);
    initCombobox();

    dialogManageFileList=new CDialogManageFileList(this);
    connect(dialogManageFileList,SIGNAL(filesTableAltered()),this,SLOT(filesTableAltered()));

    dialogFilterFileTable=new CDialogFilterFileTable(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFileTableAdded(int id, const QString &tableLabel)
{
    Q_UNUSED(tableLabel);

    //add table to comboBoxTables
    QSqlDatabase db=QSqlDatabase::database("files",true);
    if(!db.isOpen())
    {
        QMessageBox::warning(this,tr("DB Error"),tr("Could not open database"));
        qDebug() << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    if(query.exec(QString("select id,label,path,ts from tableLabel where id=%1").arg(id)))
    {
        if(query.next())
        {
            ui->comboBoxTables->addItem(query.value("label").toString(),query.value("id"));
            int idx=ui->comboBoxTables->count()-1;
            QString date(QDateTime::fromSecsSinceEpoch(query.value("ts").toInt()).toString("dd.MM.YYYY hh:mm"));
            ui->comboBoxTables->setItemData(idx,QString("%1 (%2)").arg(query.value("path").toString()).arg(date),Qt::ToolTipRole);
            //load into table view
            ui->comboBoxTables->setCurrentIndex(idx);
            on_comboBoxTables_activated(idx);
        }
    }
    else
    {
        QMessageBox::warning(this,tr("SQL Error"),tr("Could not read tableLabel"));
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
    }
}

void MainWindow::fileGatheringStarted()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void MainWindow::on_actionGather_from_Directory_triggered()
{
    gatherFileNames->getAllFilesInDirectoryRecursively();
    QApplication::restoreOverrideCursor();
}

void MainWindow::initCombobox()
{
    ui->comboBoxTables->clear();
    //init comboBox with tableLabel table
    QSqlDatabase db=QSqlDatabase::database("files",true);
    if(!db.isOpen())
    {
        QMessageBox::warning(this,tr("DB Error"),tr("Could not open database"));
        qDebug() << db.lastError().text();
        return;
    }


    ui->comboBoxTables->addItem("select table...",-1);
    QSqlQuery query(db);
    if(query.exec("select id,label,path,ts from tableLabel order by label asc"))
    {
        int idx=-1;
        while(query.next())
        {
            ui->comboBoxTables->addItem(query.value("label").toString(),query.value("id"));
            idx=ui->comboBoxTables->count()-1;
            QString date(QDateTime::fromSecsSinceEpoch(query.value("ts").toInt()).toString("dd.MM.YYYY hh:mm"));
            ui->comboBoxTables->setItemData(idx,QString("%1 (%2)").arg(query.value("path").toString()).arg(date),Qt::ToolTipRole);
        }
    }
    else
    {
        QMessageBox::warning(this,tr("SQL Error"),tr("Could not read tableLabel"));
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
    }
}

void MainWindow::on_comboBoxTables_activated(int index)
{
    tableModel->setTable("fn"+ui->comboBoxTables->itemData(index).toString());
    tableModel->select();
}

void MainWindow::on_actionFile_Lists_triggered()
{
    Q_ASSERT(dialogManageFileList);
    dialogManageFileList->exec();
}

void MainWindow::filesTableAltered()
{
    initCombobox();
}

void MainWindow::on_pushButtonFilter_clicked()
{
    //open current file table in extra dialog
    dialogFilterFileTable->setTableId(ui->comboBoxTables->currentData().toInt());
    dialogFilterFileTable->exec();
}
