#include "mainwindow.h"
#include <QApplication>
#include <windows.h>
#include <opencv2/dnn.hpp>
#include <QtZlib/zlib.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"Russian");
    QApplication a(argc, argv);
    MainWindow w;
    return a.exec();
}
