#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , last_selected_user_item(nullptr)
{
    init_focus();
    init_user_info();
    init_text_edit();
    ui->chat_record->setSpacing(10);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete logic_manager;
}

void MainWindow::init_focus()
{
    ui->setupUi(this);
    ui->name->setFocus();
}

void MainWindow::init_user_info()
{
    QObject::connect(ui->user_info, &QListWidget::itemClicked, this, &MainWindow::on_user_item_clicked);
}

void MainWindow::init_text_edit()
{
    QObject::connect(ui->input_box, &QTextEdit::textChanged, this, &MainWindow::check_enter);
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
        QListWidgetItem* new_item = new QListWidgetItem(QString(msg.c_str()), ui->user_info);
        new_item->setBackground(Qt::GlobalColor::green);
        new_item->setFlags(new_item->flags() & ~Qt::ItemIsSelectable);
        user_item_list.emplace(std::make_pair(uid, new_item));
        reverse_user_item_list.emplace(std::make_pair(new_item, uid));
        ui->user_info->addItem(new_item);
    }
    // An existed user is oneline now
    else if (user_item_list.at(uid)->background().color() == Qt::GlobalColor::gray)
    {
        user_item_list.at(uid)->setBackground(Qt::GlobalColor::green);
        user_item_list.at(uid)->setText(msg.c_str());
    }
}

void MainWindow::handle_departed_user(uint32_t uid, const std::string& msg)
{
    (void)msg;
    if (0 != user_item_list.count(uid))
    {
        user_item_list.at(uid)->setBackground(Qt::GlobalColor::gray);
    }
}

void MainWindow::handle_new_msg(uint32_t uid, const std::string& msg)
{
    QListWidgetItem* user_item = user_item_list.at(uid);
    if (ui->user_info->currentItem() == user_item)
    {
        add_chat_record(false, msg.c_str());
    }
    else
    {
        user_item->setBackground(Qt::GlobalColor::red);
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
    logic_manager->set_bye_callback([this](uint32_t uid, const std::string& msg){
        this->handle_departed_user(uid, msg);});
    logic_manager->set_msg_callback([this](uint32_t uid, const std::string& msg){
        this->handle_new_msg(uid, msg);});

    const auto* existed_users = logic_manager->get_existed_name_map();
    for (const auto& kv : *existed_users)
    {
        qDebug() << "kv: " << kv.first << " " << kv.second;
        QListWidgetItem* existed_item = new QListWidgetItem(QString(kv.second.c_str()), ui->user_info);
        existed_item->setBackground(Qt::GlobalColor::gray);
        existed_item->setFlags(existed_item->flags() & ~Qt::ItemIsSelectable);
        user_item_list.emplace(std::make_pair(kv.first, existed_item));
        reverse_user_item_list.emplace(std::make_pair(existed_item, kv.first));
        ui->user_info->addItem(existed_item);
    }

    logic_manager->start();
}


void MainWindow::on_user_item_clicked(QListWidgetItem* item)
{
    ui->peer_info->setText(item->text());

    if (item != last_selected_user_item)
    {
        if (last_selected_user_item)
        {
            QFont last_font = item->font();
            last_font.setUnderline(false);
            last_selected_user_item->setFont(last_font);
        }

        QFont select_font = item->font();
        select_font.setUnderline(true);
        item->setFont(select_font);
        last_selected_user_item = item;
    }
    // get chat record of the chosen user
    ui->chat_record->clear();
    uint32_t uid = reverse_user_item_list.at(item);
    logic_manager->load_user(uid);
    if (item->background().color() == Qt::GlobalColor::red)
    {
        item->setBackground(logic_manager->get_existed_name_map()->count(uid) == 0 ? Qt::GlobalColor::gray : Qt::GlobalColor::green);
    }
    const std::vector<ChatRecordEntry>* chat_records = logic_manager->get_records(uid);
    for (const ChatRecordEntry& record_entry : *chat_records)
    {
        add_chat_record(record_entry.is_self, record_entry.info.c_str());
    }
}

// Currently not support inputting Enter key to input box
void MainWindow::check_enter()
{
    // 10: The ASCII code of ENTER
    QString text = ui->input_box->toPlainText();
    if (1 == text.size() && 10 == text.back().toLatin1())
    {
        ui->input_box->setText("");
        return;
    }

    if (2 > text.size() || 10 != text.back().toLatin1())
    {
        return;
    }
    ui->input_box->setText(text.removeLast());
    QTextCursor new_cursor(ui->input_box->document());
    new_cursor.movePosition(QTextCursor::End);
    ui->input_box->setTextCursor(new_cursor);
    try_to_send(text.toStdString().c_str());
}

void MainWindow::try_to_send(const char* msg)
{
    if (ui->start->isEnabled())
    {
        QMessageBox::information(this, "Not ready", "Please click START button first");
        return;
    }

    if (nullptr == ui->user_info->currentItem())
    {
        QMessageBox::information(this, "Not ready", "Please chose a user first");
        return;
    }
    uint32_t uid = reverse_user_item_list.at(ui->user_info->currentItem());
    if (0 < logic_manager->send_msg(uid, msg))
    {
        add_chat_record(true, msg);
    }
    ui->input_box->setText("");
}

void MainWindow::on_send_msg_clicked()
{
    QString msg = ui->input_box->toPlainText();
    if (msg.isEmpty())
    {
        return;
    }
    try_to_send(msg.toStdString().c_str());
}

void MainWindow::add_chat_record(bool is_self, const char* msg)
{
    QListWidgetItem* tmp_item = new QListWidgetItem(ui->chat_record);
    tmp_item->setFlags(tmp_item->flags() & ~Qt::ItemIsSelectable);
    tmp_item->setText(QString(msg));
    QFont font;
    font.setPointSize(18);
    tmp_item->setFont(font);
    if (is_self)
    {
        tmp_item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    ui->chat_record->addItem(tmp_item);
}

