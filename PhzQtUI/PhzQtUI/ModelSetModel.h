#ifndef MODELSETMODEL_H
#define MODELSETMODEL_H

#include "ModelSet.h"
#include "PhzQtUI/ModelSetModel.h"
#include <QStandardItemModel>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

/**
 * @brief The ModelSetModel class
 * This class provide a Model to be used in TableView.
 * It handle the ModelSet.
 */
class ModelSetModel : public QStandardItemModel {
  Q_OBJECT

public:
  ModelSetModel();
  virtual ~ModelSetModel() = default;

  /**
   * @brief Initialise the ModelSetModel by loading the ModelSet out of the files
   * present in the  directory.
   */
  void loadSets();

  /**
   * @brief Check that the 'new_name' do not conflict with other existing ModelSet name.
   * @param new_name
   * @param row
   * @return true if the name can be used
   */
  bool checkUniqueName(QString new_name, int row) const;

  /// Selection ///
  void            selectModelSet(int row);
  void            selectModelSet(QString name);
  const ModelSet& getSelectedModelSet() const;
  int             getSelectedRow() const;

  /// Creation / deletion / edition ///
  void newModelSet(bool duplicate_from_selected = false);
  void deleteSelected();
  bool isInEdition();
  bool saveSelected();
  void cancelSelected();

  /// Listing ///
  const std::vector<QString> getModelSetList() const;

  bool doNeedReload() const;

  void reloaded();

public slots:
  void setNameToSelected(const QString& value);
  void setGlobalRedshiftRangeToSelected(const std::vector<Range>& ranges, const std::set<double>& values);
  void setGlobalEbvRangeToSelected(const std::vector<Range>& ranges, const std::set<double>& values);
  void setParameterRulesToSelected(const std::map<int, ParameterRule>& value);

private:
  bool                    m_in_edition     = false;
  bool                    m_need_reload    = true;
  int                     m_selected_row   = -1;
  int                     m_selected_index = -1;
  ModelSet                m_edited_modelSet;
  std::map<int, ModelSet> m_set_list;

  const QString getValue(int row, int column) const;

  std::string getDuplicateName(std::string name) const;
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // MODELSETMODEL_H
