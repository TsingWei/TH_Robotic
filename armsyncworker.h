#ifndef ARMSYNCWORKER_H
#define ARMSYNCWORKER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>

struct rawData
{
    double R[3][3];
    double p[3];
};

class ArmSyncWorker : public QObject
{
    Q_OBJECT
public:
    explicit ArmSyncWorker(QObject *parent = nullptr);
    ~ArmSyncWorker();
    void convertData(rawData*);


private:
    QTcpServer *tcpServer; //定义监听套接字tcpServer
    QList<QTcpSocket*> tcpClient;
    QTcpSocket *tcpSocket; //定义通信套接字tcpSocket
private slots:
    void ServerNewConnection();
signals:
    void recvData(rawData);
};



#endif // ARMSYNCWORKER_H
