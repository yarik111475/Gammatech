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
    });
    stopBtnPtr_=new QPushButton{QObject::tr("Stop")};
    QObject::connect(stopBtnPtr_,&QPushButton::clicked,[&](){
        if(udpListenerPtr_ && udpListenerPtr_->isRunning()){
            udpListenerPtr_.reset();
        }
    });
    zoomInBtnPtr_=new QPushButton{QObject::tr("Zoom In")};
    QObject::connect(zoomInBtnPtr_,&QPushButton::clicked,[&](){
        chartPtr_->zoomIn();
    });
    zoomOutBtnPtr_=new QPushButton{QObject::tr("Soom Out")};
    QObject::connect(zoomOutBtnPtr_,&QPushButton::clicked,[&](){
        chartPtr_->zoomOut();
    });
    logLineEditPtr_=new QLineEdit;
    logLineEditPtr_->setReadOnly(true);

    QHBoxLayout* hboxLauoutPtr {new QHBoxLayout};
    hboxLauoutPtr->addWidget(new QLabel(QObject::tr("Status:")));
    hboxLauoutPtr->addWidget(logLineEditPtr_);
    hboxLauoutPtr->addWidget(startBtnPtr_);
    hboxLauoutPtr->addWidget(stopBtnPtr_);
    //hboxLauoutPtr->addWidget(zoomInBtnPtr_);
    //hboxLauoutPtr->addWidget(zoomOutBtnPtr_);

    QVBoxLayout* vboxLayoutPtr {new QVBoxLayout};
    vboxLayoutPtr->addWidget(chartViewPtr_,10);
    vboxLayoutPtr->addLayout(hboxLauoutPtr);

    setLayout(vboxLayoutPtr);
    setWindowTitle("Wave Viewer");
    resize(800,600);
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
