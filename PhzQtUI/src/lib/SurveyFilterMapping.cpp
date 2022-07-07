#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QFileInfo>
#include <QTextStream>
#include <boost/algorithm/string.hpp>

#include "PhzDataModel/AdjustErrorParamMap.h"
#include "PhzQtUI/SurveyFilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("SurveyFilterMapping");

SurveyFilterMapping::SurveyFilterMapping() {}

int SurveyFilterMapping::getFilterNumber() const {
  return m_filters.size();
}

void SurveyFilterMapping::setName(std::string newSurveyName) {
  m_survey_name = newSurveyName;
}

std::string SurveyFilterMapping::getName() const {
  return m_survey_name;
}

void SurveyFilterMapping::setSourceIdColumn(std::string newSourceIdColumn) {
  m_source_id_column = newSourceIdColumn;
}

std::string SurveyFilterMapping::getSourceIdColumn() const {
  return m_source_id_column;
}

void SurveyFilterMapping::setRaColumn(std::string newRaColumn) {
  m_ra_column = newRaColumn;
}

std::string SurveyFilterMapping::getRaColumn() const {
  return m_ra_column;
}

void SurveyFilterMapping::setDecColumn(std::string newDecColumn) {
  m_dec_column = newDecColumn;
}

std::string SurveyFilterMapping::getDecColumn() const {
  return m_dec_column;
}

void SurveyFilterMapping::setGalEbvColumn(std::string newGalEbvolumn) {
  m_gal_ebv_column = newGalEbvolumn;
}
std::string SurveyFilterMapping::getGalEbvColumn() const {
  return m_gal_ebv_column;
}

void SurveyFilterMapping::setRefZColumn(std::string newRefZColumn) {
  m_ref_z_column = newRefZColumn;
}
std::string SurveyFilterMapping::getRefZColumn() const {
  return m_ref_z_column;
}

void SurveyFilterMapping::setFilters(std::vector<FilterMapping> filters) {
  m_filters = std::move(filters);
}

const std::set<std::string>& SurveyFilterMapping::getColumnList() const {
  return m_column_list;
}

void SurveyFilterMapping::setColumnList(std::set<std::string> new_list) {
  m_column_list = std::move(new_list);
}

const std::vector<FilterMapping>& SurveyFilterMapping::getFilters() const {
  return m_filters;
}

void SurveyFilterMapping::setDefaultCatalogFile(std::string new_default_catalog) {
  m_default_catalog = new_default_catalog;
}

std::string SurveyFilterMapping::getDefaultCatalogFile() const {
  return m_default_catalog;
}

void SurveyFilterMapping::setNonDetection(double non_detection) {
  m_non_detection = non_detection;
}

double SurveyFilterMapping::getNonDetection() const {
  return m_non_detection;
}

void SurveyFilterMapping::setUpperLimit(double non_detection) {
  m_upper_limit = non_detection;
}

double SurveyFilterMapping::getUpperLimit() const {
  return m_upper_limit;
}

void SurveyFilterMapping::setDoRecomputeError(bool do_recompute_error) {
  m_do_recompute_error = do_recompute_error;
}

bool SurveyFilterMapping::getDoRecomputeError() const {
  return m_do_recompute_error;
}

void SurveyFilterMapping::setDefineFilterShift(bool define_filter_shift) {
  m_define_filter_shift = define_filter_shift;
}

bool SurveyFilterMapping::getDefineFilterShift() const {
  return m_define_filter_shift;
}

void SurveyFilterMapping::setCopiedColumns(std::map<std::string, std::string> copied_columns) {
  m_copied_columns = copied_columns;
}

const std::map<std::string, std::string>& SurveyFilterMapping::getCopiedColumns() const {
  return m_copied_columns;
}

std::vector<std::string> SurveyFilterMapping::getAvailableCatalogs() {
  auto                     cat_root_path = FileUtils::getCatalogRootPath(true, "");
  std::vector<std::string> all_dirs{};
  QDirIterator directories(QString::fromStdString(cat_root_path), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                           QDirIterator::NoIteratorFlags);
  while (directories.hasNext()) {
    directories.next();
    all_dirs.push_back(directories.fileName().toStdString());
  }

  return all_dirs;
}

