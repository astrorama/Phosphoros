
#include "PhzQtUI/DialogCatalogName.h"
#include "FileUtils.h"
#include "ui_DialogCatalogName.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegExpValidator>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogCatalogName::DialogCatalogName(QWidget* parent) : QDialog(parent), ui(new Ui::DialogCatalogName) {
  ui->setupUi(this);

  QRegExp rx("[\\w\\s]+");
  ui->txt_name->setValidator(new QRegExpValidator(rx));
}

DialogCatalogName::~DialogCatalogName() {}

void DialogCatalogName::setDefaultName(std::string default_name) {
  ui->txt_name->setText(QString::fromStdString(default_name));
}

void DialogCatalogName::setExistingNames(std::vector<std::string> existing_names) {
  m_existing_names = std::move(existing_names);
}

void DialogCatalogName::setDefaultCopiedFile(std::string default_file) {
  m_file = default_file;
  ui->txt_file_name->setText(QString::fromStdString(m_file));
}

std::string DialogCatalogName::getFilePath() const {
  return m_file;
}

void DialogCatalogName::on_btn_empty_clicked() {
  m_file = "";
  ui->txt_file_name->setText(QString::fromStdString(m_file));
}

void DialogCatalogName::on_btn_import_clicked() {
  QFileDialog dialog(this);
  dialog.selectFile(QString::fromStdString(FileUtils::getCatalogRootPath(true, "")));

  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    QStringList fileNames = dialog.selectedFiles();
    m_file                = fileNames[0].toStdString();
    ui->txt_file_name->setText(QString::fromStdString(m_file));
  }
}

void DialogCatalogName::on_btn_cancel_clicked() {
  reject();
}

void DialogCatalogName::on_btn_create_clicked() {
  m_name = ui->txt_name->text().trimmed().toStdString();

  if (m_name.length() == 0) {
    QMessageBox::warning(this, "Empty Name...", "Please enter a name.", QMessageBox::Ok);
    return;
  }

  if (std::find(m_existing_names.begin(), m_existing_names.end(), m_name) != m_existing_names.end()) {
    QMessageBox::warning(this, "Duplicate name...",
                         "The catalog you selected is already mapped. Please select another one.", QMessageBox::Ok);
    return;
  }

  accept();
}

std::string DialogCatalogName::getName() const {
  return m_name;
}

}  // namespace PhzQtUI
}  // namespace Euclid
