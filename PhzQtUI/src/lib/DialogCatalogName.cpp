
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
    ui(new Ui::DialogCatalogName)
{
    ui->setupUi(this);
}

DialogCatalogName::~DialogCatalogName()
{
}


void DialogCatalogName::on_btn_cancel_clicked()
{
  reject();
}

void DialogCatalogName::on_btn_create_clicked()
{
    if (ui->txt_name->text().length()==0){
       QMessageBox::warning( this, "Empty Name...","Please enter a name.", QMessageBox::Ok );
       return;
    }


    popupClosing(ui->txt_name->text().toStdString());
    accept();
}

}
}
