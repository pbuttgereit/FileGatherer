#include "cdialogmanagefilelist.h"
#include "ui_cdialogmanagefilelist.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableView>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QModelIndex>
#include <QSqlRecord>

CDialogManageFileList::CDialogManageFileList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogManageFileList)
{
    ui->setupUi(this);

    QSqlDatabase db=QSqlDatabase::database("files");
    QSqlTableModel *model=new QSqlTableModel(this,db);
    model->setTable("tableLabel");
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

CDialogManageFileList::~CDialogManageFileList()
{
    delete ui;
}

int CDialogManageFileList::exec()
{
    dynamic_cast<QSqlTableModel*>(ui->tableView->model())->select();
    return QDialog::exec();
}

void CDialogManageFileList::on_buttonBox_accepted()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if(select->hasSelection())
    {
        QModelIndexList rows=select->selectedRows();
        QSqlTableModel *model=dynamic_cast<QSqlTableModel*>(ui->tableView->model());
        QSqlQuery query(model->database());
        bool tableAltered=false;
        for(int i=0;i<rows.count();++i)
        {
            int id=model->record(rows.at(i).row()).value("id").toInt();
            model->database().transaction();
            if(query.exec(QString("delete from tableLabel where id=%1").arg(id)))
            {
                if(query.exec(QString("drop table 'fn%1'").arg(id)))
                {
                    model->database().commit();
                    emit filesTableDropped(id);
                    tableAltered=true;
                }
            }
            else
            {
                model->database().rollback();
            }
        }
        if(tableAltered)emit filesTableAltered();
    }
}
