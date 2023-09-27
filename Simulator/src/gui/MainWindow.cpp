#include "MainWindow.h"
#include "../network/UdpServer.h"

#include <QLabel>
#include <QSlider>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    {
        sliderPtr_=new QSlider;
        sliderPtr_->setRange(5,1000);
        sliderPtr_->setTickPosition(QSlider::TicksBelow);
        sliderPtr_->setTickInterval(200);
        sliderPtr_->setOrientation(Qt::Horizontal);
        periodLblPtr_=new QLabel;
        QObject::connect(sliderPtr_,&QSlider::valueChanged,[&](int value){
            periodLblPtr_->setText(QString("%1 ms").arg(value));
            if(udpServerPtr_){
                udpServerPtr_->timeoutSlot(value);
            }
        });
        sliderPtr_->setValue(5);
        periodLblPtr_->setText(QString("%1 ms").arg(sliderPtr_->value()));

        startBtnPtr_=new QPushButton(QObject::tr("Start"));
        QObject::connect(startBtnPtr_,&QPushButton::clicked,[&](){
            if(udpServerPtr_ && udpServerPtr_->isRunning()){
                udpServerPtr_.reset();
                startBtnPtr_->setText(QObject::tr("Start"));
            }
            else{
                udpServerPtr_.reset(new UdpServer{sliderPtr_->value()});
                QObject::connect(udpServerPtr_.get(),&UdpServer::progressSignal,this,&MainWindow::progressSlot);
                udpServerPtr_->start();
                startBtnPtr_->setText(QObject::tr("Stop"));
            }
        });
        stopBtnPtr_=new QPushButton(QObject::tr("Reset"));
        QObject::connect(stopBtnPtr_,&QPushButton::clicked,[&](){
            textEditPtr_->clear();
            if(udpServerPtr_ && udpServerPtr_->isRunning()){
                udpServerPtr_.reset(new UdpServer{sliderPtr_->value()});
                QObject::connect(udpServerPtr_.get(),&UdpServer::progressSignal,this,&MainWindow::progressSlot);
                udpServerPtr_->start();
            }
        });

        QHBoxLayout* hboxLayoutPtr {new QHBoxLayout};
        hboxLayoutPtr->addWidget(new QLabel(QObject::tr("Period:")));
        hboxLayoutPtr->addWidget(periodLblPtr_);
        hboxLayoutPtr->addWidget(sliderPtr_);
        hboxLayoutPtr->addWidget(startBtnPtr_);
        hboxLayoutPtr->addWidget(stopBtnPtr_);

        textEditPtr_=new QTextEdit;
        textEditPtr_->setReadOnly(true);

        QVBoxLayout*vboxLayoutPtr {new QVBoxLayout};
        vboxLayoutPtr->addWidget(textEditPtr_,0);
        vboxLayoutPtr->addLayout(hboxLayoutPtr,0);
        setLayout(vboxLayoutPtr);
    }
    setWindowTitle(QObject::tr("Simulator"));
}

MainWindow::~MainWindow()
{
    if(udpServerPtr_ && udpServerPtr_->isRunning()){
        udpServerPtr_.reset();
    }
}

void MainWindow::progressSlot(qint32 value)
{
    const QString& text {QString("Send %1 datagrams").arg(value)};
    textEditPtr_->setText(text);
}
