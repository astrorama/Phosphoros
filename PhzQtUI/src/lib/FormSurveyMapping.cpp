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
#include "PhzQtUI/SurveyFilterMapping.h"

#include "PreferencesUtils.h"

#include "PhzQtUI/FilterMappingItemDelegate.h"
#include "ElementsKernel/Logging.h"


namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormSurveyMapping");


FormSurveyMapping::FormSurveyMapping( QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSurveyMapping) {
    ui->setupUi(this);
    m_filterInsert = false;
    m_mappingInsert = false;
}

FormSurveyMapping::~FormSurveyMapping() {
}

void FormSurveyMapping::updateSelection() {
  int selected_row = m_survey_model_ptr->getSelectedRow();
  if (selected_row >= 0) {
    ui->table_Map->selectRow(selected_row);
  }
  fillControlsWithSelected();
}



void FormSurveyMapping::loadMappingPage(
    std::shared_ptr<SurveyModel> survey_model_ptr,
    DatasetRepo filter_repository,
    std::string new_path) {

    logger.info()<< "Load the Catalog Page";
     m_survey_model_ptr = survey_model_ptr;
     ui->txt_nonDetection->setValidator(new QDoubleValidator());
     m_filter_repository = filter_repository;

     ui->table_Map->setModel(m_survey_model_ptr.get());
     ui->table_Map->setColumnHidden(2, true);
     ui->table_Map->setSelectionBehavior(QAbstractItemView::SelectRows);
     ui->table_Map->setSelectionMode(QAbstractItemView::SingleSelection);
     ui->table_Map->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
     ui->table_Map->update(QModelIndex());


     disconnect(ui->table_Map->selectionModel(),
                SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0, 0);
     connect(
       ui->table_Map->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
       SLOT(filterMappingSelectionChanged(QModelIndex, QModelIndex)));

     disconnect(ui->table_Map, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
     connect(
            ui->table_Map,
            SIGNAL(doubleClicked(QModelIndex)),
            SLOT(mappingGridDoubleClicked(QModelIndex)));

     setFilterMappingInView();

     if (new_path.length() > 0) {
       on_btn_MapNew_clicked();
       loadColumnFromFile(new_path);
       m_default_survey = new_path;
       ui->tb_df->setText(QString::fromStdString(m_default_survey));
       m_survey_model_ptr->setDefaultCatalogToSelected(QString::fromStdString(m_default_survey));
       m_survey_model_ptr->setColumnListToSelected(m_column_from_file);
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
    ui->cb_Ra->setEnabled(true);
    ui->cb_Dec->setEnabled(true);
    ui->cb_GalEbv->setEnabled(true);
    ui->btn_SelectFilters->setEnabled(true);

    ui->cb_missingPhot->setEnabled(true);
    ui->cb_upperLimit->setEnabled(true);
    ui->txt_nonDetection->setEnabled(ui->cb_missingPhot->checkState()== Qt::Checked);

    ui->table_Filter->setEnabled(true);

    m_filterInsert = false;
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
    ui->cb_Ra->setEnabled(false);
    ui->cb_Dec->setEnabled(false);
    ui->cb_GalEbv->setEnabled(false);
    ui->btn_SelectFilters->setEnabled(false);

    ui->cb_missingPhot->setEnabled(false);
    ui->cb_upperLimit->setEnabled(false);
    ui->txt_nonDetection->setEnabled(false);

    ui->table_Filter->setEnabled(false);

    m_mappingInsert = false;
}





void FormSurveyMapping::fillCbColumns(std::string current_id_value,
                                      std::string current_ra_value,
                                      std::string current_dec_value,
                                      std::string current_gebv_value) {
   ui->cb_SourceId->clear();
   ui->cb_SourceId->addItem("");
   ui->cb_Ra->clear();
   ui->cb_Ra->addItem("");
   ui->cb_Dec->clear();
   ui->cb_Dec->addItem("");
   ui->cb_GalEbv->clear();
   ui->cb_GalEbv->addItem("");

   bool found_id = false;
   bool found_ra = false;
   bool found_dec = false;
   bool found_ebv = false;
   int index = 1;

   for (auto item : m_column_from_file) {
     ui->cb_SourceId->addItem(QString::fromStdString(item));
     ui->cb_Ra->addItem(QString::fromStdString(item));
     ui->cb_Dec->addItem(QString::fromStdString(item));
     ui->cb_GalEbv->addItem(QString::fromStdString(item));
     if (item.compare(current_id_value) == 0) {
       found_id = true;
       ui->cb_SourceId->setCurrentIndex(index);
     }
     if (item.compare(current_ra_value) == 0) {
       found_ra = true;
       ui->cb_Ra->setCurrentIndex(index);
     }
     if (item.compare(current_dec_value) == 0) {
       found_dec = true;
       ui->cb_Dec->setCurrentIndex(index);
     }
     if (item.compare(current_gebv_value) == 0) {
       found_ebv = true;
       ui->cb_GalEbv->setCurrentIndex(index);
     }

      ++index;
   }

   if (!found_id) {
     ui->cb_SourceId->setItemText(0, QString::fromStdString(current_id_value));
   }
   if (!found_ra) {
     ui->cb_Ra->setItemText(0, QString::fromStdString(current_ra_value));
   }
   if (!found_dec) {
     ui->cb_Dec->setItemText(0, QString::fromStdString(current_dec_value));
   }
   if (!found_ebv) {
     ui->cb_GalEbv->setItemText(0, QString::fromStdString(current_gebv_value));
   }
}

void FormSurveyMapping::loadColumnFromFile(std::string path) {
  auto column_reader = PhzUITools::CatalogColumnReader(path);
  m_column_from_file = column_reader.getColumnNames();
  m_survey_model_ptr->setColumnListToSelected(m_column_from_file);
  m_survey_model_ptr->setDefaultCatalogToSelected(QString::fromStdString(path));

  auto current_id_text = ui->cb_SourceId->currentText();
  auto current_ra_text = ui->cb_Ra->currentText();
  auto current_dec_text = ui->cb_Dec->currentText();
  auto current_gebv_text = ui->cb_GalEbv->currentText();
  fillCbColumns(current_id_text.toStdString(),
      current_ra_text.toStdString(),
      current_dec_text.toStdString(),
      current_gebv_text.toStdString());
}

void FormSurveyMapping::on_cb_SourceId_currentIndexChanged(int) {
  m_survey_model_ptr->setIdColumnToSelected(ui->cb_SourceId->currentText());
}

void FormSurveyMapping::on_cb_Dec_currentIndexChanged(int) {
  m_survey_model_ptr->setDecColumnToSelected(ui->cb_Dec->currentText());
}

void FormSurveyMapping::on_cb_Ra_currentIndexChanged(int) {
  m_survey_model_ptr->setRaColumnToSelected(ui->cb_Ra->currentText());
}

void FormSurveyMapping::on_cb_GalEbv_currentIndexChanged(int) {
  m_survey_model_ptr->setGalEbvColumnToSelected(ui->cb_GalEbv->currentText());
}

void FormSurveyMapping::on_cb_missingPhot_stateChanged(int state) {
  ui->txt_nonDetection->setEnabled(state == Qt::Checked);
  m_survey_model_ptr->setHasMissingPhotToSelected(state == Qt::Checked);
}

void FormSurveyMapping::on_txt_nonDetection_textEdited(const QString& text) {
  m_survey_model_ptr->setNonDetectionToSelected(text);
}

void FormSurveyMapping::on_cb_upperLimit_stateChanged(int state) {
  m_survey_model_ptr->setHasUpperLimitToSelected(state == Qt::Checked);
}



//  - Slots on this page

void FormSurveyMapping::on_btn_ToAnalysis_clicked() {
  navigateToComputeRedshift(false);
}
void FormSurveyMapping::on_btn_ToOption_clicked() {
  navigateToConfig();
}
void FormSurveyMapping::on_btn_ToModel_clicked() {
  navigateToParameter(false);
}

void FormSurveyMapping::on_btn_ToPP_clicked(){
  navigateToPostProcessing(false);
}

void FormSurveyMapping::on_btn_exit_clicked() {
  quit(true);
}

void FormSurveyMapping::on_btn_MapNew_clicked() {
   m_survey_model_ptr->newSurvey(false);
   updateSelection();
   setFilterMappingInEdition();
   m_mappingInsert = true;
   std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
   popUp->setExistingNames(SurveyFilterMapping::getAvailableCatalogs());
   if (popUp->exec() == QDialog::Accepted) {
     m_survey_model_ptr->setNameToSelected(QString::fromStdString(popUp->getName()));
     ui->lbl_catalog_name->setText(QString::fromStdString(popUp->getName()));
   } else {
     on_btn_MapCancel_clicked();
   }
}

void FormSurveyMapping::on_btn_MapDuplicate_clicked() {
    m_survey_model_ptr->newSurvey(true);
    updateSelection();
    setFilterMappingInEdition();
    m_mappingInsert = true;
    std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
    popUp->setExistingNames(SurveyFilterMapping::getAvailableCatalogs());
    popUp->setDefaultName(m_survey_model_ptr->getSelectedSurvey().getName());
    if (popUp->exec() == QDialog::Accepted) {
      m_survey_model_ptr->setNameToSelected(QString::fromStdString(popUp->getName()));
      ui->lbl_catalog_name->setText(QString::fromStdString(popUp->getName()));
    } else {
      on_btn_MapCancel_clicked();
    }
}


void FormSurveyMapping::on_btn_map_delete_clicked() {
  const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();
  int row = m_survey_model_ptr->getSelectedRow();
  if (row >= 0) {
     std::string catalog_name = selected_survey.getName();
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

      bool success = true;
      // The intermediate folder
      success &= FileUtils::removeDir(QString::fromStdString(intermediate_path));

      // The catalog folder
      success &= FileUtils::removeDir(QString::fromStdString(catalog_path));

      if (success) {
        // The xml file
        m_survey_model_ptr->deletSelected();
        selectFromGrid();
      }
    }
  }
}

void FormSurveyMapping::on_btn_MapEdit_clicked() {
    setFilterMappingInEdition();
}

void FormSurveyMapping::mappingGridDoubleClicked(QModelIndex) {
   setFilterMappingInEdition();
}

void FormSurveyMapping::fillControlsWithSelected() {
      const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();

      ui->lbl_catalog_name->setText(QString::fromStdString(selected_survey.getName()));

      m_default_survey = selected_survey.getDefaultCatalogFile();
      ui->tb_df->setText(QString::fromStdString(m_default_survey));

      std::string cb_id_text = selected_survey.getSourceIdColumn();
      std::string cb_ra_text = selected_survey.getRaColumn();
      std::string cb_dec_text = selected_survey.getDecColumn();
      std::string cb_gebv_text = selected_survey.getGalEbvColumn();
      m_column_from_file = selected_survey.getColumnList();
      fillCbColumns(cb_id_text, cb_ra_text, cb_dec_text, cb_gebv_text);

      ui->cb_missingPhot->setCheckState(selected_survey.getHasMissingPhotometry() ? Qt::Checked : Qt::Unchecked);
      ui->txt_nonDetection->setText(QString::number(selected_survey.getNonDetection()));
      ui->cb_upperLimit->setCheckState(selected_survey.getHasUpperLimit() ? Qt::Checked : Qt::Unchecked);

      FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
      filter_model->setFilters(selected_survey.getFilters());
      ui->table_Filter->setItemDelegate(new FilterMappingItemDelegate(m_survey_model_ptr->getSelectedSurvey().getColumnList()));
      ui->table_Filter->setModel(filter_model);
      ui->table_Filter->setColumnHidden(3, true);
      ui->table_Filter->setSelectionBehavior(QAbstractItemView::SelectRows);
      ui->table_Filter->setSelectionMode(QAbstractItemView::SingleSelection);
      ui->table_Filter->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
      ui->table_Filter->update(QModelIndex());

}

void FormSurveyMapping::selectFromGrid() {
  QItemSelectionModel *current_selection = ui->table_Map->selectionModel();
  if (current_selection->hasSelection()) {
    int row = current_selection->selectedRows().at(0).row();
    m_survey_model_ptr->selectSurvey(row);
  } else {
    m_survey_model_ptr->selectSurvey(-1);
  }
}


void FormSurveyMapping::on_btn_MapCancel_clicked() {
    if (m_mappingInsert) {
      m_survey_model_ptr->deletSelected();
      selectFromGrid();

    } else {
      m_survey_model_ptr->cancelSelected();
    }

    fillControlsWithSelected();
    setFilterMappingInView();
}


void FormSurveyMapping::on_btn_MapSave_clicked() {
  if (ui->cb_SourceId->currentText().trimmed().length() == 0) {
            QMessageBox::warning(this,
                                       "Missing Data...",
                                       "Please provide the name of the Source ID Column.",
                                       QMessageBox::Ok);
                 return;
          }

  auto filters = getMappingFromGrid();
  for (auto& filter : filters) {
        if (filter.getFluxColumn().length() == 0 || filter.getErrorColumn().length() == 0) {
          QMessageBox::warning(this,
                                     "Missing Data...",
                                     "Please provide a name for all the Flux and Error Columns.",
                                     QMessageBox::Ok);
               return;
        }
      }
  m_survey_model_ptr->setFiltersToSelected(filters);
  if (m_survey_model_ptr->saveSelected()) {
    m_filterInsert = false;
    setFilterMappingInView();
    updateSelection();

  } else {
    // Never here because the duplicate name is tested in the popup for name selection
  }

}

void FormSurveyMapping::filterMappingSelectionChanged(QModelIndex new_index, QModelIndex) {
  if (new_index.isValid()) {
    m_survey_model_ptr->selectSurvey(new_index.row());
  } else {
    m_survey_model_ptr->selectSurvey(-1);
  }

  fillControlsWithSelected();
  setFilterMappingInView();
}

void FormSurveyMapping::on_btn_ImportColumn_clicked() {
  const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();
  std::string name = selected_survey.getName();
  QFileDialog dialog(this);
  dialog.selectFile(
        QString::fromStdString(FileUtils::getCatalogRootPath(true, name)) + QDir::separator());

  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        loadColumnFromFile(fileNames[0].toStdString());
        ui->table_Filter->setItemDelegate(new FilterMappingItemDelegate(m_column_from_file));
        m_default_survey = fileNames[0].toStdString();
        ui->tb_df->setText(QString::fromStdString(m_default_survey));
        m_survey_model_ptr->setDefaultCatalogToSelected(QString::fromStdString(m_default_survey));
        m_survey_model_ptr->setColumnListToSelected(m_column_from_file);
    }
}


