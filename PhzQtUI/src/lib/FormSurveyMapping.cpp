#include <QMessageBox>
#include <QFileDialog>

#include "ui_FormSurveyMapping.h"

#include "PhzUITools/CatalogColumnReader.h"

#include "PhzQtUI/FormSurveyMapping.h"
#include "PhzQtUI/SurveyModel.h"
#include "PhzQtUI/FilterModel.h"
#include "PhzQtUI/DialogFilterMapping.h"
#include "PhzQtUI/DialogCatalogName.h"
#include "FileUtils.h"



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


void FormSurveyMapping::loadMappingPage(std::string new_path){
     SurveyModel* model = new SurveyModel();
     model->loadSurvey();
     ui->table_Map->setModel(model);
     ui->table_Map->setColumnHidden(2, true);
     ui->table_Map->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->table_Map->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->table_Map->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
     ui->table_Map->update(QModelIndex());

     ui->cb_mapName->clear();
     for (auto cat :SurveyFilterMapping::getAvailableCatalogs()){
       ui->cb_mapName->addItem(QString::fromStdString(cat));
     }
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

     if (new_path.length()>0){
       on_btn_MapNew_clicked();
       loadColumnFromFile(new_path);
       m_default_survey=new_path;
       QFileInfo info(QString::fromStdString(new_path));
       ui->cb_mapName->addItem(info.baseName());
       ui->cb_mapName->setCurrentIndex(ui->cb_mapName->count()-1);
     }
}


void FormSurveyMapping::loadMappingPage(){
  loadMappingPage("");
}

void FormSurveyMapping::setFilterMappingInEdition(){
    startEdition(2);
    ui->table_Map->setEnabled(false);
    ui->btn_MapNew->setEnabled(false);
    ui->btn_MapDuplicate->setEnabled(false);
    ui->btn_MapDelete->setEnabled(false);
    ui->btn_MapEdit->setEnabled(false);
    ui->btn_MapCancel->setEnabled(true);
    ui->btn_MapSave->setEnabled(true);
    ui->btn_ImportColumn->setEnabled(true);
    ui->cb_mapName->setEnabled(true);
    ui->btn_newCat->setEnabled(true);
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
    ui->cb_mapName->setEnabled(false);
    ui->btn_newCat->setEnabled(false);
    ui->cb_SourceId->setEnabled(false);
    ui->btn_AddFilter->setEnabled(false);
    ui->btn_BtnEditFilter->setEnabled(false);
    ui->btn_DeleteFilter->setEnabled(false);


    ui->table_Filter->setEnabled(false);
    m_mappingInsert=false;
}


void FormSurveyMapping::fillCbColumns(std::string current_value){
   ui->cb_SourceId->clear();
   ui->cb_SourceId->addItem("");
   bool found=false;
   int index=1;
   for(auto item : m_column_from_file){
     ui->cb_SourceId->addItem(QString::fromStdString(item));
     if (item.compare(current_value)==0){
       found=true;
       ui->cb_SourceId->setCurrentIndex(index);
     }

      ++index;
   }

   if (!found){
     ui->cb_SourceId->setItemText(0,QString::fromStdString(current_value));
   }
}

void FormSurveyMapping::loadColumnFromFile(std::string path){

  auto column_reader = PhzUITools::CatalogColumnReader(path);
  m_column_from_file=column_reader.getColumnNames();

  auto current_text = ui->cb_SourceId->currentText();
  fillCbColumns(current_text.toStdString());
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
                                  "Do you really want to delete this Catalog Filter Mapping?",
                                  QMessageBox::No | QMessageBox::Yes )==QMessageBox::Yes){
        static_cast<SurveyModel*>(ui->table_Map->model())->deleteSurvey(ui->table_Map->selectionModel()->currentIndex().row());
    }
}

void FormSurveyMapping::on_btn_MapEdit_clicked()
{
    setFilterMappingInEdition();
}

void FormSurveyMapping::on_btn_newCat_clicked(){
  std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
  connect(
       popUp.get(),
       SIGNAL(popupClosing(std::string)),
       SLOT(catalogNamePopupClosing(std::string))
  );

  popUp->exec();
}

