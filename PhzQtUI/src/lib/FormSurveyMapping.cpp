#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>

#include "ui_FormSurveyMapping.h"

#include "PhzUITools/CatalogColumnReader.h"

#include "PhzQtUI/FormSurveyMapping.h"
#include "PhzQtUI/SurveyModel.h"
#include "PhzQtUI/FilterModel.h"
#include "PhzQtUI/DialogFilterMapping.h"
#include "PhzQtUI/DialogCatalogName.h"
#include "FileUtils.h"

#include "PreferencesUtils.h"

#include "PhzQtUI/FilterMappingItemDelegate.h"


namespace Euclid {
namespace PhzQtUI {


FormSurveyMapping::FormSurveyMapping( QWidget *parent) :
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

void FormSurveyMapping::updateSelection(){
  auto saved_catalog = PreferencesUtils::getUserPreference("_global_selection_",
      "catalog");

  for (int i=0;i<ui->table_Map->model()->rowCount();++i){
    auto index = ui->table_Map->model()->index(i,0);
    if (ui->table_Map->model()->data(index).toString().toStdString() == saved_catalog){
      ui->table_Map->selectRow(i);
      break;
    }
  }
}



void FormSurveyMapping::loadMappingPage(DatasetRepo filter_repository, std::string new_path){

     ui->txt_nonDetection->setValidator(new QDoubleValidator());
     m_filter_repository = filter_repository;
     SurveyModel* model = new SurveyModel();
     model->loadSurvey();
     ui->table_Map->setModel(model);
     ui->table_Map->setColumnHidden(2, true);
     ui->table_Map->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->table_Map->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->table_Map->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
     ui->table_Map->update(QModelIndex());

     ui->cb_SourceId->clear();
     ui->cb_SourceId->addItem("");

     disconnect(ui->table_Map->selectionModel(),SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),0,0);
     connect(
       ui->table_Map->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
       SLOT(filterMappingSelectionChanged(QModelIndex, QModelIndex))
      );

     disconnect(ui->table_Map,SIGNAL(doubleClicked (QModelIndex)),0,0);
     connect(
            ui->table_Map,
            SIGNAL(doubleClicked (QModelIndex)),
            SLOT(mappingGridDoubleClicked(QModelIndex))
           );

     ui->table_Map->clearSelection() ;
     FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
     ui->table_Filter->setModel(filter_model);

     setFilterMappingInView();

     if (new_path.length()>0){
       on_btn_MapNew_clicked();
       loadColumnFromFile(new_path);
       m_default_survey=new_path;
       ui->tb_df->setText(QString::fromStdString(m_default_survey));
       QFileInfo info(QString::fromStdString(new_path));
     }