std::map<int, SurveyFilterMapping> SurveyFilterMapping::loadCatalogMappings() {
  auto catalog_config_path = QString::fromStdString(FileUtils::getCatalogConfigRootPath(true));

  std::map<int, SurveyFilterMapping> mappings{};
  int                                id = 0;

  // 1) list the folders under the Catalog folder
  for (auto& catalog_name : getAvailableCatalogs()) {
    // look for existing config
    auto catalog_conf = catalog_config_path + QDir::separator() + QString::fromStdString(catalog_name) + ".xml";

    QFileInfo info(catalog_conf);
    if (info.exists()) {
      mappings[id] = SurveyFilterMapping::loadCatalog(catalog_name + ".xml");
    } else {
      SurveyFilterMapping mapping{};
      mapping.setName(catalog_name);
      mapping.ReadFilters();
      mappings[id] = std::move(mapping);
    }
    ++id;
  }

  return mappings;
}

SurveyFilterMapping SurveyFilterMapping::loadCatalog(std::string name) {
  auto additional_info_path =
      QString::fromStdString(FileUtils::getGUIConfigPath()) + QDir::separator() + "Catalogs" + QDir::separator();

  SurveyFilterMapping survey;
  survey.setName(FileUtils::removeExt(name, ".xml"));

  // read the xml encoded additional info
  QDomDocument doc("CatalogInfo");
  QFile        file(additional_info_path + QDir::separator() + QString::fromStdString(name));
  if (!file.open(QIODevice::ReadOnly))
    return survey;
  if (!doc.setContent(&file)) {
    file.close();
    return survey;
  }
  file.close();

  QDomElement root_node = doc.documentElement();
  survey.setSourceIdColumn(root_node.attribute("SourceColumnId").toStdString());
  survey.setRaColumn(root_node.attribute("RaColumn").toStdString());
  survey.setDecColumn(root_node.attribute("DecColumn").toStdString());
  survey.setGalEbvColumn(root_node.attribute("GalEbvColumn").toStdString());

  std::string ref_col = "";
  if (root_node.hasAttribute("RefZColumn")) {
    ref_col = root_node.attribute("RefZColumn").toStdString();
  }
  survey.setRefZColumn(ref_col);

  std::string path = root_node.attribute("DefaultCatalogPath").toStdString();

  if (!FileUtils::starts_with(path, "/")) {
    std::string root_path = FileUtils::getRootPath(true);
    path                  = root_path + path;
  }

  survey.setDefaultCatalogFile(path);

  if (root_node.hasAttribute("NonDetection")) {
    survey.setNonDetection(root_node.attribute("NonDetection").toDouble());
  }

  if (root_node.hasAttribute("UpperLimit")) {
    survey.setUpperLimit(root_node.attribute("UpperLimit").toDouble());
  }

  if (root_node.hasAttribute("DoRecomputeError")) {
    survey.setDoRecomputeError(root_node.attribute("DoRecomputeError").toInt());
  }

  if (root_node.hasAttribute("DefineFilterShift")) {
    survey.setDefineFilterShift(root_node.attribute("DefineFilterShift").toInt());
  }

  auto columns_node = root_node.firstChildElement("AvailableColumns");
  auto list         = columns_node.childNodes();
  survey.m_column_list.clear();
  for (int i = 0; i < list.count(); ++i) {
    auto node_column = list.at(i).toElement();
    survey.m_column_list.insert(node_column.text().toStdString());
  }

  auto copy_columns_node = root_node.firstChildElement("CopiedColumns");
  list                   = copy_columns_node.childNodes();
  survey.m_copied_columns.clear();
  for (int i = 0; i < list.count(); ++i) {
    auto        node_column = list.at(i).toElement();
    std::string copied_col  = node_column.text().toStdString();

    auto                   col_name    = copied_col;
    std::string            col_aliases = "";
    std::string::size_type pos         = copied_col.find(":");
    if (pos != std::string::npos) {
      col_aliases = col_name.substr(pos + 1);
      col_name    = col_name.substr(0, pos);
    }

    survey.m_copied_columns[col_name] = col_aliases;
  }

  if (survey.m_column_list.count(survey.getSourceIdColumn()) == 0) {
    survey.m_column_list.insert(survey.getSourceIdColumn());
  }

  survey.ReadFilters();

  return survey;
}

