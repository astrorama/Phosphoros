
#include "PhzQtUI/DialogZeroPointName.h"
#include "FileUtils.h"
#include "ui_DialogZeroPointName.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpressionValidator>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogZeroPointName::DialogZeroPointName(QWidget* parent) : QDialog(parent), ui(new Ui::DialogZeroPointName) {
  ui->setupUi(this);

  QRegularExpression rx("[\\w\\s]+");
  ui->txt_name->setValidator(new QRegularExpressionValidator(rx));
}

DialogZeroPointName::~DialogZeroPointName() {}

void DialogZeroPointName::setFolder(std::string base_folder) {
  m_folder = base_folder;
}

void DialogZeroPointName::on_btn_cancel_clicked() {
  reject();
}

void DialogZeroPointName::on_btn_create_clicked() {
  m_name = ui->txt_name->text().trimmed().toStdString();

  if (m_name.length() == 0) {
    QMessageBox::warning(this, "Empty Name...", "Please enter a name.", QMessageBox::Ok);
    return;
  }

  std::string ending = ".txt";
  if (ending.size() < m_name.size() || !std::equal(ending.rbegin(), ending.rend(), m_name.rbegin())) {
    m_name = m_name + ending;
    ui->txt_name->setText(QString::fromStdString(m_name));
  }

  string path = m_folder + "/" + m_name;

  QFileInfo check_file(QString::fromStdString(path));

  if (check_file.exists() && check_file.isFile()) {
    QMessageBox::warning(this, "Duplicate name...",
                         "The catalog you selected is already mapped. Please select another one.", QMessageBox::Ok);
    return;
  }

  accept();
}

std::string DialogZeroPointName::getName() const {
  return m_name;
}

}  // namespace PhzQtUI
}  // namespace Euclid