     updateSelection();


}


void FormSurveyMapping::setFilterMappingInEdition(){

    ui->frm_nav->setEnabled(false);
    ui->table_Map->setEnabled(false);
    ui->btn_MapNew->setEnabled(false);
    ui->btn_MapDuplicate->setEnabled(false);
    ui->btn_map_delete->setEnabled(false);
    ui->btn_MapEdit->setEnabled(false);
    ui->btn_MapCancel->setEnabled(true);
    ui->btn_MapSave->setEnabled(true);
    ui->btn_ImportColumn->setEnabled(true);
    ui->cb_SourceId->setEnabled(true);
    ui->btn_SelectFilters->setEnabled(true);


    ui->cb_missingPhot->setEnabled(true);
    ui->cb_upperLimit->setEnabled(true);
    ui->txt_nonDetection->setEnabled(ui->cb_missingPhot->checkState()== Qt::Checked);


    ui->table_Filter->setEnabled(true);



    m_filterInsert=false;
}

void FormSurveyMapping::setFilterMappingInView(){

    ui->table_Map->setEnabled(true);
    bool has_mapping_selected = ui->table_Map->selectionModel()->currentIndex().isValid();


    ui->frm_nav->setEnabled(true);

    ui->btn_MapNew->setEnabled(true);
    ui->btn_MapDuplicate->setEnabled(has_mapping_selected);
    ui->btn_map_delete->setEnabled(has_mapping_selected);
    ui->btn_MapEdit->setEnabled(has_mapping_selected);
    ui->btn_MapCancel->setEnabled(false);
    ui->btn_MapSave->setEnabled(false);
    ui->btn_ImportColumn->setEnabled(false);
    ui->cb_SourceId->setEnabled(false);
    ui->btn_SelectFilters->setEnabled(false);

    ui->cb_missingPhot->setEnabled(false);
     ui->cb_upperLimit->setEnabled(false);
     ui->txt_nonDetection->setEnabled(false);


   // ui->table_Filter->setEnabled(false);
    m_mappingInsert=false;
}


void FormSurveyMapping::on_gridEditionStart(){
  if (ui->frm_nav->isEnabled()){
    setFilterMappingInEdition();
  }
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

void FormSurveyMapping::on_btn_ToAnalysis_clicked(){
  navigateToComputeRedshift(false);
}
void FormSurveyMapping::on_btn_ToOption_clicked(){
  navigateToConfig();
}
void FormSurveyMapping::on_btn_ToModel_clicked(){
  navigateToParameter(false);
}
void FormSurveyMapping::on_btn_exit_clicked(){
  quit(true);
}

void FormSurveyMapping::on_btn_MapNew_clicked()
{
    int row_to_select =  static_cast<SurveyModel*>(ui->table_Map->model())->newSurvey(-1);
    ui->table_Map->selectRow(row_to_select);

    setFilterMappingInEdition();
    m_mappingInsert=true;

    SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
       std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
       popUp->setExistingNames(SurveyFilterMapping::getAvailableCatalogs());

       if (popUp->exec()== QDialog::Accepted){
         int row = ui->table_Map->selectionModel()->currentIndex().row();
         model->setName(popUp->getName(),row);
         ui->lbl_catalog_name->setText(QString::fromStdString(popUp->getName()));
       } else {
         on_btn_MapCancel_clicked();
       }

}

void FormSurveyMapping::on_btn_MapDuplicate_clicked()
{

    int row = ui->table_Map->selectionModel()->currentIndex().row();
    int row_to_select =  static_cast<SurveyModel*>(ui->table_Map->model())->newSurvey(row);
    ui->table_Map->selectRow(row_to_select);
    setFilterMappingInEdition();
    m_mappingInsert=true;

    SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
    std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
    popUp->setExistingNames(SurveyFilterMapping::getAvailableCatalogs());
    popUp->setDefaultName(model->getName(row_to_select));
    if (popUp->exec()== QDialog::Accepted){
      int row = ui->table_Map->selectionModel()->currentIndex().row();
      model->setName(popUp->getName(),row);
      ui->lbl_catalog_name->setText(QString::fromStdString(popUp->getName()));
    } else {
      on_btn_MapCancel_clicked();
    }
}


void FormSurveyMapping::on_btn_map_delete_clicked(){

  SurveyModel* model = static_cast<SurveyModel*>(ui->table_Map->model());

  int row = ui->table_Map->selectionModel()->currentIndex().row();
  std::string catalog_name = model->getName(row);

  std::string catalog_path = FileUtils::getCatalogRootPath(false, catalog_name);
  std::string intermediate_path = FileUtils::getIntermediaryProductRootPath(
      false, catalog_name);
  std::string result_path = FileUtils::getResultRootPath(false, catalog_name,
      "");

  if (QMessageBox::question(this, "Confirm deletion...",
      "Do you really want to delete the Catalog Type '"
          + QString::fromStdString(catalog_name) + "' ?\n \n"
              "!!! WARNING !!!\n"
              "This action will also DELETE :\n"
              "    - The Catalog folder  '"
          + QString::fromStdString(catalog_path)
          + "' and its content,\n"
              "    - All the related intermediate products (Model Grids,...) you may have computed and stored in '"
          + QString::fromStdString(intermediate_path) + "',\n"
              "\nHowever related results in the folder '"
          + QString::fromStdString(result_path) + "' will not be deleted.",
      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

    bool success=true;
    // The intermediate folder
    success &= FileUtils::removeDir(QString::fromStdString(intermediate_path));

    // The catalog folder
    success &= FileUtils::removeDir(QString::fromStdString(catalog_path));

    if (success){
      // The xml file
      model->deleteSurvey(row);
    }

  }
}


void FormSurveyMapping::on_btn_MapEdit_clicked()
{
    setFilterMappingInEdition();
}

void FormSurveyMapping::mappingGridDoubleClicked(QModelIndex)
{
   setFilterMappingInEdition();
}


void FormSurveyMapping::on_cb_missingPhot_stateChanged(int state){
  ui->txt_nonDetection->setEnabled(state==Qt::Checked);
}



void FormSurveyMapping::on_btn_MapCancel_clicked()
{
    SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
    int row = ui->table_Map->selectionModel()->currentIndex().row();
    if (m_mappingInsert){
         model->deleteSurvey(row);
         m_mappingInsert=false;

     } else{

        ui->cb_missingPhot->setCheckState( model->getHasMissingPhot(row)?Qt::Checked:Qt::Unchecked);
        ui->cb_upperLimit->setCheckState( model->getHasUpperLimit(row)?Qt::Checked:Qt::Unchecked);
        ui->txt_nonDetection->setText( QString::number(model->getNonDetection(row)));


        std::string cb_text =model->getSourceIdColumn(row);
        m_column_from_file=model->getColumnList(row);
        m_default_survey=model->getDefaultCatalog(row);
        fillCbColumns(cb_text);

        FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
        filter_model->setFilters(model->getFilters(row));
        ui->table_Filter->setModel(filter_model);
        ui->tb_df->setText(QString::fromStdString(m_default_survey));
    }

    setFilterMappingInView();
}


void FormSurveyMapping::on_btn_MapSave_clicked()
{

  if (ui->cb_SourceId->currentText().trimmed().length()==0){
            QMessageBox::warning( this,
                                       "Missing Data...",
                                       "Please provide the name of the Source ID Column.",
                                       QMessageBox::Ok );
                 return;
          }

  auto filters = getMappingFromGrid();
  for (auto&filter : filters){
        if (filter.getFluxColumn().length()==0 || filter.getErrorColumn().length()==0){
          QMessageBox::warning( this,
                                     "Missing Data...",
                                     "Please provide a name for all the Flux and Error Columns.",
                                     QMessageBox::Ok );
               return;
        }
      }

  SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
     int row = ui->table_Map->selectionModel()->currentIndex().row();

     std::string old_name=model->getName(row);
     model->setSourceIdColumn(ui->cb_SourceId->currentText().toStdString(),row);

     model->setNonDetection(ui->txt_nonDetection->text().toDouble(),row);
     model->setHasMissingPhot(ui->cb_missingPhot->checkState()==Qt::Checked,row);
     model->setHasUpperLimit(ui->cb_upperLimit->checkState()==Qt::Checked,row);





     model->setFilters(std::move(filters),row);

     model->setColumnList(m_column_from_file,row);
     model->setDefaultCatalog(m_default_survey,row);

     model->saveSurvey(row,old_name);


     PreferencesUtils::setUserPreference("_global_selection_",
                "catalog",model->getName(row));
     m_filterInsert=false;

     setFilterMappingInView();
     filterMappingSelectionChanged(ui->table_Map->selectionModel()->currentIndex(),ui->table_Map->selectionModel()->currentIndex());

}

void FormSurveyMapping::filterMappingSelectionChanged(QModelIndex new_index, QModelIndex) {

  ui->cb_SourceId->clear();
  ui->cb_SourceId->addItem("");
  m_column_from_file.clear();
  std::string cb_text ="";
  FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
  if (new_index.isValid()) {


    SurveyModel* model = static_cast<SurveyModel*>(ui->table_Map->model());


    PreferencesUtils::setUserPreference("_global_selection_",
               "catalog",model->getName(new_index.row()));

    m_default_survey=model->getDefaultCatalog(new_index.row());

    cb_text = model->getSourceIdColumn(new_index.row());

    m_column_from_file=model->getColumnList(new_index.row());

    filter_model->setFilters(model->getFilters(new_index.row()));

    ui->txt_nonDetection->setText(QString::number(model->getNonDetection(new_index.row())));
    ui->cb_missingPhot->setCheckState( model->getHasMissingPhot(new_index.row())?Qt::Checked:Qt::Unchecked);
    ui->cb_upperLimit->setCheckState( model->getHasUpperLimit(new_index.row())?Qt::Checked:Qt::Unchecked);

    ui->tb_df->setText(QString::fromStdString(m_default_survey));
    ui->lbl_catalog_name->setText(QString::fromStdString(model->getName(new_index.row())));

    auto delegate_item = new FilterMappingItemDelegate(m_column_from_file);
    ui->table_Filter->setItemDelegate(delegate_item);
    connect(
           delegate_item,
           SIGNAL(editionStarting()),
           SLOT(on_gridEditionStart())
          );

  } else {
    m_default_survey="";
    ui->cb_SourceId->setCurrentIndex(0);
    ui->cb_SourceId->clearEditText();
    ui->txt_nonDetection->setText(QString::number(-99.));
    ui->cb_missingPhot->setCheckState( Qt::Unchecked);
    ui->cb_upperLimit->setCheckState( Qt::Unchecked);

    ui->tb_df->setText("");
    ui->lbl_catalog_name->setText("Selected");
    ui->table_Filter->setItemDelegate(new FilterMappingItemDelegate(std::set<std::string>{}));


  }

  fillCbColumns(cb_text);

  ui->table_Filter->setModel(filter_model);
  ui->table_Filter->setColumnHidden(3, true);
  ui->table_Filter->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_Filter->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_Filter->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  ui->table_Filter->update(QModelIndex());


  setFilterMappingInView();

}



// todo reload the grid
void FormSurveyMapping::on_btn_ImportColumn_clicked()
{
  SurveyModel* model=static_cast<SurveyModel*>(ui->table_Map->model());
   int row = ui->table_Map->selectionModel()->currentIndex().row();

   std::string name=model->getName(row);

    QFileDialog dialog(this);
    dialog.selectFile(
        QString::fromStdString(FileUtils::getCatalogRootPath(true,name))+QDir::separator());

    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()){
        QStringList fileNames=dialog.selectedFiles();
        loadColumnFromFile(fileNames[0].toStdString());

        ui->table_Filter->setItemDelegate(new FilterMappingItemDelegate(m_column_from_file));

        m_default_survey=fileNames[0].toStdString();
        ui->tb_df->setText(QString::fromStdString(m_default_survey));
    }
}


std::vector<std::string> FormSurveyMapping::getGridFiltersNames() const{
  std::vector<std::string> filters{};
   for (int i=0; i<ui->table_Filter->model()->rowCount();++i){
     filters.push_back(ui->table_Filter->model()->data(ui->table_Filter->model()->index(i,3)).toString().toStdString());
   }
   return filters;
}


// to get from the controls
std::vector<FilterMapping> FormSurveyMapping::getMappingFromGrid() const{
  std::vector<FilterMapping> filters{};
     for (int i=0; i<ui->table_Filter->model()->rowCount();++i){
       auto name = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i,3)).toString().toStdString();
       auto flux = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i,1)).toString().toStdString();
       auto err = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i,2)).toString().toStdString();
       FilterMapping mapping{};
       mapping.setFilterFile(name);
       mapping.setFluxColumn(flux);
       mapping.setErrorColumn(err);
       filters.push_back(mapping);
     }
     return filters;
}

