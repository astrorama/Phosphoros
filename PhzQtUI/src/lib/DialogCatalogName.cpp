
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "PhzQtUI/DialogCatalogName.h"
#include "ui_DialogCatalogName.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogCatalogName::DialogCatalogName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCatalogName){
  ui->setupUi(this);

  QRegExp rx("[\\w\\s]+");
  ui->txt_name->setValidator(new QRegExpValidator(rx));

}

DialogCatalogName::~DialogCatalogName(){}

void DialogCatalogName::setDefaultName(std::string default_name){
  ui->txt_name->setText(QString::fromStdString(default_name));
}

void DialogCatalogName::setExistingNames( std::list<std::string> existing_names){
  m_existing_names = std::move(existing_names);
}

void DialogCatalogName::on_btn_cancel_clicked(){
  reject();
}

void DialogCatalogName::on_btn_create_clicked(){
  m_name = ui->txt_name->text().trimmed().toStdString();

  if (m_name.length()==0){
    QMessageBox::warning( this, "Empty Name...","Please enter a name.", QMessageBox::Ok );
    return;
  }


  if (std::find(m_existing_names.begin(),m_existing_names.end(),m_name) != m_existing_names.end()){
  QMessageBox::warning( this, "Duplicate name...",
                                            "The catalog you selected is already mapped. Please select another one.",
                                            QMessageBox::Ok );
  return;
 }


  accept();
}


std::string DialogCatalogName::getName() const{
  return m_name;
}


}
}
