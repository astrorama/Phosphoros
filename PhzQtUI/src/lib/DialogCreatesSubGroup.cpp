#include "PhzQtUI/DialogCreatesSubGroup.h"
#include "ui_DialogCreatesSubGroup.h"
#include <QDir>
#include <QMessageBox>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogCreateSubGroup::DialogCreateSubGroup(QWidget* parent) : QDialog(parent), ui(new Ui::DialogCreateSubGroup) {
  ui->setupUi(this);
}

DialogCreateSubGroup::~DialogCreateSubGroup() {}

void DialogCreateSubGroup::setParentFolder(string folderFull, string folderDisplay) {
  m_parent_folder_full = folderFull;
  ui->txt_parent->setText(QString::fromStdString(folderDisplay));
}

void DialogCreateSubGroup::on_btn_cancel_clicked() {
  reject();
}

void DialogCreateSubGroup::on_pushButton_clicked() {
  QString folder_to_create = QString::fromStdString(m_parent_folder_full) + QDir::separator() + ui->txt_new->text();
  if (QDir(folder_to_create).exists() || !QDir().mkdir(folder_to_create)) {
    QMessageBox::warning(this, "Duplicate name...",
                         "It is not possible to create a Group with the provided name . Please enter a new name.",
                         QMessageBox::Ok);
  } else {
    accept();
  }
}

}  // namespace PhzQtUI
}  // namespace Euclid
