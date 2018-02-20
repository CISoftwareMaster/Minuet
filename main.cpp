#include "mpmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MPMainWindow w;
    w.show();

    return a.exec();
}
