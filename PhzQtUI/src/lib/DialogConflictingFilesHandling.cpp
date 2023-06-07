
#include "PhzQtUI/DialogConflictingFilesHandling.h"
#include "FileUtils.h"
#include "ui_DialogConflictingFilesHandling.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogConflictingFilesHandling::DialogConflictingFilesHandling(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogConflictingFilesHandling) {
  ui->setupUi(this);
}

DialogConflictingFilesHandling::~DialogConflictingFilesHandling() {}

void DialogConflictingFilesHandling::setFilesPath(std::string temp_folder, std::string conflicting_file,
                                                  std::string resolution_file) {
  m_conflicting_file = conflicting_file;
  m_resolution_file  = resolution_file;
  m_temp_folder      = temp_folder;
}

void DialogConflictingFilesHandling::loadConflicts() {
  std::ifstream      in(m_conflicting_file.c_str());
  std::ostringstream sstr;
  sstr << in.rdbuf();
  std::string conflict_content = sstr.str();
  conflict_content.erase(std::remove(conflict_content.begin(), conflict_content.end(), '{'), conflict_content.end());
  conflict_content.erase(std::remove(conflict_content.begin(), conflict_content.end(), '}'), conflict_content.end());
  conflict_content.erase(std::remove(conflict_content.begin(), conflict_content.end(), '"'), conflict_content.end());
  std::vector<std::string> conflict_elements;
  boost::algorithm::split(conflict_elements, conflict_content, boost::is_any_of(","));
  QStringList strList;
  for (size_t i = 0; i < conflict_elements.size(); ++i) {
    std::vector<std::string> conflict_names;
    boost::algorithm::split(conflict_names, conflict_elements[i], boost::is_any_of(":"));
    size_t index = 0;
    index        = conflict_names[0].find(m_temp_folder, index);
    conflict_names[0].replace(index, m_temp_folder.size(), "");
    auto name = conflict_names[0];
    boost::trim(name);
    strList << QString::fromStdString(name);
  }

  ui->lw->addItems(strList);

  QListWidgetItem* item = 0;
  for (int i = 0; i < ui->lw->count(); ++i) {
    item = ui->lw->item(i);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
  }
}

void DialogConflictingFilesHandling::saveResolution() {

  QStringList      items;
  QListWidgetItem* item = 0;
  for (int i = 0; i < ui->lw->count(); ++i) {
    item          = ui->lw->item(i);
    auto name     = item->text();
    auto fullname = "\"" + QString::fromStdString(m_temp_folder) + name + "\":\"";

    if (item->checkState() == Qt::Checked) {
      fullname = fullname + "r\"";
    } else {
      fullname = fullname + "k\"";
    }

    items << fullname;
  }

  QString json_content = "{" + items.join(',') + "}";

  std::ofstream fout(m_resolution_file);
  fout << json_content.toStdString();
}

void DialogConflictingFilesHandling::on_cb_all_clicked() {
  auto state = Qt::Unchecked;

  if (ui->cb_all->checkState() == Qt::Checked) {
    state = Qt::Checked;
  }

  QListWidgetItem* item = 0;
  for (int i = 0; i < ui->lw->count(); ++i) {
    item = ui->lw->item(i);
    item->setCheckState(state);
  }
}

void DialogConflictingFilesHandling::on_btn_apply_clicked() {
  saveResolution();
  accept();
}

}  // namespace PhzQtUI
}  // namespace Euclid
