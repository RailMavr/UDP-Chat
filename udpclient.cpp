#include "udpclient.h"
#include "./ui_udpclient.h"

constexpr uint32_t SLEEP_MS = 100;

UDPClient::UDPClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UDPClient)
{
    ui->setupUi(this);

    ui->lineEdit->setDisabled(true);
    ui->disconnect->setDisabled(true);

    ui->local_host->setMaximum(9999);

    ui->remote_host->setMaximum(9999);
    ui->remote_host->setValue(9999);


}

UDPClient::~UDPClient()
{
    delete ui;
}

void UDPClient::on_connection_clicked()
{
    ui->lineEdit->setEnabled(true);
    ui->connection->setDisabled(true);
    ui->disconnect->setEnabled(true);
    is_connected = true;

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, ui->local_host->value());

    sending = std::thread([&](){
        SendMsg();
    });

    receiving =  std::thread([&](){
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(ReadingData()));
    });
}

void UDPClient::on_disconnect_clicked()
{
    ui->lineEdit->setDisabled(true);
    ui->connection->setEnabled(true);
    ui->disconnect->setDisabled(true);
    is_connected = false;

    sending.join();
    receiving.join();

    udpSocket->disconnectFromHost ();
}

void UDPClient::ReadingData()
{
    QHostAddress sender;
    quint16 senderPort;

    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        if(datagram.size() != 0)
        {
            ui->textEdit->append("От " + ui->remote_host->text() + ": <font color=red>" +QString(datagram) + "</font>");
            qDebug()<<datagram.data()<<"IP: " + sender.toString() << "Port: " + QString("%1").arg(senderPort);
        }
    }
}


void UDPClient::on_bt_SendingData_clicked()
{
    if (!(ui->lineEdit->text().isEmpty()))
    {
        SafeSendQueuePush(ui->lineEdit->text());

        ui->textEdit->append("Вы: <font color=green>" + ui->lineEdit->text() + "</font>");
        ui->lineEdit->clear();
    }
}

void UDPClient::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        UDPClient::on_bt_SendingData_clicked();
    }
}

void UDPClient::SendMsg()
{
    while (is_connected)
    {
        Sleep(SLEEP_MS);
        if (!send_queue.empty())
        {
            udpSocket->writeDatagram(SafeSendQueuePop().toUtf8(), QHostAddress::LocalHost, ui->remote_host->value());
        }
    }
}

void UDPClient::SafeSendQueuePush(QString str)
{
     std::lock_guard<std::mutex> lock(send_mutex);
     send_queue.push(str);
}

QString UDPClient::SafeSendQueuePop()
{
     std::lock_guard<std::mutex> lock(send_mutex);
     auto str = send_queue.back();
     send_queue.pop();
     return str;
 }


