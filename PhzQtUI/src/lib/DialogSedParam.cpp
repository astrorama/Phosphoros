#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStringList>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <list>
#include <vector>

#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include "PhzQtUI/DialogSedParam.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/SedParamUtils.h"
#include "ui_DialogSedParam.h"
#include "PhzExecutables/BuildPPConfig.h"
#include "PhzDataModel/PPConfig.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogSedParam");

DialogSedParam::DialogSedParam(DatasetRepo sed_repository, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogSedParam) {
  ui->setupUi(this);
  m_sed_repository = sed_repository;
}

DialogSedParam::~DialogSedParam() {}

void DialogSedParam::setSed(const XYDataset::QualifiedName& sed) {
  m_file_path = SedParamUtils::getFile(sed);

  logger.info() << m_file_path;

  ui->lbl_sed_name->setText(QString::fromStdString(sed.qualifiedName()));
  std::string keyword       = "PARAMETER";
  auto        string_params = QString::fromStdString(m_sed_repository->getProvider()->getParameter(sed, keyword));

  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(6);
  grid_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  grid_model->setHeaderData(1, Qt::Horizontal, tr("A"));
  grid_model->setHeaderData(2, Qt::Horizontal, tr("B"));
  grid_model->setHeaderData(3, Qt::Horizontal, tr("C"));
  grid_model->setHeaderData(4, Qt::Horizontal, tr("D"));
  grid_model->setHeaderData(5, Qt::Horizontal, tr("Units"));

  std::vector<MessageButton*> message_buttons;

  PhzExecutables::BuildPPConfig parser{};
  std::map<std::string, PhzDataModel::PPConfig> param_map = parser.getParamMap(string_params.toStdString());
  for (auto const& param_iter : param_map) {
	  QList<QStandardItem*> items;
	  QStandardItem*        item = new QStandardItem(QString::fromStdString(param_iter.first));
	  items.push_back(item);
	  QStandardItem* itemA = new QStandardItem(QString::number(param_iter.second.getA()));
	  items.push_back(itemA);
	  QStandardItem* itemB = new QStandardItem(QString::number(param_iter.second.getB()));
	  items.push_back(itemB);
	  QStandardItem* itemC = new QStandardItem(QString::number(param_iter.second.getC()));
	  items.push_back(itemC);
	  QStandardItem* itemD = new QStandardItem(QString::number(param_iter.second.getD()));
	  items.push_back(itemD);
	  QStandardItem* uItem = new QStandardItem(QString::fromStdString(param_iter.second.getUnit()));
	  items.push_back(uItem);

	  grid_model->appendRow(items);
  }

  ui->table_param->setModel(grid_model);
}

void DialogSedParam::on_btn_cancel_clicked() {
  reject();
}

}  // namespace PhzQtUI
}  // namespace Euclid
