#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <logic_manager.hpp>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();

private:
    void init_focus();
    void init_chat_record();
    bool check_configuration(const QString& port, const QString& name, const QList<QString>& send_port_list);

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model;
    LogicManager* logic_manager;
};
#endif // MAINWINDOW_H
