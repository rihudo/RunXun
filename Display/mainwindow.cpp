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
    model = new QStandardItemModel();
    ui->chat_record->setModel(model);
    // TO_DO TEST
#if 1
    model->appendRow(new QStandardItem("TEST 1"));
    model->appendRow(new QStandardItem("TEST 2"));
    model->appendRow(new QStandardItem("TEST 3"));
    ui->chat_record->show();
#endif
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
    logic_manager->start();
}

