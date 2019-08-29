#include "cdialogfilterfiletable.h"
#include "ui_cdialogfilterfiletable.h"

CDialogFilterFileTable::CDialogFilterFileTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogFilterFileTable)
{
    ui->setupUi(this);
}

CDialogFilterFileTable::~CDialogFilterFileTable()
{
    delete ui;
}
