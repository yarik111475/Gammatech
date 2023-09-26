#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include <QThread>
#include <QByteArray>
#include <QHostAddress>
#include <QJsonObject>
#include <QSharedPointer>

class QUdpSocket;
class UdpListener : public QThread
{
    Q_OBJECT
private:
    const qint32       datagramSize_  {8192};
    const QHostAddress clientAddress_ {QHostAddress::LocalHost};
    const qint32       clientPort_    {10002};
    QSharedPointer<QUdpSocket> socketPtr_ {nullptr};

protected:
    virtual void run()override;

public:
    explicit UdpListener(QObject* parent=nullptr);
    ~UdpListener();

Q_SIGNALS:
    void datagramSignal(const QString& senderAddress,qint32 senderPort,const QByteArray datagramData);

};

#endif // UDPLISTENER_H
