#include <QMessageBox>
#include <QFileDialog>

#include "ui_FormSurveyMapping.h"

#include "PhzUITools/CatalogColumnReader.h"

#include "PhzQtUI/FormSurveyMapping.h"
#include "PhzQtUI/SurveyModel.h"
#include "PhzQtUI/FilterModel.h"
#include "PhzQtUI/DialogFilterMapping.h"
#include "PhzQtUI/FileUtils.h"

namespace Euclid {
namespace PhzQtUI {

FormSurveyMapping::FormSurveyMapping(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSurveyMapping)
{
    ui->setupUi(this);
    m_filterInsert=false;
    m_mappingInsert=false;
}

FormSurveyMapping::~FormSurveyMapping()
{
}


void FormSurveyMapping::loadMappingPage(){
    SurveyModel* model = new SurveyModel();
    model->loadSurvey(FileUtils::getMappingRootPath(true));
    ui->table_Map->setModel(model);
    ui->table_Map->setColumnHidden(3, true);
    ui->table_Map->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_Map->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_Map->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->table_Map->update(QModelIndex());

    ui->txt_MapName->clear();
    ui->cb_SourceId->clear();
    ui->cb_SourceId->addItem("");

    connect(
      ui->table_Map->selectionModel(),
      SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      SLOT(filterMappingSelectionChanged(QModelIndex, QModelIndex))
     );

    ui->table_Map->clearSelection() ;
    FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
    ui->table_Filter->setModel(filter_model);

    setFilterMappingInView();
}

void FormSurveyMapping::setFilterMappingInEdition(){
    startEdition(0);
    ui->table_Map->setEnabled(false);
    ui->btn_MapNew->setEnabled(false);
    ui->btn_MapDuplicate->setEnabled(false);
    ui->btn_MapDelete->setEnabled(false);
    ui->btn_MapEdit->setEnabled(false);
    ui->btn_MapCancel->setEnabled(true);
    ui->btn_MapSave->setEnabled(true);
    ui->btn_ImportColumn->setEnabled(true);
    ui->txt_MapName->setEnabled(true);
    ui->cb_SourceId->setEnabled(true);
    ui->btn_AddFilter->setEnabled(true);

     ui->table_Filter->setEnabled(true);

    bool has_filter_selected = ui->table_Filter->selectionModel()->currentIndex().isValid();
    ui->btn_BtnEditFilter->setEnabled(has_filter_selected);
    ui->btn_DeleteFilter->setEnabled(has_filter_selected);

    m_filterInsert=false;
}

void FormSurveyMapping::setFilterMappingInView(){
    endEdition();
    ui->table_Map->setEnabled(true);
    bool has_mapping_selected = ui->table_Map->selectionModel()->currentIndex().isValid();

    ui->btn_MapNew->setEnabled(true);
    ui->btn_MapDuplicate->setEnabled(has_mapping_selected);
    ui->btn_MapDelete->setEnabled(has_mapping_selected);
    ui->btn_MapEdit->setEnabled(has_mapping_selected);
    ui->btn_MapCancel->setEnabled(false);
    ui->btn_MapSave->setEnabled(false);
    ui->btn_ImportColumn->setEnabled(false);
    ui->txt_MapName->setEnabled(false);
    ui->cb_SourceId->setEnabled(false);
    ui->btn_AddFilter->setEnabled(false);
    ui->btn_BtnEditFilter->setEnabled(false);
    ui->btn_DeleteFilter->setEnabled(false);


    ui->table_Filter->setEnabled(false);
    m_mappingInsert=false;
}

void FormSurveyMapping::loadColumnFromFile(std::string path){

  auto column_reader = PhzUITools::CatalogColumnReader(path);
  m_column_from_file=column_reader.getColumnNames();

  auto current_text = ui->cb_SourceId->currentText();
  ui->cb_SourceId->clear();
  ui->cb_SourceId->addItem("");
  bool found=false;
  int index=1;
  for(auto item : m_column_from_file){
    ui->cb_SourceId->addItem(QString::fromStdString(item));
    if (item.compare(current_text.toStdString())==0){
      found=true;
      ui->cb_SourceId->setCurrentIndex(index);
    }

     ++index;
  }

  if (!found){
    ui->cb_SourceId->setItemText(0,current_text);
  }
}


//  - Slots on this page

void FormSurveyMapping::on_btn_MapNew_clicked()
{
    int row_to_select =  static_cast<SurveyModel*>(ui->table_Map->model())->newSurvey(-1);
    ui->table_Map->selectRow(row_to_select);

    setFilterMappingInEdition();
    m_mappingInsert=true;
}

void FormSurveyMapping::on_btn_MapDuplicate_clicked()
{
    int row_to_select =  static_cast<SurveyModel*>(ui->table_Map->model())->newSurvey( ui->table_Map->selectionModel()->currentIndex().row());
    ui->table_Map->selectRow(row_to_select);
    setFilterMappingInEdition();
    m_mappingInsert=true;
}

void FormSurveyMapping::on_btn_MapDelete_clicked()
{
    if (QMessageBox::question( this, "Confirm deletion...",
                                  "Do you really want to delete this Survey Filter Mapping Set?",
                                  QMessageBox::Yes|QMessageBox::Cancel )==QMessageBox::Yes){
        static_cast<SurveyModel*>(ui->table_Map->model())->deleteSurvey(ui->table_Map->selectionModel()->currentIndex().row());
    }
}

void FormSurveyMapping::on_btn_MapEdit_clicked()
{
    setFilterMappingInEdition();
}

void FormSurveyMapping::on_btn_MapCancel_clicked()
{
    SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
    int row = ui->table_Map->selectionModel()->currentIndex().row();

     if (m_mappingInsert){
         model->deleteSurvey(row);
         m_mappingInsert=false;
     } else{
        ui->txt_MapName->setText(QString::fromStdString(model->getName(row)));


        int index = 0;
        std::string cb_text =model->getSourceIdColumn(row);
        for (int i=0; i<ui->cb_SourceId->count();++i){
            if (cb_text.compare(ui->cb_SourceId->itemText(i).toStdString())){
                index=i;
                break;
            }
        }
        ui->cb_SourceId->setCurrentIndex(index);
        if(index==0)
        ui->cb_SourceId->setItemText(0,QString::fromStdString(cb_text));

        FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));

