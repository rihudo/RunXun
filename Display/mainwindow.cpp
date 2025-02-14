#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    init_focus();
    init_chat_record();
    init_user_info();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_focus()
{
    ui->setupUi(this);
    ui->name->setFocus();
}

void MainWindow::init_chat_record()
{
    record_model = new QStandardItemModel();
    ui->chat_record->setModel(record_model);
    // TO_DO TEST
#if 1
    auto* test_item = new QStandardItem("TEST 1");
    test_item->setBackground(QBrush(Qt::GlobalColor::gray));
    record_model->appendRow(test_item);
    record_model->appendRow(new QStandardItem("TEST 2"));
    record_model->appendRow(new QStandardItem("TEST 3"));
    //ui->chat_record->show();
    QObject::connect(ui->chat_record, SIGNAL(indexesMoved(QModelIndexList)), this, SLOT(on_indexesMoved(QModelIndexList)));
#endif
}

void MainWindow::init_user_info()
{
    user_model = new QStandardItemModel();
    ui->user_info->setModel(user_model);
    QObject::connect(ui->user_info, SIGNAL(indexesMoved(QModelIndexList)), this, SLOT(on_indexesMoved(QModelIndexList)));
}

bool MainWindow::check_configuration(const QString& port, const QString& name, const QList<QString>& send_port_list)
{
    for (const auto& send_port : send_port_list)
    {
        if (!send_port.isEmpty() && send_port.toInt() < 1024 || send_port.toInt() > 49151)
        {
            qDebug() << "send_port " << send_port;
            QMessageBox::information(this, "Configuration illegal", "Send part is illegal. Its range should be from 1024 to 49151");
            return false;
        }
    }

    if (port.isEmpty() || port.toInt() < 1024 || port.toInt() > 49151)
    {
        QMessageBox::information(this, "Configuration illegal", "Port is illegal. Its range should be from 1024 to 49151");
        return false;
    }

    if (name.isEmpty())
    {
        QMessageBox::information(this, "Configuration illegal", "Name can't be empty");
        return false;
    }
    return true;
}

void MainWindow::handle_newly_appeared_user(uint32_t uid, const std::string& msg)
{
    // A new user appeared
    if (0 == user_item_list.count(uid))
    {
        // add new item of user information to USER_INFOR window
        auto* new_user_item = new QStandardItem(QString(msg.c_str()));
        new_user_item->setBackground(QBrush(Qt::GlobalColor::green));
        user_model->appendRow(new_user_item);
        user_item_list.emplace(std::make_pair(uid, new_user_item));
    }
    // An existed user is oneline now
    else if (!user_item_list.at(uid)->isEnabled())
    {
        user_item_list.at(uid)->setBackground(QBrush(Qt::GlobalColor::green));
    }
}

void MainWindow::on_start_clicked()
{
    QString local_port = ui->local_port->text();
    QString name = ui->name->text();
    QList<QString> send_port_list{ui->send_port_one->text(), ui->send_port_two->text(), ui->send_port_three->text()};
    // Check whether configuration is legal
    if (!check_configuration(local_port, name, send_port_list))
    {
        return;
    }
    ui->start->setEnabled(false);
    ui->start->setText("STARTED");
    std::list<int> port_list;
    for (auto& port_str : send_port_list)
    {
        if (!port_str.isEmpty())
        {
            port_list.push_back(port_str.toInt());
        }
    }
    logic_manager = new LogicManager(name.toStdString(), local_port.toInt(), port_list);
    // Set callbacks
    logic_manager->set_hello_callback([this](uint32_t uid, const std::string& msg){
        this->handle_newly_appeared_user(uid, msg);});
    logic_manager->set_hello_reply_callback([this](uint32_t uid, const std::string& msg){
        this->handle_newly_appeared_user(uid, msg);});

    logic_manager->start();
}

void MainWindow::on_indexesMoved(const QModelIndexList & index_list)
{
    for (const auto& idx : index_list)
    {
        // TO_DO
        qDebug() << "lhood" << idx.column() << idx.row();
    }
}

