#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QThread>
#include <QHostAddress>
#include <QSharedPointer>

class QUdpSocket;
class UdpServer : public QThread
{
    Q_OBJECT
private:
    const qint32       hostPort_ {10002};
    const QHostAddress hostAddress_ {QHostAddress::LocalHost};
    const qint32       datagramSize_ {8192};
    const qint32       scaleFactor_ {1};

    qint32 timeOut_ {};
    qint32 datagramCounter_ {};
    QSharedPointer<QUdpSocket> socketPtr_ {nullptr};
protected:
    virtual void run()override;

public:
    explicit UdpServer(qint32 timeOut, QObject *parent = nullptr);
    ~UdpServer();

private Q_SLOTS:
    void startSlot();
    void destructSlot();

public Q_SLOTS:
    void timeoutSlot(qint32 value);
Q_SIGNALS:
    void progressSignal(qint32 value);
};

#endif // UDPSERVER_H
