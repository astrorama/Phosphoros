
#include <QMessageBox>


#include <QDir>
#include <QSettings>
#include "QtUI/DialogFilterMapping.h"
#include "ui_DialogFilterMapping.h"
#include "QtUI/DirectoryTreeModel.h"

DialogFilterMapping::DialogFilterMapping(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFilterMapping)
{
    ui->setupUi(this);
}

DialogFilterMapping::~DialogFilterMapping()
{
    delete ui;
}


void DialogFilterMapping::setFilter(const PhosphorosUiDm::FilterMapping& filter, const std::list<std::string>& columns){
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



    QSettings settings("SDC-CH", "PhosphorosUI");
    QString path_filter =settings.value(QString::fromStdString("Gerneral/root-path")).toString()+QDir::separator()+ "Filter";
    QFileInfo info_filter(path_filter);
    if (!info_filter.exists()){
        QDir().mkpath(path_filter);
    }

    DirectoryTreeModel* treeModel_filter = new DirectoryTreeModel();
    treeModel_filter->loadDirectory(path_filter.toStdString(),true,"Filters");
    treeModel_filter->setEnabled(true);
    ui->treeView_filter->setModel(treeModel_filter);
    ui->treeView_filter->expandAll();

    connect( treeModel_filter, SIGNAL(itemChanged(QStandardItem*)), treeModel_filter,
                 SLOT(onItemChangedSingleLeaf(QStandardItem*)));

    treeModel_filter->setState(filter.getFilterFile(),std::list<std::string>());
}


void DialogFilterMapping::on_btn_save_clicked()
{
    auto filter_res = static_cast<DirectoryTreeModel*>(ui->treeView_filter->model())->getRootSelection();

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

   PhosphorosUiDm::FilterMapping filter;
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
