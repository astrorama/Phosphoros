#ifndef PHZQTUI_DATASPACKHANDLER
#define PHZQTUI_DATASPACKHANDLER

#include <vector>
#include <string>
#include <memory>
#include <QProcess>
#include <QWidget>
#include <QObject>
#include "XYDataset/QualifiedName.h"


namespace Euclid {
namespace PhzQtUI {

class DataPackHandler: public QWidget {
  Q_OBJECT
public:
  explicit DataPackHandler(QWidget *parent = 0);

  ~DataPackHandler() {};

  void check(bool force = false);

signals:
  void completed();


private slots:

  void getDPVersionFinished(int, QProcess::ExitStatus);

  void getConflictFinished(int, QProcess::ExitStatus);

  void getResolutionFinished(int, QProcess::ExitStatus);

private:
  QWidget* m_parent;
  std::string m_version_file;
  std::string m_temp_folder;
  std::string m_conflict_file;
  std::string m_resolution_file;
  bool m_force;


};

}
}

#endif // PHZQTUI_DATASPACKHANDLER