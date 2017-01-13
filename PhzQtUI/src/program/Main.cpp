#include "PhzQtUI/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Euclid::PhzQtUI::MainWindow w;
    w.show();

    return a.exec();
}
