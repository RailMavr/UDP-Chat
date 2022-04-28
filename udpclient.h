#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QMainWindow>
#include <QUdpSocket>
#include <thread>
#include <mutex>
#include <queue>
#include <QKeyEvent>
#include <iostream>
#include <windows.h>

QT_BEGIN_NAMESPACE
namespace Ui { class UDPClient; }
QT_END_NAMESPACE

class UDPClient : public QMainWindow
{
    Q_OBJECT

public:
    UDPClient(QWidget *parent = nullptr);
    ~UDPClient();

    void SendMsg();

    void SafeSendQueuePush(QString str);
    QString SafeSendQueuePop();

public slots:
    void ReadingData();

private slots:
    void keyPressEvent(QKeyEvent *event);

    void on_bt_SendingData_clicked();

    void on_connection_clicked();

    void on_disconnect_clicked();


private:
    Ui::UDPClient *ui;
    QUdpSocket *udpSocket;
    std::thread receiving;
    std::thread sending;
    std::queue <QString> send_queue;
    std::mutex send_mutex;
    bool is_connected = false;
};
#endif // UDPCLIENT_H
