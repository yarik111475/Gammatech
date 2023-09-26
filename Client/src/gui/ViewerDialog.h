#ifndef VIEWERDIALOG_H
#define VIEWERDIALOG_H

#include <QVector>
#include <QDialog>
#include <QSharedPointer>


class QLineEdit;
class QCustomPlot;
class QPushButton;
class UdpListener;

namespace QtCharts{
    class QChart;
    class QChartView;
    class QValueAxis;
    class QLineSeries;
    class QScatterSeries;
}
class ViewerDialog : public QDialog
{
    Q_OBJECT
private:
    const qint32 samplesSize_  {4096};
    QtCharts::QChart* chartPtr_             {nullptr};
    QtCharts::QChartView* chartViewPtr_     {nullptr};
    QtCharts::QLineSeries* graphSeriesPtr_  {nullptr};
    QtCharts::QLineSeries* medianSeriesPtr_ {nullptr};
    QtCharts::QValueAxis* xAxesPtr_         {nullptr};
    QtCharts::QValueAxis* yAxesPtr_         {nullptr};

    QPushButton* startBtnPtr_   {nullptr};
    QPushButton* stopBtnPtr_    {nullptr};
    QPushButton* zoomInBtnPtr_  {nullptr};
    QPushButton* zoomOutBtnPtr_ {nullptr};
    QLineEdit* logLineEditPtr_  {nullptr};
    QSharedPointer<UdpListener> udpListenerPtr_ {nullptr};
public:
    explicit ViewerDialog(QWidget* parent=nullptr);
    ~ViewerDialog();

public Q_SLOTS:
    void datagramSlot(const QString& senderAddress,qint32 senderPort,const QByteArray datagramData);
};

#endif // VIEWERDIALOG_H
