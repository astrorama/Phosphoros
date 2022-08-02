#include "PhzQtUI/MainWindow.h"
#include <QApplication>
#include <QString>
#include <QVector>

int main(int argc, char* argv[]) {
  QLocale::setDefault(QLocale::c());
  QApplication a(argc, argv);

  typedef QVector<QString> QStringVector;
  qRegisterMetaType<QStringVector>("QStringVector");

  Euclid::PhzQtUI::MainWindow w;
  w.show();

  return a.exec();
}
