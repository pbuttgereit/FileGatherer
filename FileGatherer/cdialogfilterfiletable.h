#ifndef CDIALOGFILTERFILETABLE_H
#define CDIALOGFILTERFILETABLE_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class CDialogFilterFileTable;
}

class QSqlQueryModel;

class CDialogFilterFileTable : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogFilterFileTable(QWidget *parent = nullptr);
    ~CDialogFilterFileTable();

public slots:
    void setTableId(int id);
    int exec();

private slots:
    void on_pushButtonRefresh_clicked();

    void on_pushButtonExportList_clicked();

    void on_pushButtonCopySelectedFiles_clicked();

    void on_pushButtonRedundantFiles_clicked();

private:
    void fillFileTypeList();
    QString getSelectedFileTypes();
    Ui::CDialogFilterFileTable *ui;
    int tableId;
    QSqlQueryModel *fileTypeModel,*filesModel;
};

#endif // CDIALOGFILTERFILETABLE_H