void FormSurveyMapping::on_btn_SelectFilters_clicked()
{
  auto filters = getGridFiltersNames();
  for (int i=0; i<ui->table_Filter->model()->rowCount();++i){
    filters.push_back(ui->table_Filter->model()->data(ui->table_Filter->model()->index(i,0)).toString().toStdString());
  }

  m_filterInsert=true;
  std::unique_ptr<DialogFilterMapping> popUp(new DialogFilterMapping(m_filter_repository));
  popUp->setFilters(filters);

  connect(popUp.get(), SIGNAL(popupClosing(std::vector<std::string>)),
      SLOT(filterEditionPopupClosing(std::vector<std::string>)));

  popUp->exec();
}


// todo update the filters lists and the grid
// tableView->setIndexWidget(tableView->model()->index(2, 1), new QPushButton);
void FormSurveyMapping::filterEditionPopupClosing(std::vector<std::string> filters){

   auto existing_filters = getMappingFromGrid();
   std::vector<FilterMapping> new_filters{};
   for (auto& filter : existing_filters){
     auto iter = std::find(filters.begin(), filters.end(), filter.getFilterFile());
     if (iter!=filters.end()){
       new_filters.push_back(filter);
       filters.erase(iter);
     }
   }

   for (auto& new_filter:filters){
     FilterMapping new_filter_mapping{};
     new_filter_mapping.setFilterFile(new_filter);
     new_filters.push_back(new_filter_mapping);
   }

   FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
   filter_model->setFilters(new_filters);

   ui->table_Filter->setModel(filter_model);
   ui->table_Filter->setColumnHidden(3, true);
   ui->table_Filter->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui->table_Filter->setSelectionMode(QAbstractItemView::SingleSelection);
   ui->table_Filter->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
   ui->table_Filter->update(QModelIndex());



}


}
}