void SurveyFilterMapping::ReadFilters() {
  std::vector<FilterMapping> mappings{};

  auto mapping_path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(true, getName())) +
                      QDir::separator() + "filter_mapping.txt";

  try {

    bool                     header_found              = false;
    int                      filtr_column_index        = 0;
    int                      flux_column_index         = 1;
    int                      error_column_index        = 2;
    int                      upper_limit_column_index  = -1;
    int                      convertion_column_index   = -1;
    int                      filter_shift_column_index = -1;
    std::vector<std::string> expected_column_name{
        "Filter", "Flux Column", "Error Column", "Upper Limit/error ratio", "Convert from MAG", "Filter Shift Column"};

    std::ifstream in{mapping_path.toStdString()};
    std::string   line;
    while (std::getline(in, line)) {
      boost::trim(line);
      if (line[0] == '#') {
        if (!header_found) {
          std::string trimmed_line = line.substr(1);
          boost::trim(trimmed_line);
          std::vector<int>         proposed_column_index{-1, -1, -1, -1, -1, -1};
          std::vector<std::string> strs;
          boost::split(strs, trimmed_line, boost::is_any_of(","));

          for (size_t index = 0; index < expected_column_name.size(); ++index) {
            for (size_t index_string = 0; index_string < strs.size(); ++index_string) {
              std::string item = strs[index_string];
              boost::trim(item);
              if (item == expected_column_name[index]) {
                proposed_column_index[index] = index_string;
              }
            }
          }

          if (proposed_column_index[0] >= 0 && proposed_column_index[1] >= 0 && proposed_column_index[2] >= 0) {
            header_found              = true;
            filtr_column_index        = proposed_column_index[0];
            flux_column_index         = proposed_column_index[1];
            error_column_index        = proposed_column_index[2];
            upper_limit_column_index  = proposed_column_index[3];
            convertion_column_index   = proposed_column_index[4];
            filter_shift_column_index = proposed_column_index[5];
          }
        }
        continue;
      }

      std::vector<std::string> cells;
      boost::split(cells, line, boost::is_any_of(" "));

      if (int(cells.size()) <= filtr_column_index || int(cells.size()) <= flux_column_index ||
          int(cells.size()) <= error_column_index) {
        throw Elements::Exception() << "File with missing values for the mandatory fields";
      }
      std::string filter_value = cells[filtr_column_index];
      boost::trim(filter_value);
      std::string flux_value = cells[flux_column_index];
      boost::trim(flux_value);
      std::string error_value = cells[error_column_index];

      FilterMapping mapping;
      mapping.setFilterFile(filter_value);
      mapping.setFluxColumn(flux_value);
      mapping.setErrorColumn(error_value);

      if (upper_limit_column_index > 0 && int(cells.size()) > upper_limit_column_index &&
          cells[upper_limit_column_index] != "") {
        float n = std::stof(cells[upper_limit_column_index]);
        mapping.setN(n);
      } else {
        mapping.setN(3.0);
      }

      if (convertion_column_index > 0 && int(cells.size()) > convertion_column_index &&
          cells[convertion_column_index] != "") {
        bool f = std::stoi(cells[convertion_column_index]);
        mapping.setFromMag(f);
      } else {
        mapping.setFromMag(false);
      }

      if (filter_shift_column_index > 0 && int(cells.size()) > filter_shift_column_index &&
          cells[filter_shift_column_index] != "" && cells[filter_shift_column_index] != "NONE") {
        std::string shift_value = cells[filter_shift_column_index];
        boost::trim(shift_value);
        if (shift_value == "") {
          shift_value = "NONE";
        }
        mapping.setShiftColumn(shift_value);
      } else {
        mapping.setShiftColumn("NONE");
      }

      mappings.push_back(mapping);
    }
  } catch (...) {
  }

  // read the error re-computation parameters
  auto error_param_path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(true, getName())) +
                          QDir::separator() + "error_adjustment_param.txt";
  try {
    std::ifstream in{error_param_path.toStdString()};
    auto          param_map = PhzDataModel::readAdjustErrorParamMap(in);

    for (auto& mapping : mappings) {
      // Check if element exists in map or not
      if (param_map.find(mapping.getFilterFile()) != param_map.end()) {
        auto tuple = param_map[mapping.getFilterFile()];
        mapping.setAlpha(std::get<0>(tuple));
        mapping.setBeta(std::get<1>(tuple));
        mapping.setGamma(std::get<2>(tuple));
      }
    }

  } catch (...) {

    logger.warn() << "Unable to read recomputation parameters saved in " << error_param_path.toStdString();
  }

  setFilters(mappings);
}

void SurveyFilterMapping::deleteSurvey() {
  auto additional_info_path =
      QString::fromStdString(FileUtils::getGUIConfigPath()) + QDir::separator() + "Catalogs" + QDir::separator();
  QFile(additional_info_path + QString::fromStdString(getName() + ".xml")).remove();
}

