#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "ui_FormSurveyMapping.h"
#include <QtCore/qdebug.h>

#include "PhzUITools/CatalogColumnReader.h"

#include "FileUtils.h"
#include "PhzQtUI/DialogCatalogName.h"
#include "PhzQtUI/DialogFilterMapping.h"
#include "PhzQtUI/FilterModel.h"
#include "PhzQtUI/FormSurveyMapping.h"
#include "PhzQtUI/SurveyFilterMapping.h"
#include "PhzQtUI/SurveyModel.h"

#include "PreferencesUtils.h"

#include "ElementsKernel/Logging.h"
#include "PhzQtUI/BoolItemDelegate.h"
#include "PhzQtUI/FilterMappingItemDelegate.h"
#include "PhzQtUI/NumberItemDelegate.h"
#include "PhzQtUI/filecopyer.h"

#include <QtCore/qthread.h>
#include <boost/filesystem.hpp>

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormSurveyMapping");

FormSurveyMapping::FormSurveyMapping(QWidget* parent) : QWidget(parent), ui(new Ui::FormSurveyMapping) {
  ui->setupUi(this);
  m_filterInsert  = false;
  m_mappingInsert = false;
}

FormSurveyMapping::~FormSurveyMapping() {}

void FormSurveyMapping::on_cb_catalog_type_currentIndexChanged(int) {
  if (!m_diconnect_cb) {
    logger.info() << "Selected Catalog changed to:" << ui->cb_catalog_type->currentText().toStdString();
    m_survey_model_ptr->selectSurvey(ui->cb_catalog_type->currentText());
    updateSelection();
  }
}

void FormSurveyMapping::loadCatalogCB(std::string selected) {
  m_diconnect_cb = true;
  ui->cb_catalog_type->clear();
  int index = 0;
  for (auto& catalog_name : m_survey_model_ptr->getSurveyList()) {
    ui->cb_catalog_type->addItem(catalog_name);
    if (selected == catalog_name.toStdString()) {
      ui->cb_catalog_type->setCurrentIndex(index);
    }
    ++index;
  }
  m_diconnect_cb = false;
}

void FormSurveyMapping::updateSelection(bool force_reload_cb) {
  if (force_reload_cb ||
      (m_survey_model_ptr->getSelectedRow() >= 0 &&
       ui->cb_catalog_type->currentText().toStdString() != m_survey_model_ptr->getSelectedSurvey().getName())) {
    loadCatalogCB(m_survey_model_ptr->getSelectedSurvey().getName());
  }
  fillControlsWithSelected();
  setFilterMappingInView();
}

std::set<std::string> FormSurveyMapping::getFilteredColumns() {
  const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();
  auto                       full_list       = std::set<std::string>{selected_survey.getColumnList()};
  full_list.erase(selected_survey.getSourceIdColumn());
  full_list.erase(selected_survey.getRaColumn());
  full_list.erase(selected_survey.getDecColumn());
  full_list.erase(selected_survey.getGalEbvColumn());
  full_list.erase(selected_survey.getRefZColumn());

  return full_list;
}

