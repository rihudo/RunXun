#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <logic_manager.hpp>
#include <QListWidget>
#include <unordered_map>

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

    void on_send_msg_clicked();

private:
    void init_focus();
    void init_user_info();
    void init_text_edit();
    bool check_configuration(const QString& port, const QString& name, const QList<QString>& send_port_list);
    void handle_newly_appeared_user(uint32_t uid, const std::string& msg);
    void handle_departed_user(uint32_t uid, const std::string& msg);
    void handle_new_msg(uint32_t uid, const std::string& msg);
    void on_user_item_clicked(QListWidgetItem*);
    void check_enter();
    void try_to_send(const char* msg);
    void add_chat_record(bool is_self, const char* msg);


private:
    Ui::MainWindow *ui;
    LogicManager* logic_manager;
    std::unordered_map<uint32_t, QListWidgetItem*> user_item_list;
    std::unordered_map<QListWidgetItem*, uint32_t> reverse_user_item_list;
    QListWidgetItem* last_selected_user_item;
};
#endif // MAINWINDOW_H
