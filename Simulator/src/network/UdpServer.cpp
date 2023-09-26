#include "UdpServer.h"

#include <QList>
#include <QDebug>
#include <QTimer>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QRandomGenerator>

void UdpServer::run()
{
   socketPtr_.reset(new QUdpSocket);
   startSlot();
   QThread::exec();
}

UdpServer::UdpServer(qint32 timeOut, QObject *parent)
    : QThread{parent},timeOut_{timeOut}
{
}

UdpServer::~UdpServer()
{
    destructSlot();
}

void UdpServer::startSlot()
{
    if(socketPtr_){
        QVector<quint32> randomList {};
        randomList.resize(datagramSize_);
        const quint32 seed[2]{1,255};
        QRandomGenerator rand(seed,seed[1]);
        rand.fillRange(randomList.data(),randomList.size());
        QByteArray datagramData {};
        std::transform(randomList.begin(),randomList.end(),std::back_inserter(datagramData),[](quint32 value){
            return static_cast<short>(value);
        });
        socketPtr_->writeDatagram(datagramData,hostAddress_,hostPort_);
        Q_EMIT progressSignal(++datagramCounter_);
    }
    QTimer::singleShot(timeOut_ * scaleFactor_,this,&UdpServer::startSlot);
}

void UdpServer::destructSlot()
{
    QThread::quit();
    QThread::wait();
}

void UdpServer::timeoutSlot(qint32 value)
{
    timeOut_=value;
}
