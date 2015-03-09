
#include <QMessageBox>

#include "PhzQtUI/DialogFilterMapping.h"
#include "ui_DialogFilterMapping.h"
#include "PhzQtUI/XYDataSetTreeModel.h"
#include "PhzQtUI/FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogFilterMapping::DialogFilterMapping(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFilterMapping)
{
    ui->setupUi(this);
}

DialogFilterMapping::~DialogFilterMapping()
{
}


void DialogFilterMapping::setFilter(const FilterMapping& filter, const set<string>& columns){
    ui->cb_error->clear();
    ui->cb_error->addItem("");
    ui->cb_flux->clear();
    ui->cb_flux->addItem("");
    for(auto item : columns){
        ui->cb_error->addItem(QString::fromStdString(item));
        ui->cb_flux->addItem(QString::fromStdString(item));
    }

    ui->cb_error->setCurrentIndex(0);
    ui->cb_flux->setCurrentIndex(0);

    ui->txt_name->setText(QString::fromStdString(filter.getName()));
    ui->cb_error->setItemText(0,QString::fromStdString(filter.getErrorColumn()));
    ui->cb_flux->setItemText(0,QString::fromStdString(filter.getFluxColumn()));




    string path_filter = FileUtils::getFilterRootPath(true);

    XYDataSetTreeModel* treeModel_filter = new XYDataSetTreeModel();
    treeModel_filter->loadDirectory(path_filter,true,"Filters");
    treeModel_filter->setEnabled(true);
    ui->treeView_filter->setModel(treeModel_filter);
    ui->treeView_filter->expandAll();

    connect( treeModel_filter, SIGNAL(itemChanged(QStandardItem*)), treeModel_filter,
                 SLOT(onItemChangedSingleLeaf(QStandardItem*)));

    treeModel_filter->setState(filter.getFilterFile(),list<string>());
}


void DialogFilterMapping::on_btn_save_clicked()
{
    auto filter_res = static_cast<XYDataSetTreeModel*>(ui->treeView_filter->model())->getRootSelection();

    if (!filter_res.first
            || ui->txt_name->text().trimmed().length()==0
            || ui->cb_flux->currentText().trimmed().length()==0
            || ui->cb_error->currentText().trimmed().length()==0
            ){

        QMessageBox::warning( this, "Missing Data...",
                                          "Please provide a name, flux column, error column and a filter transmission.",
                                          QMessageBox::Ok );
        return;
    }

   FilterMapping filter;
   filter.setName(ui->txt_name->text().trimmed().toStdString());
   filter.setFluxColumn(ui->cb_flux->currentText().trimmed().toStdString());
   filter.setErrorColumn(ui->cb_error->currentText().trimmed().toStdString());
   filter.setFilterFile(filter_res.second);

   popupClosing(filter);
   accept();
}

void DialogFilterMapping::on_btn_cancel_clicked()
{
    reject();
}

}
}
