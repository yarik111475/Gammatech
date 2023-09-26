#include "ViewerDialog.h"
#include "../network/UdpListener.h"

#include <QPen>
#include <QLabel>
#include <QtCharts>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <algorithm>
#include <numeric>

QT_CHARTS_USE_NAMESPACE

ViewerDialog::ViewerDialog(QWidget *parent):QDialog{parent}
{
    chartPtr=new QChart;
    chartViewPtr_=new QChartView{chartPtr};
    chartViewPtr_->setRenderHint(QPainter::Antialiasing);

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
        chartPtr->zoomIn();
    });
    zoomOutBtnPtr_=new QPushButton{QObject::tr("Soom Out")};
    QObject::connect(zoomOutBtnPtr_,&QPushButton::clicked,[&](){
        chartPtr->zoomOut();
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
    qDebug()<<datagramData.size();
    int index {0};
    QByteArray tempData {datagramData};
    QVector<double> samplesList {};
    auto secondPartIt {std::stable_partition(tempData.begin(),tempData.end(),[&index](char){
            return ((index++) % 2==0);
        })};
    if(secondPartIt!=tempData.end()){
        auto firstPartIt {tempData.begin()};
        while(secondPartIt!=tempData.end()){
            const int sample {static_cast<quint16>(*firstPartIt) * 0x100 + (*secondPartIt)};
            samplesList.push_back(static_cast<double>(sample & 0xFFFF));
            firstPartIt++;
            secondPartIt++;
        }
    }

    if(chartPtr->series().size()!=0){
        chartPtr->removeAllSeries();
    }
    QScatterSeries* maxSeriesPtr {new QScatterSeries};
    QLineSeries* graphSeriesPtr {new QLineSeries};
    QLineSeries* medianSeriesPtr {new QLineSeries};

    QPen medianSeriesPen {medianSeriesPtr->pen()};
    medianSeriesPen.setStyle(Qt::DotLine);
    medianSeriesPen.setColor(Qt::black);
    medianSeriesPen.setWidth(3);
    medianSeriesPtr->setPen(medianSeriesPen);

    const double max {*std::max_element(samplesList.begin(),samplesList.end())};
    const double sum {std::accumulate(samplesList.begin(),samplesList.end(),0.0,std::plus<double>())};
    const double average {sum/samplesList.size()};
    for(int i=0;i<samplesList.size();++i){
        *graphSeriesPtr<<QPointF(i,samplesList.at(i));
        *medianSeriesPtr<<QPointF(i,average);
        if(samplesList.at(i)==max){
            *maxSeriesPtr<<QPointF(i,max);
        }
    }

    chartPtr->addSeries(graphSeriesPtr);
    chartPtr->addSeries(medianSeriesPtr);
    chartPtr->addSeries(maxSeriesPtr);
    chartPtr->createDefaultAxes();
}
