#include "ViewerDialog.h"
#include "../network/UdpListener.h"

#include <QPen>
#include <QLabel>
#include <QtCharts>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <numeric>
#include <limits.h>

QT_CHARTS_USE_NAMESPACE

ViewerDialog::ViewerDialog(QWidget *parent):QDialog{parent}
{
    chartPtr_=new QChart;
    chartPtr_->legend()->hide();

    chartViewPtr_=new QChartView{chartPtr_};
    chartViewPtr_->setRenderHint(QPainter::Antialiasing);

    xAxesPtr_=new QValueAxis;
    xAxesPtr_->setRange(0,4096);
    xAxesPtr_->setTickCount(10);
    chartPtr_->addAxis(xAxesPtr_,Qt::AlignBottom);

    yAxesPtr_=new QValueAxis;
    yAxesPtr_->setRange(0,USHRT_MAX);
    yAxesPtr_->setTickCount(10);
    chartPtr_->addAxis(yAxesPtr_,Qt::AlignLeft);

    startBtnPtr_=new QPushButton{QObject::tr("Start")};
    QObject::connect(startBtnPtr_,&QPushButton::clicked,[&](){
        if(udpListenerPtr_ && udpListenerPtr_->isRunning()){
            udpListenerPtr_.reset();
        }
        udpListenerPtr_.reset(new UdpListener);
        QObject::connect(udpListenerPtr_.get(),&UdpListener::datagramSignal,
                         this,&ViewerDialog::datagramSlot);
        udpListenerPtr_->start();
        logLabelPtr_->setText(QObject::tr("Listen"));
    });
    stopBtnPtr_=new QPushButton{QObject::tr("Stop")};
    QObject::connect(stopBtnPtr_,&QPushButton::clicked,[&](){
        logLabelPtr_->setText(QObject::tr("Wait"));
        if(udpListenerPtr_ && udpListenerPtr_->isRunning()){
            udpListenerPtr_.reset();
        }
    });

    logLabelPtr_=new QLabel;
    logLabelPtr_->setText(QObject::tr("Wait"));

    QHBoxLayout* hboxLauoutPtr {new QHBoxLayout};
    hboxLauoutPtr->addWidget(new QLabel(QObject::tr("Status:")));
    hboxLauoutPtr->addWidget(logLabelPtr_);
    hboxLauoutPtr->addStretch(100);
    hboxLauoutPtr->addWidget(startBtnPtr_);
    hboxLauoutPtr->addWidget(stopBtnPtr_);

    QVBoxLayout* vboxLayoutPtr {new QVBoxLayout};
    vboxLayoutPtr->addWidget(chartViewPtr_,10);
    vboxLayoutPtr->addLayout(hboxLauoutPtr);

    setLayout(vboxLayoutPtr);
    setWindowTitle("Wave Viewer");
    setWindowFlags(Qt::Window);
    resize(800,600);
}

ViewerDialog::~ViewerDialog()
{
    if(udpListenerPtr_ && udpListenerPtr_->isRunning()){
        udpListenerPtr_.reset();
    }
}

void ViewerDialog::datagramSlot(const QString &senderAddress, qint32 senderPort, const QByteArray datagramData)
{
    int index {0};
    QByteArray tempData {datagramData};
    QVector<double> samplesList {};
    auto secondPartIt {std::stable_partition(tempData.begin(),tempData.end(),[&index](char){
            return ((index++) % 2==0);
        })};
    if(secondPartIt!=tempData.end()){
        auto firstPartIt {tempData.begin()};
        while(secondPartIt!=tempData.end()){
            const int sample {static_cast<int>(*firstPartIt) * 0x100 + (*secondPartIt)};
            samplesList.push_back(static_cast<double>(sample & 0xFFFF));
            firstPartIt++;
            secondPartIt++;
        }
    }

    QScatterSeries* maxSeriesPtr {new QScatterSeries};
    maxSeriesPtr->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    maxSeriesPtr->setMarkerSize(20);

    QLineSeries* graphSeriesPtr {new QLineSeries};
    graphSeriesPtr->setPointLabelsClipping(false);
    QLineSeries* medianSeriesPtr {new QLineSeries};

    QPen medianSeriesPen {medianSeriesPtr->pen()};
    medianSeriesPen.setStyle(Qt::DotLine);
    medianSeriesPen.setColor(Qt::black);
    medianSeriesPen.setWidth(3);
    medianSeriesPtr->setPen(medianSeriesPen);

    const double max {*std::max_element(samplesList.begin(),samplesList.end())};
    const double sum {std::accumulate(samplesList.begin(),samplesList.end(),0.0,std::plus<double>())};
    const double average {sum/samplesList.size()};

    if(chartPtr_->series().size()!=0){
        chartPtr_->removeAllSeries();
    }
    for(int i=0;i<samplesList.size();++i){
        *graphSeriesPtr<<QPointF(i,samplesList.at(i));
        *medianSeriesPtr<<QPointF(i,average);
        if(samplesList.at(i)==max){
            *maxSeriesPtr<<QPointF(i,max);
        }
    }

    chartPtr_->addSeries(graphSeriesPtr);
    chartPtr_->addSeries(medianSeriesPtr);
    chartPtr_->addSeries(maxSeriesPtr);

    graphSeriesPtr->attachAxis(xAxesPtr_);
    graphSeriesPtr->attachAxis(yAxesPtr_);

    medianSeriesPtr->attachAxis(xAxesPtr_);
    medianSeriesPtr->attachAxis(yAxesPtr_);

    maxSeriesPtr->attachAxis(xAxesPtr_);
    maxSeriesPtr->attachAxis(yAxesPtr_);
}