void FormSurveyMapping::fillControlsWithSelected() {
  const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();

  m_default_survey = selected_survey.getDefaultCatalogFile();
  ui->tb_df->setText(QString::fromStdString(m_default_survey));

  std::string cb_id_text   = selected_survey.getSourceIdColumn();
  std::string cb_ra_text   = selected_survey.getRaColumn();
  std::string cb_dec_text  = selected_survey.getDecColumn();
  std::string cb_gebv_text = selected_survey.getGalEbvColumn();
  std::string cb_refz_text = selected_survey.getRefZColumn();
  m_column_from_file       = selected_survey.getColumnList();
  fillCbColumns(cb_id_text, cb_ra_text, cb_dec_text, cb_gebv_text, cb_refz_text);

  ui->txt_nonDetection->setText(QString::number(selected_survey.getNonDetection()));
  ui->txt_UpperLimit->setText(QString::number(selected_survey.getUpperLimit()));
  ui->ckb_error_prop->setCheckState(selected_survey.getDoRecomputeError() ? Qt::Checked : Qt::Unchecked);
  ui->ckb_filterShift->setCheckState(selected_survey.getDefineFilterShift() ? Qt::Checked : Qt::Unchecked);

  if (ui->table_Filter->model() != NULL) {
    disconnect(ui->table_Filter->model(), SIGNAL(itemChanged(QStandardItem*)), 0, 0);
  }

  if (ui->table_Filter->selectionModel() != NULL) {
    disconnect(ui->table_Filter->selectionModel(),
               SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 0, 0);
  }

  FilterModel* filter_model = new FilterModel(FileUtils::getFilterRootPath(false));
  filter_model->setFilters(selected_survey.getFilters());
  ui->table_Filter->setItemDelegateForColumn(1, new FilterMappingItemDelegate(getFilteredColumns()));
  ui->table_Filter->setItemDelegateForColumn(2, new FilterMappingItemDelegate(getFilteredColumns()));

  ui->table_Filter->setItemDelegateForColumn(4, new NumberItemDelegate());
  ui->table_Filter->setItemDelegateForColumn(5, new NumberItemDelegate());
  ui->table_Filter->setItemDelegateForColumn(6, new NumberItemDelegate());
  ui->table_Filter->setItemDelegateForColumn(7, new NumberItemDelegate());
  ui->table_Filter->setItemDelegateForColumn(8, new BoolItemDelegate());
  ui->table_Filter->setItemDelegateForColumn(9, new FilterMappingItemDelegate(getFilteredColumns(), "NONE"));

  ui->table_Filter->setModel(filter_model);
  ui->table_Filter->setColumnHidden(3, true);
  ui->table_Filter->setColumnHidden(5, !selected_survey.getDoRecomputeError());
  ui->table_Filter->setColumnHidden(6, !selected_survey.getDoRecomputeError());
  ui->table_Filter->setColumnHidden(7, !selected_survey.getDoRecomputeError());
  ui->table_Filter->setColumnHidden(9, !selected_survey.getDefineFilterShift());
  ui->table_Filter->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_Filter->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_Filter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->table_Filter->update(QModelIndex());

  ui->btn_prop_err->setEnabled(ui->table_Filter->selectionModel()->hasSelection());

  connect(ui->table_Filter->model(), SIGNAL(itemChanged(QStandardItem*)), SLOT(filter_model_changed(QStandardItem*)));
  connect(ui->table_Filter->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
          SLOT(filterMappingSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

void FormSurveyMapping::loadMappingPage(std::shared_ptr<SurveyModel> survey_model_ptr, DatasetRepo filter_repository,
                                        std::string new_path) {

  logger.info() << "Load the Catalog Page";
  m_survey_model_ptr = survey_model_ptr;
  ui->txt_nonDetection->setValidator(new QDoubleValidator());
  ui->txt_UpperLimit->setValidator(new QDoubleValidator());
  m_filter_repository = filter_repository;

  loadCatalogCB(m_survey_model_ptr->getSelectedSurvey().getName());

  if (new_path.length() > 0) {
    on_btn_MapNew_clicked();
    m_survey_model_ptr->setDefaultCatalogToSelected(QString::fromStdString(m_default_survey));
    m_default_survey = new_path;
    ui->tb_df->setText(QString::fromStdString(m_default_survey));

    loadColumnFromFile(new_path);
    m_survey_model_ptr->setColumnListToSelected(m_column_from_file);
  } else {
    updateSelection();
  }
}

void FormSurveyMapping::setFilterMappingInEdition() {

  ui->frm_nav->setEnabled(false);
  ui->cb_catalog_type->setEnabled(false);
  ui->btn_MapNew->setEnabled(false);
  ui->btn_MapDuplicate->setEnabled(false);
  ui->btn_map_delete->setEnabled(false);
  ui->btn_MapCancel->setEnabled(true);
  ui->btn_MapSave->setEnabled(true);

  bool has_mapping_selected = m_survey_model_ptr->getSelectedRow() >= 0;
  ui->cb_SourceId->setEnabled(has_mapping_selected);
  ui->cb_Ra->setEnabled(has_mapping_selected);
  ui->cb_Dec->setEnabled(has_mapping_selected);
  ui->cb_GalEbv->setEnabled(has_mapping_selected);
  ui->cb_RefZ->setEnabled(has_mapping_selected);

  ui->txt_nonDetection->setEnabled(has_mapping_selected);
  ui->txt_UpperLimit->setEnabled(has_mapping_selected);
  ui->ckb_error_prop->setEnabled(has_mapping_selected);
  ui->ckb_filterShift->setEnabled(has_mapping_selected);
  ui->btn_SelectFilters->setEnabled(has_mapping_selected);

  ui->btn_prop_err->setEnabled(false);
  m_filterInsert = false;
}

void FormSurveyMapping::setFilterMappingInView() {
  bool has_mapping_selected = m_survey_model_ptr->getSelectedRow() >= 0;
  ui->frm_nav->setEnabled(true);
  ui->cb_catalog_type->setEnabled(true);
  ui->btn_MapNew->setEnabled(true);
  ui->btn_MapDuplicate->setEnabled(has_mapping_selected);
  ui->btn_map_delete->setEnabled(has_mapping_selected);
  ui->btn_MapCancel->setEnabled(false);
  ui->btn_MapSave->setEnabled(false);

  ui->btn_ImportColumn->setEnabled(has_mapping_selected);
  ui->cb_SourceId->setEnabled(has_mapping_selected);
  ui->cb_Ra->setEnabled(has_mapping_selected);
  ui->cb_Dec->setEnabled(has_mapping_selected);
  ui->cb_GalEbv->setEnabled(has_mapping_selected);
  ui->cb_RefZ->setEnabled(has_mapping_selected);

  ui->txt_nonDetection->setEnabled(has_mapping_selected);
  ui->txt_UpperLimit->setEnabled(has_mapping_selected);
  ui->ckb_error_prop->setEnabled(has_mapping_selected);
  ui->ckb_filterShift->setEnabled(has_mapping_selected);
  ui->btn_SelectFilters->setEnabled(has_mapping_selected);

  m_mappingInsert = false;
}

void FormSurveyMapping::fillCbColumns(std::string current_id_value, std::string current_ra_value,
                                      std::string current_dec_value, std::string current_gebv_value,
                                      std::string current_refz_value) {
  ui->cb_SourceId->clear();
  ui->cb_SourceId->addItem("");
  ui->cb_Ra->clear();
  ui->cb_Ra->addItem("");
  ui->cb_Dec->clear();
  ui->cb_Dec->addItem("");
  ui->cb_GalEbv->clear();
  ui->cb_GalEbv->addItem("");
  ui->cb_RefZ->clear();
  ui->cb_RefZ->addItem("");

  bool found_id   = false;
  bool found_ra   = false;
  bool found_dec  = false;
  bool found_ebv  = false;
  bool found_refz = false;
  int  index      = 1;

  for (auto item : m_column_from_file) {
    ui->cb_SourceId->addItem(QString::fromStdString(item));
    ui->cb_Ra->addItem(QString::fromStdString(item));
    ui->cb_Dec->addItem(QString::fromStdString(item));
    ui->cb_GalEbv->addItem(QString::fromStdString(item));
    ui->cb_RefZ->addItem(QString::fromStdString(item));
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
    if (item.compare(current_refz_value) == 0) {
      found_refz = true;
      ui->cb_RefZ->setCurrentIndex(index);
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
  if (!found_refz) {
    ui->cb_RefZ->setItemText(0, QString::fromStdString(current_refz_value));
  }
}

void FormSurveyMapping::loadColumnFromFile(std::string path) {
  auto column_reader = PhzUITools::CatalogColumnReader(path);
  m_column_from_file = column_reader.getColumnNames();
  m_survey_model_ptr->setColumnListToSelected(m_column_from_file);
  m_survey_model_ptr->setDefaultCatalogToSelected(QString::fromStdString(path));

  auto current_id_text   = ui->cb_SourceId->currentText();
  auto current_ra_text   = ui->cb_Ra->currentText();
  auto current_dec_text  = ui->cb_Dec->currentText();
  auto current_gebv_text = ui->cb_GalEbv->currentText();
  auto current_refz_text = ui->cb_RefZ->currentText();
  fillCbColumns(current_id_text.toStdString(), current_ra_text.toStdString(), current_dec_text.toStdString(),
                current_gebv_text.toStdString(), current_refz_text.toStdString());
}

void FormSurveyMapping::on_cb_SourceId_currentIndexChanged(int) {
  m_survey_model_ptr->setIdColumnToSelected(ui->cb_SourceId->currentText());
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_cb_Dec_currentIndexChanged(int) {
  m_survey_model_ptr->setDecColumnToSelected(ui->cb_Dec->currentText());
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_cb_Ra_currentIndexChanged(int) {
  m_survey_model_ptr->setRaColumnToSelected(ui->cb_Ra->currentText());
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_cb_GalEbv_currentIndexChanged(int) {
  m_survey_model_ptr->setGalEbvColumnToSelected(ui->cb_GalEbv->currentText());
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_cb_RefZ_currentIndexChanged(int) {
  m_survey_model_ptr->setRefZColumnToSelected(ui->cb_RefZ->currentText());
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_txt_nonDetection_textEdited(const QString& text) {
  m_survey_model_ptr->setNonDetectionToSelected(text);
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_txt_UpperLimit_textEdited(const QString& text) {
  m_survey_model_ptr->setHasUpperLimitToSelected(text);
  setFilterMappingInEdition();
}

void FormSurveyMapping::filter_model_changed(QStandardItem*) {
  logger.info() << "Table filter model changed";
  setFilterMappingInEdition();
}

void FormSurveyMapping::on_btn_prop_err_clicked() {
  if (ui->table_Filter->selectionModel()->hasSelection()) {
    bool            do_err    = m_survey_model_ptr->getSelectedSurvey().getDoRecomputeError();
    QModelIndexList selection = ui->table_Filter->selectionModel()->selectedRows();
    QModelIndex     index     = selection.at(0);
    FilterModel*    model     = static_cast<FilterModel*>(ui->table_Filter->model());
    FilterMapping   filter    = model->getFilter(index.row());
    double          n         = filter.getN();
    double          alpha     = filter.getAlpha();
    double          beta      = filter.getBeta();
    double          gamma     = filter.getGamma();
    QString         message   = "Do you really want to set the following values to all the filters?\n \n"
                                "Upper limit over error ratio = " +
                      QString::number(n);
    if (do_err) {
      message += "\n"
                 "Alpha = " +
                 QString::number(alpha) +
                 "\n"
                 "Beta = " +
                 QString::number(beta) +
                 "\n"
                 "Gamma = " +
                 QString::number(gamma) + "\n";
    }

    if (QMessageBox::question(this, "Confirm copy...", message, QMessageBox::Yes | QMessageBox::No) ==
        QMessageBox::Yes) {

      for (size_t index_col = 0; index_col < model->getFilters().size(); ++index_col) {
        model->setItem(index_col, 4, new QStandardItem(QString::number(n)));
        if (do_err) {
          model->setItem(index_col, 5, new QStandardItem(QString::number(alpha)));
          model->setItem(index_col, 6, new QStandardItem(QString::number(beta)));
          model->setItem(index_col, 7, new QStandardItem(QString::number(gamma)));
        }
      }
      setFilterMappingInEdition();
    }
  }
}

void FormSurveyMapping::on_ckb_error_prop_stateChanged(int state) {
  m_survey_model_ptr->setDoRecomputeErrorToSelected(state);

  ui->table_Filter->setColumnHidden(5, state == 0);
  ui->table_Filter->setColumnHidden(6, state == 0);
  ui->table_Filter->setColumnHidden(7, state == 0);

  setFilterMappingInEdition();
}

void FormSurveyMapping::on_ckb_filterShift_stateChanged(int state) {
  m_survey_model_ptr->setDefineFilterShiftToSelected(state);

  ui->table_Filter->setColumnHidden(9, state == 0);

  setFilterMappingInEdition();
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

void FormSurveyMapping::on_btn_ToPP_clicked() {
  navigateToPostProcessing(false);
}

void FormSurveyMapping::on_btn_exit_clicked() {
  quit(true);
}

void FormSurveyMapping::copyingFinished(bool s, QVector<QString> path) {
  if (s) {
    logger.info() << "file copied to " << path[0].toStdString();
    loadColumnFromFile(path[0].toStdString());
    ui->table_Filter->setItemDelegateForColumn(1, new FilterMappingItemDelegate(getFilteredColumns()));
    ui->table_Filter->setItemDelegateForColumn(2, new FilterMappingItemDelegate(getFilteredColumns()));
    ui->table_Filter->setItemDelegateForColumn(9, new FilterMappingItemDelegate(getFilteredColumns(), "NONE"));
    m_default_survey = path[0].toStdString();
    m_survey_model_ptr->setDefaultCatalogToSelected(QString::fromStdString(m_default_survey));
    m_survey_model_ptr->setColumnListToSelected(m_column_from_file);
    updateSelection();
  } else {
    logger.warn() << "Copy of the catalog file failed";
  }
}

void FormSurveyMapping::copyProgress(qint64 copy, qint64 total) {
  logger.info() << "File copy progress => " << QString::number((qreal(copy) / qreal(total)) * 100.0).toStdString()
                << "%";
}

void FormSurveyMapping::on_btn_MapNew_clicked() {
  std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
  popUp->setExistingNames(SurveyFilterMapping::getAvailableCatalogs());
  if (popUp->exec() == QDialog::Accepted) {
    m_survey_model_ptr->newSurvey(false);
    m_survey_model_ptr->setNameToSelected(QString::fromStdString(popUp->getName()));
    m_survey_model_ptr->saveSelected();

    auto      file_to_copy = popUp->getFilePath();
    QFileInfo info(QString::fromStdString(file_to_copy));
    if (info.exists()) {
      auto base_path = FileUtils::getCatalogRootPath(true, popUp->getName());
      auto full      = QString::fromStdString(base_path + "/") + info.fileName();

      auto local  = new QThread;
      auto worker = new FileCopyer(local);
      QObject::connect(worker, SIGNAL(finished(bool, QVector<QString>)), SLOT(copyingFinished(bool, QVector<QString>)));
      QObject::connect(worker, SIGNAL(copyProgress(qint64, qint64)), SLOT(copyProgress(qint64, qint64)));
      worker->setSourcePaths({QString::fromStdString(file_to_copy)});
      worker->setDestinationPaths({full});
      local->start();

      logger.info() << "copy of " << file_to_copy << " launched";

    } else {
      logger.info() << file_to_copy << " not found";
    }

    loadCatalogCB(popUp->getName());
    setFilterMappingInEdition();
    m_mappingInsert = true;
  }
}

void FormSurveyMapping::on_btn_MapDuplicate_clicked() {
  std::unique_ptr<DialogCatalogName> popUp(new DialogCatalogName());
  popUp->setExistingNames(SurveyFilterMapping::getAvailableCatalogs());
  popUp->setDefaultName(m_survey_model_ptr->getSelectedSurvey().getName() + "_COPY");
  popUp->setDefaultCopiedFile(m_survey_model_ptr->getSelectedSurvey().getDefaultCatalogFile());
  if (popUp->exec() == QDialog::Accepted) {
    m_survey_model_ptr->newSurvey(true);
    m_survey_model_ptr->setNameToSelected(QString::fromStdString(popUp->getName()));
    m_survey_model_ptr->saveSelected();

    auto      file_to_copy = popUp->getFilePath();
    QFileInfo info(QString::fromStdString(file_to_copy));
    if (info.exists()) {
      auto base_path = FileUtils::getCatalogRootPath(true, popUp->getName());
      auto full      = QString::fromStdString(base_path + "/") + info.fileName();

      auto local  = new QThread;
      auto worker = new FileCopyer(local);
      QObject::connect(worker, SIGNAL(finished(bool, QVector<QString>)), SLOT(copyingFinished(bool, QVector<QString>)));
      QObject::connect(worker, SIGNAL(copyProgress(qint64, qint64)), SLOT(copyProgress(qint64, qint64)));
      worker->setSourcePaths({QString::fromStdString(file_to_copy)});
      worker->setDestinationPaths({full});
      local->start();

      logger.info() << "copy of " << file_to_copy << " launched";

    } else {
      logger.info() << file_to_copy << " not found";
    }

    loadCatalogCB(popUp->getName());
    setFilterMappingInEdition();
    m_mappingInsert = true;
  }
}

void FormSurveyMapping::on_btn_map_delete_clicked() {
  const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();
  int                        row             = m_survey_model_ptr->getSelectedRow();
  if (row >= 0) {
    std::string catalog_name      = selected_survey.getName();
    std::string catalog_path      = FileUtils::getCatalogRootPath(false, catalog_name);
    std::string intermediate_path = FileUtils::getIntermediaryProductRootPath(false, catalog_name);
    std::string result_path       = FileUtils::getResultRootPath(false, catalog_name, "");

    if (QMessageBox::question(
            this, "Confirm deletion...",
            "Do you really want to delete the Catalog Type '" + QString::fromStdString(catalog_name) +
                "' ?\n \n"
                "!!! WARNING !!!\n"
                "This action will also DELETE :\n"
                "    - The Catalog folder  '" +
                QString::fromStdString(catalog_path) +
                "' and its content,\n"
                "    - All the related intermediate products (Model Grids,...) you may have computed and stored in '" +
                QString::fromStdString(intermediate_path) +
                "',\n"
                "\nHowever related results in the folder '" +
                QString::fromStdString(result_path) + "' will not be deleted.",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

      bool success = true;
      // The intermediate folder
      success &= FileUtils::removeDir(QString::fromStdString(intermediate_path));

      // The catalog folder
      success &= FileUtils::removeDir(QString::fromStdString(catalog_path));

      if (success) {
        // The xml file
        m_survey_model_ptr->deletSelected();
        updateSelection(true);
        on_cb_catalog_type_currentIndexChanged(-1);
        fillControlsWithSelected();
        setFilterMappingInView();
      }
    }
  }
}


void FormSurveyMapping::on_btn_purgeGrids_clicked() {
	 const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();
	  int                        row             = m_survey_model_ptr->getSelectedRow();
	  if (row >= 0) {
	    std::string catalog_name      = selected_survey.getName();
	    std::string intermediate_path = FileUtils::getIntermediaryProductRootPath(false, catalog_name);
	    std::vector<std::string> sub_folders {"ModelGrids","LuminosityModelGrids","GalacticCorrectionCoefficientGrids","FilterVariationCoefficientGrids"};

	    std::vector<int> content{0,0,0,0};
	    std::vector<long> size{0,0,0,0};
	    std::vector<std::string> size_str{"0 B","0 B","0 B","0 B"};

	    auto cont_iter = content.begin();
	    auto size_iter = size.begin();
	    auto size_str_iter = size_str.begin();
	    for (auto folder_iter = sub_folders.begin(); folder_iter != sub_folders.end(); ++folder_iter){
	    	auto path = intermediate_path+ "/"+*folder_iter;
	    	int count = 0;
	    	if (boost::filesystem::exists(path)){
				for (auto& obj : boost::filesystem::directory_iterator(path)) {
					 if (!boost::filesystem::is_directory(obj)) {
						 (*size_iter) +=boost::filesystem::file_size(obj);
					 }
					 ++count;
				}
				(*cont_iter)=count;

				int i{};
				double mantissa = *size_iter;
				for (; mantissa >= 1024.; mantissa /= 1024., ++i) { }
				mantissa = std::round(mantissa);
				(*size_str_iter)= std::to_string(int(mantissa)) + " KMGTPE"[i] + "B";

	    	}
	    	++cont_iter;
	    	++size_iter;
	    	++size_str_iter;
	    }



	    if (QMessageBox::question(
	            this, "Confirm deletion...",
	            "Do you really want to delete the Grids computed for the Catalog Type '" + QString::fromStdString(catalog_name) +
	                "' ?\n \n"
	                "This action will also DELETE all the Grid intermediate products stored in '" +
	                QString::fromStdString(intermediate_path) +
	                "':\n"
					" - "+QString::fromStdString(sub_folders[0])+" : "+ QString::number(content[0])+ " files ("+QString::fromStdString(size_str[0])+"),\n"
					" - "+QString::fromStdString(sub_folders[1])+" : "+ QString::number(content[1])+ " files ("+QString::fromStdString(size_str[1])+"),\n"
					" - "+QString::fromStdString(sub_folders[2])+" : "+ QString::number(content[2])+ " files ("+QString::fromStdString(size_str[2])+"),\n"
					" - "+QString::fromStdString(sub_folders[3])+" : "+ QString::number(content[3])+ " files ("+QString::fromStdString(size_str[3])+").\n"
					"\n"
	                "\n(You will be able to recompute them if you need them again)",
	            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

	      bool success = true;
	      // The intermediate folders
	      success &= FileUtils::removeDir(QString::fromStdString(intermediate_path+"/"+sub_folders[0]));
	      success &= FileUtils::removeDir(QString::fromStdString(intermediate_path+"/"+sub_folders[1]));
	      success &= FileUtils::removeDir(QString::fromStdString(intermediate_path+"/"+sub_folders[2]));
	      success &= FileUtils::removeDir(QString::fromStdString(intermediate_path+"/"+sub_folders[3]));

	    }
	  }
}

void FormSurveyMapping::on_btn_MapCancel_clicked() {
  if (m_mappingInsert) {
    std::string catalog_name      = m_survey_model_ptr->getSelectedSurvey().getName();
    std::string catalog_path      = FileUtils::getCatalogRootPath(false, catalog_name);
    std::string intermediate_path = FileUtils::getIntermediaryProductRootPath(false, catalog_name);
    FileUtils::removeDir(QString::fromStdString(intermediate_path));
    FileUtils::removeDir(QString::fromStdString(catalog_path));
    m_survey_model_ptr->deletSelected();
    loadCatalogCB(m_survey_model_ptr->getSelectedSurvey().getName());
    on_cb_catalog_type_currentIndexChanged(-1);

  } else {
    m_survey_model_ptr->cancelSelected();
    updateSelection(false);
  }
}

void FormSurveyMapping::on_btn_MapSave_clicked() {
  if (ui->cb_SourceId->currentText().trimmed().length() == 0) {
    QMessageBox::warning(this, "Missing Data...", "Please provide the name of the Source ID Column.", QMessageBox::Ok);
    return;
  }

  auto filters = getMappingFromGrid();
  for (auto& filter : filters) {
    if (filter.getFluxColumn().length() == 0 || filter.getErrorColumn().length() == 0) {
      QMessageBox::warning(this, "Missing Data...", "Please provide a name for all the Flux and Error Columns.",
                           QMessageBox::Ok);
      return;
    }
  }
  m_survey_model_ptr->setFiltersToSelected(filters);
  if (m_survey_model_ptr->saveSelected()) {
    m_filterInsert = false;
    updateSelection();

  } else {
    // Never here because the duplicate name is tested in the popup for name selection
  }
}

void FormSurveyMapping::filterMappingSelectionChanged(const QItemSelection&, const QItemSelection&) {
  ui->btn_prop_err->setEnabled(ui->table_Filter->selectionModel()->hasSelection());
}

void FormSurveyMapping::on_btn_ImportColumn_clicked() {
  const SurveyFilterMapping& selected_survey = m_survey_model_ptr->getSelectedSurvey();
  std::string                name            = selected_survey.getName();
  QFileDialog                dialog(this);
  dialog.selectFile(QString::fromStdString(FileUtils::getCatalogRootPath(true, name)) + QDir::separator());

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
  for (int i = 0; i < ui->table_Filter->model()->rowCount(); ++i) {
    filters.push_back(ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 3)).toString().toStdString());
  }
  return filters;
}

// to get from the controls
std::vector<FilterMapping> FormSurveyMapping::getMappingFromGrid() const {
  std::vector<FilterMapping> filters{};
  for (int i = 0; i < ui->table_Filter->model()->rowCount(); ++i) {
    auto name    = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 3)).toString().toStdString();
    auto flux    = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 1)).toString().toStdString();
    auto err     = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 2)).toString().toStdString();
    auto n       = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 4)).toString().toDouble();
    auto alpha   = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 5)).toString().toDouble();
    auto beta    = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 6)).toString().toDouble();
    auto gamma   = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 7)).toString().toDouble();
    bool fromMag = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 8)).toString().toInt();
    auto shift   = ui->table_Filter->model()->data(ui->table_Filter->model()->index(i, 9)).toString().toStdString();

    FilterMapping mapping{};
    mapping.setFilterFile(name);
    mapping.setFluxColumn(flux);
    mapping.setErrorColumn(err);
    mapping.setN(n);
    mapping.setAlpha(alpha);
    mapping.setBeta(beta);
    mapping.setGamma(gamma);
    mapping.setFromMag(fromMag);
    mapping.setShiftColumn(shift);
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

  auto                       existing_filters = getMappingFromGrid();
  std::vector<FilterMapping> new_filters{};
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
  ui->table_Filter->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->table_Filter->update(QModelIndex());

  m_survey_model_ptr->setFiltersToSelected(new_filters);

  ui->table_Filter->setItemDelegateForColumn(1, new FilterMappingItemDelegate(getFilteredColumns()));
  ui->table_Filter->setItemDelegateForColumn(2, new FilterMappingItemDelegate(getFilteredColumns()));
  ui->table_Filter->setItemDelegateForColumn(9, new FilterMappingItemDelegate(getFilteredColumns(), "NONE"));

  setFilterMappingInEdition();
}

}  // namespace PhzQtUI
}  // namespace Euclid
