#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    printf("======================================NI HAO======================================\n");
    printf("======================================Hello World======================================\n");
    printf("======================================BEIJING======================================\n");
    MainWindow w;
    w.show();
    return a.exec();
}
