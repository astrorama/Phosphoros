#include "PhzQtUI/MainWindow.h"
#include <QApplication>
#include <QString>
#include <QVector>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    typedef QVector<QString> QStringVector;
    qRegisterMetaType<QStringVector>("QStringVector");

    Euclid::PhzQtUI::MainWindow w;
    w.show();

    return a.exec();
}
