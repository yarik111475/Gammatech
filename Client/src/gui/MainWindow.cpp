#include "MainWindow.h"
#include "ViewerDialog.h"

#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QMenu* mainMenuPtr {new QMenu{QObject::tr("Tools"),this}};
    mainMenuPtr->addAction(QObject::tr("Wave Viewer"),[&](){
        ViewerDialog dialog{this};
        dialog.exec();
    });
    mainMenuPtr->addAction(QObject::tr("About"),[&](){
    });
    menuBar()->addMenu(mainMenuPtr);
}
