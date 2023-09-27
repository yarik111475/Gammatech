#include "MainWindow.h"
#include "ViewerDialog.h"

#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QMenu* mainMenuPtr {new QMenu{QObject::tr("Tools"),this}};
    mainMenuPtr->addAction(QObject::tr("Wave Viewer"),[&](){
        ViewerDialog dialog{this};
        dialog.exec();
    });
    mainMenuPtr->addAction(QObject::tr("About"),[&](){
        QMessageBox::about(this,"About","WaveViewer test program\nVersion 1.0.0");
    });
    menuBar()->addMenu(mainMenuPtr);
}
