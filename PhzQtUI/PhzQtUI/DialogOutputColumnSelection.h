#ifndef DIALOGOUTPUTCOLUMNSELECTION_H
#define DIALOGOUTPUTCOLUMNSELECTION_H

#include <memory>
#include <map>
#include <QDialog>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogOutputColumnSelection;
}

/**
 * @class DialogOutputColumnSelection
 * @brief
 * */
class DialogOutputColumnSelection: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogOutputColumnSelection(
      std::list<std::string> all_columns,
      std::string id_column ="",
      std::map<std::string,std::string> copied_columns={},
      QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogOutputColumnSelection();


  signals:

  void selectedColumns(std::map<std::string,std::string>);

private slots:

  /**
   * @brief SLOT on_btn_save_clicked
   */
  void on_btn_save_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogOutputColumnSelection> ui;

  std::list<std::string> m_all_columns;
  std::string m_id_column;
  std::map<std::string,std::string> m_copied_columns;


};

}
}

#endif // DIALOGOUTPUTCOLUMNSELECTION_H
