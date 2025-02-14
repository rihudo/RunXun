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
    void on_indexesMoved(const QModelIndexList &);

private:
    void init_focus();
    void init_chat_record();
    void init_user_info();
    bool check_configuration(const QString& port, const QString& name, const QList<QString>& send_port_list);
    void handle_newly_appeared_user(uint32_t uid, const std::string& msg);

private:
    Ui::MainWindow *ui;
    QStandardItemModel* record_model;
    QStandardItemModel* user_model;
    LogicManager* logic_manager;
    std::unordered_map<uint32_t, QStandardItem*> user_item_list;
};
#endif // MAINWINDOW_H
