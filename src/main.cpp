#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Starting Espanso Helper...";
    
    MainWindow window;
    window.show();
    
    return app.exec();
} 