void SurveyFilterMapping::saveSurvey(std::string oldName) {
  // ensure the Catalog path
  auto      cat_path = QString::fromStdString(FileUtils::getCatalogRootPath(true, getName())) + QDir::separator();
  QFileInfo info(cat_path);
  if (!info.exists()) {
    QDir().mkpath(cat_path);
  }

  // save the xml additional info
  auto additional_info_path =
      QString::fromStdString(FileUtils::getGUIConfigPath()) + QDir::separator() + "Catalogs" + QDir::separator();
  QFile(additional_info_path + QString::fromStdString(oldName + ".xml")).remove();
  QFile file(additional_info_path + QString::fromStdString(getName() + ".xml"));
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);

  QDomDocument doc("CatalogInfo");
  QDomElement  root = doc.createElement("CatalogInfo");
  root.setAttribute("SourceColumnId", QString::fromStdString(m_source_id_column));
  root.setAttribute("RaColumn", QString::fromStdString(m_ra_column));
  root.setAttribute("DecColumn", QString::fromStdString(m_dec_column));
  root.setAttribute("GalEbvColumn", QString::fromStdString(m_gal_ebv_column));
  root.setAttribute("RefZColumn", QString::fromStdString(m_ref_z_column));
  root.setAttribute("NonDetection", QString::number(m_non_detection));
  root.setAttribute("UpperLimit", QString::number(m_upper_limit));
  root.setAttribute("DoRecomputeError", QString::number(m_do_recompute_error));
  root.setAttribute("DefineFilterShift", QString::number(m_define_filter_shift));

  std::string path      = m_default_catalog;
  std::string root_path = FileUtils::getRootPath(true);
  if (FileUtils::starts_with(path, root_path)) {
    path = FileUtils::removeStart(path, root_path);
  }

  root.setAttribute("DefaultCatalogPath", QString::fromStdString(path));

  doc.appendChild(root);

  QDomElement columns_node = doc.createElement("AvailableColumns");
  for (auto& column : m_column_list) {
    QDomElement text_element = doc.createElement("AvailableColumn");
    text_element.appendChild(doc.createTextNode(QString::fromStdString(column)));
    columns_node.appendChild(text_element);
  }
  root.appendChild(columns_node);

  QDomElement copied_columns_node = doc.createElement("CopiedColumns");
  for (auto& column : m_copied_columns) {
    QDomElement text_element = doc.createElement("CopiedColumn");

    QString text = QString::fromStdString(column.first);
    if (column.second != "" && column.second != column.first) {
      text = text + ":" + QString::fromStdString(column.second);
    }

    text_element.appendChild(doc.createTextNode(text));
    copied_columns_node.appendChild(text_element);
  }
  root.appendChild(copied_columns_node);

  QString xml = doc.toString();
  stream << xml;
  file.close();

  // save the filter mapping info
  auto mapping_path =
      QString::fromStdString(FileUtils::getIntermediaryProductRootPath(true, getName())) + QDir::separator();
  QFile mapping_file(mapping_path + "filter_mapping.txt");

  mapping_file.open(QIODevice::WriteOnly);
  QTextStream mapping_stream(&mapping_file);

  mapping_stream
      << "# Filter, Flux Column, Error Column, Upper Limit/error ratio, Convert from MAG, Filter Shift Column\n";
  for (auto& filter : m_filters) {
    std::string shift = m_define_filter_shift ? filter.getShiftColumn() : "NONE";
    mapping_stream << QString::fromStdString(filter.getFilterFile()) << " "
                   << QString::fromStdString(filter.getFluxColumn()) << " "
                   << QString::fromStdString(filter.getErrorColumn()) << " " << QString::number(filter.getN()) << " "
                   << QString::number(filter.getFromMag()) << " " << QString::fromStdString(shift) << "\n";
  }

  mapping_file.close();
  logger.info() << "Filter Mapping saved in " << mapping_path.toStdString() + "filter_mapping.txt";

  // Save the error recomputation params
  PhzDataModel::AdjustErrorParamMap param_map = {};
  for (auto& filter : m_filters) {
    auto filter_name = XYDataset::QualifiedName{filter.getFilterFile()};
    auto tuple       = std::make_tuple(filter.getAlpha(), filter.getBeta(), filter.getGamma());
    param_map.insert({filter_name, tuple});
  }

  std::string error_param_file = mapping_path.toStdString() + "error_adjustment_param.txt";

  std::filebuf fb;
  fb.open(error_param_file, std::ios::out);
  std::ostream os(&fb);
  if (param_map.size() > 0) {
    PhzDataModel::writeAdjustErrorParamMap(os, param_map);
  }
  fb.close();

  logger.info() << "Error recomputation parameters saved in " << error_param_file;
}

}  // namespace PhzQtUI
}  // namespace Euclid
