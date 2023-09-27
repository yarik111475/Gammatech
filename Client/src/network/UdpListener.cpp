#include "UdpListener.h"

#include <QDebug>
#include <QString>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>

void UdpListener::run()
{
    socketPtr_.reset(new QUdpSocket);
    QObject::connect(this,&UdpListener::finished,[&](){
        if(socketPtr_){
            socketPtr_.reset();
        }
    });
    QObject::connect(socketPtr_.get(),&QUdpSocket::readyRead,[&](){
        while(socketPtr_->hasPendingDatagrams()){
            const qint64& datagramSize {socketPtr_->pendingDatagramSize()};
            if(datagramSize!=datagramSize_){
                continue;
            }
            const QNetworkDatagram& datagram {socketPtr_->receiveDatagram()};
            const QString& senderAddress {datagram.senderAddress().toString()};
            const qint32& senderPort {datagram.senderPort()};
            const QByteArray& datagramData {datagram.data()};
            Q_EMIT datagramSignal(senderAddress,senderPort,datagramData);
        }
    });
    const bool isBindOk {socketPtr_->bind(clientAddress_,clientPort_)};
    if(!isBindOk){
        const QString& msg {QString("Fail to bind to: '%1:%2'").arg(clientAddress_.toString()).arg(clientPort_)};
        QThread::quit();
    }
    QThread::exec();
}

UdpListener::UdpListener(QObject *parent)
    :QThread{parent}
{
}

UdpListener::~UdpListener()
{
    QThread::quit();
    QThread::wait();
}
