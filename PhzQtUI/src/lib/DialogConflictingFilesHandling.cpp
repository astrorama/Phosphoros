
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegExpValidator>
#include "PhzQtUI/DialogConflictingFilesHandling.h"
#include "ui_DialogConflictingFilesHandling.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogConflictingFilesHandling::DialogConflictingFilesHandling(QWidget *parent) :
    QDialog(parent) ,
    ui(new Ui::DialogConflictingFilesHandling) {
    ui->setupUi(this);
}

DialogConflictingFilesHandling::~DialogConflictingFilesHandling() {}

void DialogConflictingFilesHandling::setFilesPath(std::string conflicting_file, std::string resolution_file) {
  m_conflicting_file = conflicting_file;
  m_resolution_file = resolution_file;
}

void DialogConflictingFilesHandling::loadConflicts() {


}

void DialogConflictingFilesHandling::saveResolution() {


}

void DialogConflictingFilesHandling::on_btn_apply_clicked() {


}

void DialogConflictingFilesHandling::on_cd_all_clicked() {
  accept();

}


}
}
