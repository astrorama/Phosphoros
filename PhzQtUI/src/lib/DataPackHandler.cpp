
#include "PhzQtUI/DataPackHandler.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include "PhzQtUI/DialogConflictingFilesHandling.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("DataPackHandler");

DataPackHandler::DataPackHandler(QWidget* parent) : QWidget(parent) {
  m_parent          = parent;
  m_version_file    = FileUtils::getGUIConfigPath() + "/dp_version.json";
  m_temp_folder     = FileUtils::getRootPath(true) + "Temp/";
  m_conflict_file   = m_temp_folder + "conflict.json";
  m_resolution_file = m_temp_folder + "resolution.json";
  m_force           = false;
}

void DataPackHandler::check(bool force) {

  m_force = force;
  // Clean the version file
  std::remove(m_version_file.c_str());

  // Run the process reading the versions
  auto get_version_process = new QProcess;
  connect(get_version_process, SIGNAL(finished(int, QProcess::ExitStatus)), this,
          SLOT(getDPVersionFinished(int, QProcess::ExitStatus)));
  connect(get_version_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this,
          SLOT(getDPVersionError(QProcess::ProcessError)));

  QStringList s3;
  s3
      /*dbg << QString::fromStdString("--repo-url") << QString::fromStdString("http://localhost:8001") */
      << QString::fromStdString("--output-version-match") << QString::fromStdString(m_version_file);

  const QString& command = QString("Phosphoros UDP ") + s3.join(" ");
  get_version_process->start(command);
}

void DataPackHandler::getDPVersionFinished(int, QProcess::ExitStatus) {
  std::ifstream f(m_version_file.c_str());
  if (f.good()) {
    boost::property_tree::ptree loadPtreeRoot;
    boost::property_tree::read_json(m_version_file, loadPtreeRoot);
    std::string remote = loadPtreeRoot.get_child("version_remote").get_value<std::string>();
    std::string local  = loadPtreeRoot.get_child("version_local").get_value<std::string>();

    if ((remote != local || m_force) &&
        QMessageBox::Yes ==
            QMessageBox::question(
                m_parent, "New Data Package Avalable ...",
                QString::fromStdString("The version " + remote +
                                       " of the data package is available. Do you want to download it ?"),
                QMessageBox::Yes | QMessageBox::Ignore)) {

      // Ensure the temp folder exists
      QFileInfo info(QString::fromStdString(m_temp_folder));
      if (!info.exists()) {
        QDir().mkpath(QString::fromStdString(m_temp_folder));
      }

      // Clean the conflict file
      std::remove(m_conflict_file.c_str());

      // Run the process for importing data and log conflicts
      auto get_conflict_process = new QProcess;
      connect(get_conflict_process, SIGNAL(finished(int, QProcess::ExitStatus)), this,
              SLOT(getConflictFinished(int, QProcess::ExitStatus)));

      QStringList s3;
      s3
          /*dbg   << QString::fromStdString("--repo-url") << QString::fromStdString("http://localhost:8001") */
          << QString::fromStdString("--force") << QString::fromStdString("True") << QString::fromStdString("--download")
          << QString::fromStdString("True") << QString::fromStdString("--temp-folder")
          << QString::fromStdString(m_temp_folder) << QString::fromStdString("--output-conflict")
          << QString::fromStdString(m_conflict_file);

      const QString& command = QString("Phosphoros UDP ") + s3.join(" ");
      get_conflict_process->start(command);
    } else {
      // User cancel the import
      completed();
    }
  } else {
    if (m_force) {
      QMessageBox::information(
          m_parent, "Data Package Management...",
          QString::fromStdString(
              "Unable to contact the Data Package Repository. Check your Internet connection and try again."),
          QMessageBox::Close);
    }
    // Unable to contact the backend
    completed();
  }
}

void DataPackHandler::getConflictFinished(int, QProcess::ExitStatus) {
  // Clean resolution file
  std::remove(m_resolution_file.c_str());

  // Check for conflict
  QFileInfo info(QString::fromStdString(m_conflict_file));
  if (info.exists()) {
    // Open Conflict resolution Dialog
    std::unique_ptr<DialogConflictingFilesHandling> popUp(new DialogConflictingFilesHandling(m_parent));
    popUp->setFilesPath(m_temp_folder, m_conflict_file, m_resolution_file);
    popUp->loadConflicts();
    if (popUp->exec() == QDialog::Accepted) {
      // Lanch the resolution process
      auto get_resolution_process = new QProcess;
      connect(get_resolution_process, SIGNAL(finished(int, QProcess::ExitStatus)), this,
              SLOT(getResolutionFinished(int, QProcess::ExitStatus)));

      QStringList s3;
      s3
          /*dbg  << QString::fromStdString("--repo-url") << QString::fromStdString("http://localhost:8001") */
          << QString::fromStdString("--force") << QString::fromStdString("True") << QString::fromStdString("--download")
          << QString::fromStdString("True") << QString::fromStdString("--temp-folder")
          << QString::fromStdString(m_temp_folder) << QString::fromStdString("--conflict-resolution")
          << QString::fromStdString(m_resolution_file);

      const QString& command = QString("Phosphoros UDP ") + s3.join(" ");

      logger.info() << "Running :" << command.toStdString();

      get_resolution_process->start(command);
    } else {
      // Popup closed in another way
      completed();
    }
  } else {
    // No conflict
    completed();
    QMessageBox::information(
        m_parent, "Data Package Imported ...",
        QString::fromStdString("The new version of the data package has been successfully imported."),
        QMessageBox::Close);
  }
}

void DataPackHandler::getResolutionFinished(int, QProcess::ExitStatus) {
  completed();
  QMessageBox::information(
      m_parent, "Data Package Imported ...",
      QString::fromStdString("The new version of the data package has been successfully imported."),
      QMessageBox::Close);
}

void DataPackHandler::getDPVersionError(QProcess::ProcessError error) {
  completed();
  QMessageBox::warning(
      m_parent, "Failed to update Data Package",
      QString::fromStdString(
          "Failed to check the remote version of the Data Package: Probably Phosphoros is not in the PATH"),
      QMessageBox::Close);
}

}  // namespace PhzQtUI
}  // namespace Euclid