        filter_model->setFilters(model->getFilters(row));
        ui->table_Filter->setModel(filter_model);
    }


    setFilterMappingInView();
}

void FormSurveyMapping::on_btn_MapSave_clicked()
{
    SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
     int row = ui->table_Map->selectionModel()->currentIndex().row();
     if (!model->checkUniqueName(ui->txt_MapName->text(),row)){

         QMessageBox::warning( this, "Duplicate name...",
                                           "The name you keyed in is already used. Please enter a new name.",
                                           QMessageBox::Ok );
         return;
     }

     std::string old_name=model->getName(row);
     model->setName(ui->txt_MapName->text().toStdString(),row);
     model->setSourceIdColumn(ui->cb_SourceId->currentText().toStdString(),row);

     FilterModel* filter_model=static_cast<FilterModel*>(ui->table_Filter->model());
     model->setFilters(std::move(filter_model->getFilters()),row);

     model->saveSurvey(row,old_name);

     m_filterInsert=false;

     setFilterMappingInView();
}

void FormSurveyMapping::filterMappingSelectionChanged(QModelIndex new_index, QModelIndex){

  FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
     if (new_index.isValid()){
         SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());

         ui->txt_MapName->setText(QString::fromStdString(model->getName(new_index.row())));


         int index = 0;
         std::string cb_text =model->getSourceIdColumn(new_index.row());
         for (int i=0; i<ui->cb_SourceId->count();++i){
             if (cb_text.compare(ui->cb_SourceId->itemText(i).toStdString())){
                 index=i;
                 break;
             }
         }
         ui->cb_SourceId->setCurrentIndex(index);
         if(index==0)
         ui->cb_SourceId->setItemText(0,QString::fromStdString(cb_text));


         filter_model->setFilters(model->getFilters(new_index.row()));
     }
     else{
         ui->txt_MapName->setText(QString::fromStdString(""));
         ui->cb_SourceId->setCurrentIndex(0);
         ui->cb_SourceId->clearEditText();
     }

     ui->table_Filter->setModel(filter_model);
     ui->table_Filter->setColumnHidden(4, true);
     ui->table_Filter->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->table_Filter->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->table_Filter->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
     ui->table_Filter->update(QModelIndex());

     connect(
       ui->table_Filter->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
       SLOT(filterSelectionChanged(QModelIndex, QModelIndex))
      );

     setFilterMappingInView();
 }

void FormSurveyMapping::on_btn_ImportColumn_clicked()
{
    QFileDialog dialog(this);
    dialog.selectFile(QString::fromStdString(FileUtils::getRootPath()));
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()){
        QStringList fileNames=dialog.selectedFiles();
        loadColumnFromFile(fileNames[0].toStdString());
    }
}

void FormSurveyMapping::filterSelectionChanged(QModelIndex, QModelIndex){
     bool has_filter_selected = ui->table_Filter->selectionModel()->currentIndex().isValid();
     ui->btn_BtnEditFilter->setEnabled(has_filter_selected);
     ui->btn_DeleteFilter->setEnabled(has_filter_selected);
}

void FormSurveyMapping::on_btn_AddFilter_clicked()
{
     m_filterInsert=true;
     std::unique_ptr<DialogFilterMapping> popUp(new DialogFilterMapping());
    popUp->setFilter(FilterMapping(),m_column_from_file);

    connect(
      popUp.get(),
      SIGNAL(popupClosing(FilterMapping)),
      SLOT(filterEditionPopupClosing(FilterMapping))
     );

    popUp->exec();
}

void FormSurveyMapping::on_btn_BtnEditFilter_clicked()
{
     m_filterInsert=false;
     std::unique_ptr<DialogFilterMapping> popUp( new DialogFilterMapping());

    popUp->setFilter(  static_cast<FilterModel*>(ui->table_Filter->model())->getFilter(ui->table_Filter->selectionModel()->currentIndex().row())
                       ,m_column_from_file);
    connect(
         popUp.get(),
         SIGNAL(popupClosing(FilterMapping)),
         SLOT(filterEditionPopupClosing(FilterMapping))
        );

    popUp->exec();
}

void FormSurveyMapping::filterEditionPopupClosing(FilterMapping filter){
  FilterModel* filter_model=static_cast<FilterModel*>(ui->table_Filter->model());

    if (m_filterInsert){
         filter_model->addFilter(filter);
         m_filterInsert=false;
    } else{
          int row = ui->table_Filter->selectionModel()->currentIndex().row();
          filter_model->setFilter(filter,row);
    }
}

void FormSurveyMapping::on_btn_DeleteFilter_clicked()
{
    if (QMessageBox::question( this, "Confirm deletion...",
                                  "Do you really want to delete this Filter?",
                                  QMessageBox::Yes|QMessageBox::Cancel )==QMessageBox::Yes){
      FilterModel* model=static_cast<FilterModel*>(ui->table_Filter->model());
        model->deleteFilter(ui->table_Filter->selectionModel()->currentIndex().row());
    }
}

}
}
