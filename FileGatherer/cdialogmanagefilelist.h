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

public slots:
    int exec();

signals:
    void filesTableDropped(int id);
    void filesTableAltered();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CDialogManageFileList *ui;
};

#endif // CDIALOGMANAGEFILELIST_H
