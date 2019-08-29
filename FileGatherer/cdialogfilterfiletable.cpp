#include "cdialogfilterfiletable.h"
#include "ui_cdialogfilterfiletable.h"

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QDebug>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QModelIndex>
#include <QRegularExpression>

CDialogFilterFileTable::CDialogFilterFileTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogFilterFileTable),tableId(-1),fileTypeModel(nullptr),filesModel(nullptr)
{
    ui->setupUi(this);

    filesModel=new QSqlQueryModel(this);
    ui->tableView->setModel(filesModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);

    fileTypeModel=new QSqlQueryModel(this);
    ui->listViewFileTypes->setModel(fileTypeModel);
    ui->listViewFileTypes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listViewFileTypes->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listViewFileTypes->setSelectionBehavior(QAbstractItemView::SelectRows);
}

CDialogFilterFileTable::~CDialogFilterFileTable()
{
    delete ui;
}

void CDialogFilterFileTable::setTableId(int id)
{
    tableId=id;
}

int CDialogFilterFileTable::exec()
{
    fillFileTypeList();
    return QDialog::exec();
}

void CDialogFilterFileTable::on_pushButtonRefresh_clicked()
{
    QString query(QString("select id,fileName,filePath,fileDate,fileHash,fileType,fileRevision from fn%1").arg(tableId));

    QString fileTypesClauseFragment=getSelectedFileTypes();
    QString fileNamePattern=ui->lineEditFileNamePattern->text();



    if(ui->checkBoxDistinctContent->isChecked())
    {

        QStringList cols;
        cols << "id" << "fileName" << "filePath" << "fileDate" << "fileType" << "fileRevision";
        for(int i=0;i<cols.count();++i)
        {
            query=query.replace(cols.at(i),QString("group_concat(%1,' | ') as %1").arg(cols.at(i)));
        }
        query=query.replace("select","select count(id) as Redundant, ");
        query=query.append(" group by fileHash order by Redundant asc");

    }

    if(fileTypesClauseFragment.length()>0 || fileNamePattern.length()>0)
    {
        QString tableName=QString("fn%1").arg(tableId);
        QString queryFragment=QString(" %1 where ").arg(tableName);

        if(fileTypesClauseFragment.length()>0 && fileNamePattern.length()>0)
        {
            queryFragment=queryFragment.append(QString(" (%1) and (fileName like '%2')").arg(fileTypesClauseFragment).arg(fileNamePattern));
        }
        else if(fileTypesClauseFragment.length()>0)
        {
            queryFragment=queryFragment.append(fileTypesClauseFragment);
        }
        else
        {
            queryFragment=queryFragment.append(QString(" fileName like '%1'").arg(fileNamePattern));
        }

        query.replace(tableName,queryFragment);
    }

    qDebug() << query;

    filesModel->setQuery(query,QSqlDatabase::database("files"));
    ui->plainTextEditSqlQuery->clear();
    if(!filesModel->query().exec())
    {
        QMessageBox::warning(this,tr("SQL Error"),QString(tr("Error executing:\n%1\n%2")).arg(query).arg(filesModel->lastError().text()));
        ui->plainTextEditSqlQuery->appendPlainText("ERROR:");
    }

    ui->plainTextEditSqlQuery->appendPlainText(query);
}

void CDialogFilterFileTable::on_pushButtonExportList_clicked()
{

}

void CDialogFilterFileTable::on_pushButtonCopySelectedFiles_clicked()
{

}

void CDialogFilterFileTable::on_pushButtonRedundantFiles_clicked()
{

}

void CDialogFilterFileTable::fillFileTypeList()
{
    QSqlDatabase db=QSqlDatabase::database("files");
    QSqlQuery query(db);
    if(query.exec(QString("select label from tableLabel where id=%1").arg(tableId)))
    {
        if(query.next())
        {
            qDebug() << "setting query";
            fileTypeModel->setQuery(QString("select distinct fileType as type from fn%1 order by type asc").arg(tableId),db);
            fileTypeModel->setHeaderData(0,Qt::Horizontal,"File Types");
            fileTypeModel->query().exec();
            qDebug() << fileTypeModel->lastError().text();
        }
        else
        {
            QMessageBox::warning(this,tr("DB Error"),QString(tr("Table for ID %1 not found")).arg(tableId));
            qDebug() << query.lastQuery();
            qDebug() << query.lastError().text();
        }
    }
    else
    {
        QMessageBox::warning(this,tr("DB Error"),tr("Could not read tableLabel"));
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text();
    }
}

QString CDialogFilterFileTable::getSelectedFileTypes()
{
    static QRegularExpression regex("or $");
    QItemSelectionModel *selection=ui->listViewFileTypes->selectionModel();
    QModelIndexList rows=selection->selectedRows();
    qDebug() << "number of selected file types" << rows.count();
    QStringList types;
    QString clauseFragment("");
    for(int i=0;i<rows.count();++i)
    {
        types << fileTypeModel->data(rows.at(i)).toString();
        clauseFragment += QString("fileType='%1' or ").arg(fileTypeModel->data(rows.at(i)).toString());
    }

    return clauseFragment.remove(regex);
}
