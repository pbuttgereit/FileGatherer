#ifndef CDIALOGMANAGEFILELIST_H
#define CDIALOGMANAGEFILELIST_H

#include <QDialog>

namespace Ui {
class CDialogManageFileList;
}

class CDialogManageFileList : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogManageFileList(QWidget *parent = nullptr);
    ~CDialogManageFileList();

private:
    Ui::CDialogManageFileList *ui;
};

#endif // CDIALOGMANAGEFILELIST_H
