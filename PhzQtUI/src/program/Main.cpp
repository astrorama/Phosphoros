#include "PhzQtUI/MainWindow.h"
#include <QApplication>
#include <QSettings>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Euclid::PhzQtUI::MainWindow w;
    w.show();


    // default value for the root-path
    QSettings settings("SDC-CH", "PhosphorosUI");

    std::string test_value = "default";
    if (test_value.compare(settings.value(QString::fromStdString("General/root-path"), QString::fromStdString(test_value)).toString().toStdString())==0){

        settings.beginGroup("General");
        settings.setValue(QString::fromStdString("root-path"), QDir::currentPath());
        settings.endGroup();
    }

    return a.exec();
}