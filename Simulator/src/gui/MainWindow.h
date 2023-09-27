#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSharedPointer>

class QLabel;
class QSlider;
class QTextEdit;
class QPushButton;
class UdpServer;
class MainWindow : public QWidget
{
    Q_OBJECT

private:
    QSlider*     sliderPtr_    {nullptr};
    QLabel*      periodLblPtr_ {nullptr};
    QTextEdit*   textEditPtr_  {nullptr};
    QPushButton* startBtnPtr_  {nullptr};
    QPushButton* stopBtnPtr_   {nullptr};
    QSharedPointer<UdpServer>   udpServerPtr_ {nullptr};

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private Q_SLOTS:
    void progressSlot(qint32 value);

};

#endif // MAINWINDOW_H