void FormSurveyMapping::catalogNamePopupClosing(std::string name){
  ui->cb_mapName->addItem(QString::fromStdString(name));
  ui->cb_mapName->setCurrentIndex(ui->cb_mapName->count()-1);
}

void FormSurveyMapping::on_btn_MapCancel_clicked()
{
    SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
    int row = ui->table_Map->selectionModel()->currentIndex().row();
    ui->cb_mapName->setCurrentIndex(-1);
    if (m_mappingInsert){
         model->deleteSurvey(row);
         m_mappingInsert=false;

     } else{

        auto current_name = model->getName(row);
        for (auto i=0;i< ui->cb_mapName->count();i++){

               if (ui->cb_mapName->itemText(i).toStdString()==current_name){
                   ui->cb_mapName->setCurrentIndex(i);
                   break;
               }
           }

        std::string cb_text =model->getSourceIdColumn(row);
        m_column_from_file=model->getColumnList(row);
        m_default_survey=model->getDefaultCatalog(row);
        fillCbColumns(cb_text);

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
     if (ui->cb_mapName->currentText().length()==0){
       QMessageBox::warning( this, "Empty name...",
                                                  "Please select a Catalog.",
                                                  QMessageBox::Ok );
       return;

     }
     if (!model->checkUniqueName(ui->cb_mapName->currentText(),row)){

         QMessageBox::warning( this, "Duplicate name...",
                                           "The catalog you selected is already mapped. Please select another one.",
                                           QMessageBox::Ok );
         return;
     }

     std::string old_name=model->getName(row);
     model->setName(ui->cb_mapName->currentText().toStdString(),row);
     model->setSourceIdColumn(ui->cb_SourceId->currentText().toStdString(),row);

     FilterModel* filter_model=static_cast<FilterModel*>(ui->table_Filter->model());
     model->setFilters(std::move(filter_model->getFilters()),row);

     model->setColumnList(m_column_from_file,row);
     model->setDefaultCatalog(m_default_survey,row);

     model->saveSurvey(row,old_name);

     m_filterInsert=false;

     setFilterMappingInView();
}

void FormSurveyMapping::filterMappingSelectionChanged(QModelIndex new_index, QModelIndex) {

  ui->cb_SourceId->clear();
  ui->cb_SourceId->addItem("");
  m_column_from_file.clear();
  std::string cb_text ="";
  FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
  if (new_index.isValid()) {
    SurveyModel* model = static_cast<SurveyModel*>(ui->table_Map->model());

    auto current_name = model->getName(new_index.row());
    for (auto i=0;i< ui->cb_mapName->count();i++){
        if (ui->cb_mapName->itemText(i).toStdString()==current_name){
            ui->cb_mapName->setCurrentIndex(i);
            break;
        }
    }

    m_default_survey=model->getDefaultCatalog(new_index.row());

    cb_text = model->getSourceIdColumn(new_index.row());

    m_column_from_file=model->getColumnList(new_index.row());

    filter_model->setFilters(model->getFilters(new_index.row()));
  } else {
    m_default_survey="";
    ui->cb_SourceId->setCurrentIndex(0);
    ui->cb_SourceId->clearEditText();
  }

  fillCbColumns(cb_text);

  ui->table_Filter->setModel(filter_model);
  ui->table_Filter->setColumnHidden(3, true);
  ui->table_Filter->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_Filter->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_Filter->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  ui->table_Filter->update(QModelIndex());

  connect(ui->table_Filter->selectionModel(),
      SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      SLOT(filterSelectionChanged(QModelIndex, QModelIndex)));

  setFilterMappingInView();
}

void FormSurveyMapping::on_btn_ImportColumn_clicked()
{
    QFileDialog dialog(this);
    dialog.selectFile(
        QString::fromStdString(FileUtils::getCatalogRootPath(true,
            ui->cb_mapName->currentText().toStdString()))+QDir::separator());

    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()){
        QStringList fileNames=dialog.selectedFiles();
        loadColumnFromFile(fileNames[0].toStdString());

        m_default_survey=fileNames[0].toStdString();

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
