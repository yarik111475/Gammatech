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
        sliderPtr_.reset(new QSlider);
        sliderPtr_->setRange(5,1000);
        sliderPtr_->setTickPosition(QSlider::TicksBelow);
        sliderPtr_->setTickInterval(200);
        sliderPtr_->setOrientation(Qt::Horizontal);
        periodLblPtr_.reset(new QLabel);
        QObject::connect(sliderPtr_.get(),&QSlider::valueChanged,[&](int value){
            periodLblPtr_->setText(QString("%1 ms").arg(value));
            if(udpServerPtr_){
                udpServerPtr_->timeoutSlot(value);
            }
        });
        sliderPtr_->setValue(5);
        periodLblPtr_->setText(QString("%1 ms").arg(sliderPtr_->value()));

        startBtnPtr_.reset(new QPushButton(QObject::tr("Start")));
        QObject::connect(startBtnPtr_.get(),&QPushButton::clicked,[&](){
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
        stopBtnPtr_.reset(new QPushButton(QObject::tr("Reset")));
        QObject::connect(stopBtnPtr_.get(),&QPushButton::clicked,[&](){
            textEditPtr_->clear();
            if(udpServerPtr_ && udpServerPtr_->isRunning()){
                udpServerPtr_.reset(new UdpServer{sliderPtr_->value()});
                QObject::connect(udpServerPtr_.get(),&UdpServer::progressSignal,this,&MainWindow::progressSlot);
                udpServerPtr_->start();
            }
        });

        QHBoxLayout* hboxLayoutPtr {new QHBoxLayout};
        hboxLayoutPtr->addWidget(new QLabel(QObject::tr("Period:")));
        hboxLayoutPtr->addWidget(periodLblPtr_.get());
        hboxLayoutPtr->addWidget(sliderPtr_.get());
        hboxLayoutPtr->addWidget(startBtnPtr_.get());
        hboxLayoutPtr->addWidget(stopBtnPtr_.get());

        textEditPtr_.reset(new QTextEdit);
        textEditPtr_->setReadOnly(true);

        QVBoxLayout*vboxLayoutPtr {new QVBoxLayout};
        vboxLayoutPtr->addWidget(textEditPtr_.get());
        vboxLayoutPtr->addLayout(hboxLayoutPtr);
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
