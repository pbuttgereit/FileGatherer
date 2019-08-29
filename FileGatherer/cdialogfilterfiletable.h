#ifndef CDIALOGFILTERFILETABLE_H
#define CDIALOGFILTERFILETABLE_H

#include <QDialog>

namespace Ui {
class CDialogFilterFileTable;
}

class CDialogFilterFileTable : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogFilterFileTable(QWidget *parent = nullptr);
    ~CDialogFilterFileTable();

private:
    Ui::CDialogFilterFileTable *ui;
};

#endif // CDIALOGFILTERFILETABLE_H
