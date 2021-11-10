#include <map>
#include <QString>
#include "PhzQtUI/OptionModel.h"
#include "FileUtils.h"
#include "PreferencesUtils.h"
#include "PhzUtils/Multithreading.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "XYDataset/AsciiParser.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {
  OptionModel::OptionModel() {}

  void OptionModel::loadOption(DatasetRepo filter_repository,
                               DatasetRepo seds_repository,
                               DatasetRepo redenig_curves_repository,
                               DatasetRepo luminosity_repository) {
    m_filter_repository = filter_repository;
    m_seds_repository = seds_repository;
    m_redenig_curves_repository = redenig_curves_repository;
    m_luminosity_repository = luminosity_repository;

    m_global_edition = false;
    m_cosmo_edition = false;

    m_root_path = QString::fromStdString(FileUtils::getRootPath(false));

    auto path_map = FileUtils::readPath();
    m_cat_path_saved = QString::fromStdString(path_map["Catalogs"]);
    m_cat_path_edited = m_cat_path_saved;
    m_aux_path_saved = QString::fromStdString(path_map["AuxiliaryData"]);
    m_aux_path_edited = m_aux_path_saved;
    m_inter_path_saved = QString::fromStdString(path_map["IntermediateProducts"]);
    m_inter_path_edited = m_inter_path_saved;
    m_res_path_saved = QString::fromStdString(path_map["Results"]);
    m_res_path_edited = m_res_path_saved;

    size_t thread_value = PreferencesUtils::getThreadNumberOverride();
    m_override_thread_saved = thread_value < PhzUtils::getThreadNumber();
    m_override_thread_edited = m_override_thread_saved;
    if (m_override_thread_saved) {
      m_thread_number_saved = thread_value;
    } else {
      m_thread_number_saved = PhzUtils::getThreadNumber();
    }

    m_loglevel_saved=QString::fromStdString(PreferencesUtils::getLogLevel());
    m_loglevel_edited = m_loglevel_saved;

    m_thread_number_edited = m_thread_number_saved;

    m_buffer_size_saved = PreferencesUtils::getBufferSize();
    m_buffer_size_edited = m_buffer_size_saved;

    auto cosmology = PreferencesUtils::getCosmologicalParameters();
    m_hubble_saved = cosmology.getHubbleConstant();
    m_hubble_edited = m_hubble_saved;
    m_omega_m_saved = cosmology.getOmegaM();
    m_omega_m_edited = m_omega_m_saved;
    m_omega_l_saved = cosmology.getOmegaLambda();
    m_omega_l_edited = m_omega_l_saved;
  }

  bool OptionModel::isGlobalInEdition() {
    return m_global_edition;
  }

  bool OptionModel::isCosmoInEdition() {
    return m_cosmo_edition;
  }

  QString OptionModel::getRootPath() {
    return m_root_path;
  }

  QString OptionModel::getCatPath() {
    return m_cat_path_edited;
  }

  QString OptionModel::getAuxPath() {
    return m_aux_path_edited;
  }

  QString OptionModel::getInterPath() {
    return m_inter_path_edited;
  }

  QString OptionModel::getResPath() {
    return m_res_path_edited;
  }

  bool OptionModel::isCatPathDefault() {
    return QString::fromStdString(FileUtils::getDefaultCatalogRootPath()) == m_cat_path_edited;
  }

  bool OptionModel::isAuxPathDefault() {
    return QString::fromStdString(FileUtils::getDefaultAuxRootPath()) == m_aux_path_edited;
  }

  bool OptionModel::isInterPathDefault() {
    return QString::fromStdString(FileUtils::getDefaultIntermediaryProductRootPath()) == m_inter_path_edited;
  }

  bool OptionModel::isResPathDefault() {
    return QString::fromStdString(FileUtils::getDefaultResultsRootPath()) == m_res_path_edited;
  }

  bool OptionModel::getOverrideThread() {
    return m_override_thread_edited;
  }

  size_t OptionModel::getThreadNb() {
    return m_thread_number_edited;
  }

  QString OptionModel::getLogLevel() {
	return m_loglevel_edited;
  }

  size_t OptionModel::getBufferSize() {
    return m_buffer_size_edited;
  }

  double OptionModel::getHubble() {
    return m_hubble_edited;
  }

  double OptionModel::getOmegaM() {
    return m_omega_m_edited;
  }

  double OptionModel::getOmegaLambda() {
    return m_omega_l_edited;
  }

  DatasetRepo OptionModel::getFilterRepo() {
    return m_filter_repository;
  }

  DatasetRepo OptionModel::getSedRepo() {
    return m_seds_repository;
  }

  DatasetRepo OptionModel::getReddeningRepo() {
    return m_redenig_curves_repository;
  }

  DatasetRepo OptionModel::getLuminosityRepo() {
    return m_luminosity_repository;
  }

  void OptionModel::save() {
    if (m_global_edition) {
      m_cat_path_saved = m_cat_path_edited;
      m_aux_path_saved = m_aux_path_edited;
      m_inter_path_saved = m_inter_path_edited;
      m_res_path_saved = m_res_path_edited;

      m_buffer_size_saved = m_buffer_size_edited;

      m_override_thread_saved = m_override_thread_edited;
      m_thread_number_saved = m_thread_number_edited;
      m_loglevel_saved = m_loglevel_edited;

      std::map<std::string,std::string> map{};
      map.insert(std::make_pair("LastUsed", FileUtils::getLastUsedPath()));
      map.insert(std::make_pair("Catalogs", m_cat_path_saved.toStdString()));
      map.insert(std::make_pair("AuxiliaryData", m_aux_path_saved.toStdString()));
      map.insert(std::make_pair("IntermediateProducts", m_inter_path_saved.toStdString()));
      map.insert(std::make_pair("Results", m_res_path_saved.toStdString()));
      FileUtils::savePath(map);

      int thread_value = PhzUtils::getThreadNumber();
      if (m_override_thread_saved) {
        thread_value = m_thread_number_saved;
      }
      PreferencesUtils::setThreadNumberOverride(thread_value);
      PreferencesUtils::setBufferSize(m_buffer_size_saved);
      PreferencesUtils::setLogLevel(m_loglevel_saved.toStdString());

      std::unique_ptr <XYDataset::FileParser > filter_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> filter_provider(new XYDataset::FileSystemProvider{FileUtils::getFilterRootPath(true), std::move(filter_file_parser) });
      m_filter_repository->resetProvider(std::move(filter_provider));
      std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser) });
      m_seds_repository->resetProvider(std::move(sed_provider));
      std::unique_ptr <XYDataset::FileParser > reddening_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> red_curve_provider(new XYDataset::FileSystemProvider{  FileUtils::getRedCurveRootPath(true), std::move(reddening_file_parser) });
      m_redenig_curves_repository->resetProvider(std::move(red_curve_provider));
      std::unique_ptr <XYDataset::FileParser > luminosity_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> luminosity_curve_provider(new XYDataset::FileSystemProvider{  FileUtils::getLuminosityFunctionCurveRootPath(true), std::move(luminosity_file_parser) });
      m_luminosity_repository->resetProvider(std::move(luminosity_curve_provider));
      m_global_edition = false;
      repoReloaded(-1);
    }

    if (m_cosmo_edition) {
      m_hubble_saved = m_hubble_edited;
      m_omega_m_saved = m_omega_m_edited;
      m_omega_l_saved = m_omega_l_edited;
      PhysicsUtils::CosmologicalParameters cosmology{m_omega_m_saved, m_omega_l_saved, m_hubble_saved};
      PreferencesUtils::setCosmologicalParameters(cosmology);
      m_cosmo_edition = false;
    }
  }

  void OptionModel::cancel() {
    if (m_global_edition) {
      m_cat_path_edited = m_cat_path_saved;
      m_aux_path_edited = m_aux_path_saved;
      m_inter_path_edited = m_inter_path_saved;
      m_res_path_edited = m_res_path_saved;
      m_override_thread_edited = m_override_thread_saved;
      m_thread_number_edited = m_thread_number_saved;
      m_loglevel_edited = m_loglevel_saved;
      m_buffer_size_edited = m_buffer_size_saved;
      m_global_edition = false;
    }

    if (m_cosmo_edition) {
      m_hubble_edited = m_hubble_saved;
      m_omega_m_edited = m_omega_m_saved;
      m_omega_l_edited = m_omega_l_saved;
      m_cosmo_edition = false;
    }
  }

  void OptionModel::setCatalog(const QString & new_cat_path) {
    m_global_edition = true;
    if (new_cat_path != "") {
      m_cat_path_edited = new_cat_path;
    } else {
      m_cat_path_edited = QString::fromStdString(FileUtils::getDefaultCatalogRootPath());
    }
  }

  void OptionModel::setAuxiliary(const QString & new_aux_path) {
    m_global_edition = true;
    if (new_aux_path != "") {
      m_aux_path_edited = new_aux_path;
    } else {
      m_aux_path_edited = QString::fromStdString(FileUtils::getDefaultAuxRootPath());
    }
  }

  void OptionModel::setIntermediary(const QString & new_iter_path) {
    m_global_edition = true;
    if (new_iter_path != "") {
      m_inter_path_edited = new_iter_path;
    } else {
      m_inter_path_edited = QString::fromStdString(FileUtils::getDefaultIntermediaryProductRootPath());
    }
  }

  void OptionModel::setResult(const QString & new_res_path) {
    m_global_edition = true;
    if (new_res_path != "") {
      m_res_path_edited = new_res_path;
    } else {
      m_res_path_edited = QString::fromStdString(FileUtils::getDefaultResultsRootPath());
    }
  }

  void OptionModel::setThread(int thread_nb) {
    m_global_edition = true;
    if (m_override_thread_edited) {
      m_thread_number_edited = thread_nb;
    }
  }

  void OptionModel::setDefaultThread(bool custom) {
    m_global_edition = true;
    m_override_thread_edited = custom;
    if (!m_override_thread_edited) {
      m_thread_number_edited = PhzUtils::getThreadNumber();
    }
  }


  void OptionModel::setLogLevel(QString new_log_level) {
	  m_global_edition = true;
	  m_loglevel_edited = new_log_level;

  }

  void OptionModel::setBufferSize(int new_buffer_size) {
    m_global_edition = true;
    if (new_buffer_size > 0) {
        m_buffer_size_edited = new_buffer_size;
    }
  }


  void OptionModel::setHubble(const QString &  new_h0) {
    m_cosmo_edition = true;
    if (new_h0.toDouble() <= -1) {
      PhysicsUtils::CosmologicalParameters cosmology {};
      m_hubble_edited = cosmology.getHubbleConstant();
    } else {
      m_hubble_edited = new_h0.toDouble();
    }
  }

  void OptionModel::setOmegaM(const QString &  new_omega_m) {
    m_cosmo_edition = true;
    if (new_omega_m.toDouble() <= -1) {
      PhysicsUtils::CosmologicalParameters cosmology {};
      m_omega_m_edited = cosmology.getOmegaM();
    } else {
      m_omega_m_edited = new_omega_m.toDouble();
    }
  }

  void OptionModel::setOmegaLambda(const QString &  new_omega_lambda) {
    m_cosmo_edition = true;
    if (new_omega_lambda.toDouble() <= -1) {
      PhysicsUtils::CosmologicalParameters cosmology {};
      m_omega_l_edited = cosmology.getOmegaLambda();
    } else {
      m_omega_l_edited = new_omega_lambda.toDouble();
    }
  }



}
}
