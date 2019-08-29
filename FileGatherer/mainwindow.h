#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class CGatherFileNames;
class QSqlTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFileTableAdded(int id, const QString &tableLabel);
    void fileGatheringStarted();

    void on_actionGather_from_Directory_triggered();

    void on_comboBoxTables_activated(int index);

private:
    void initCombobox();
    Ui::MainWindow *ui;
    CGatherFileNames *gatherFileNames;
    QSqlTableModel *tableModel;
};

#endif // MAINWINDOW_H
