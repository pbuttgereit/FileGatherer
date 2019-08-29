#include "cdialogmanagefilelist.h"
#include "ui_cdialogmanagefilelist.h"

CDialogManageFileList::CDialogManageFileList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDialogManageFileList)
{
    ui->setupUi(this);
}

CDialogManageFileList::~CDialogManageFileList()
{
    delete ui;
}