std::vector<std::string> FormSurveyMapping::getGridFiltersNames() const {
  std::vector<std::string> filters{};
   for (int i=0; i < ui->table_Filter->model()->rowCount(); ++i) {
     filters.push_back(ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 3)).toString().toStdString());
   }
   return filters;
}


// to get from the controls
std::vector<FilterMapping> FormSurveyMapping::getMappingFromGrid() const {
  std::vector<FilterMapping> filters{};
     for (int i = 0; i < ui->table_Filter->model()->rowCount(); ++i) {
       auto name = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 3)).toString().toStdString();
       auto flux = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 1)).toString().toStdString();
       auto err = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 2)).toString().toStdString();
       FilterMapping mapping{};
       mapping.setFilterFile(name);
       mapping.setFluxColumn(flux);
       mapping.setErrorColumn(err);
       filters.push_back(mapping);
     }
     return filters;
}

void FormSurveyMapping::on_btn_SelectFilters_clicked() {
  auto filters = getGridFiltersNames();
  for (int i = 0; i < ui->table_Filter->model()->rowCount(); ++i) {
    filters.push_back(ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 0)).toString().toStdString());
  }

  m_filterInsert = true;
  std::unique_ptr<DialogFilterMapping> popUp(new DialogFilterMapping(m_filter_repository));
  popUp->setFilters(filters);

  connect(popUp.get(), SIGNAL(popupClosing(std::vector<std::string>)),
      SLOT(filterEditionPopupClosing(std::vector<std::string>)));

  popUp->exec();
}


// tableView->setIndexWidget(tableView->model()->index(2, 1), new QPushButton);
void FormSurveyMapping::filterEditionPopupClosing(std::vector<std::string> filters) {

   auto existing_filters = getMappingFromGrid();
   std::vector<FilterMapping> new_filters {};
   for (auto& filter : existing_filters) {
     auto iter = std::find(filters.begin(), filters.end(), filter.getFilterFile());
     if (iter != filters.end()) {
       new_filters.push_back(filter);
       filters.erase(iter);
     }
   }

   for (auto& new_filter : filters) {
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

   m_survey_model_ptr->setFiltersToSelected(new_filters);
}


}
}
