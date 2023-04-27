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

  m_P = new QProcess;

  connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));
}

DialogSedParam::~DialogSedParam() {}

void DialogSedParam::setSed(const XYDataset::QualifiedName& sed) {
  m_file_path = SedParamUtils::getFile(sed);

  logger.info() << m_file_path;

  ui->lbl_sed_name->setText(QString::fromStdString(sed.qualifiedName()));
  std::string keyword       = "PARAMETER";
  auto        string_params = QString::fromStdString(m_sed_repository->getProvider()->getParameter(sed, keyword));

  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(7);
  grid_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  grid_model->setHeaderData(1, Qt::Horizontal, tr("A"));
  grid_model->setHeaderData(2, Qt::Horizontal, tr("B"));
  grid_model->setHeaderData(3, Qt::Horizontal, tr("C"));
  grid_model->setHeaderData(4, Qt::Horizontal, tr("D"));
  grid_model->setHeaderData(5, Qt::Horizontal, tr("Units"));
  grid_model->setHeaderData(6, Qt::Horizontal, tr(""));

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
	  QStandardItem* itemAct = new QStandardItem("");
	  items.push_back(itemAct);
      MessageButton* delButton = new MessageButton(QString::fromStdString(param_iter.first), "Remove");
	  message_buttons.push_back(delButton);
      connect(delButton, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(delParamClicked(const QString&)));

	  grid_model->appendRow(items);
  }

  ui->table_param->setModel(grid_model);

  for (int counter = 0; counter < grid_model->rowCount(); ++counter) {
    auto item_btn = grid_model->index(counter, 6);
    ui->table_param->setIndexWidget(item_btn, message_buttons[counter]);
  }

  ui->le_A->setText("0");
  ui->le_B->setText("0");
  ui->le_C->setText("0");
  ui->le_D->setText("0");
  ui->le_unit->setText("");
}

void DialogSedParam::delParamClicked(const QString& param) {
  auto model = static_cast<QStandardItemModel*>(ui->table_param->model());
  for (int i = 0; i < model->rowCount(); ++i) {
    if (model->item(i, 0)->text() == param) {
      model->removeRows(model->item(i, 0)->row(), 1);
      break;
    }
  }
}

void DialogSedParam::on_btn_new_clicked() {
  if (ui->le_new_name->text() != "") {
    bool ok;
    ui->le_A->text().toDouble(&ok);
    if (ok) {
      ui->le_B->text().toDouble(&ok);
      if (ok) {
    	ui->le_C->text().toDouble(&ok);
    	if (ok) {
    	  ui->le_D->text().toDouble(&ok);
    	  if (ok) {
    	    QList<QStandardItem*> items;
			QStandardItem*        item = new QStandardItem(ui->le_new_name->text());
			items.push_back(item);
			QStandardItem* item2 = new QStandardItem(ui->le_A->text());
			items.push_back(item2);
			QStandardItem* item3 = new QStandardItem(ui->le_B->text());
			items.push_back(item3);
			QStandardItem* item4 = new QStandardItem(ui->le_C->text());
			items.push_back(item4);
			QStandardItem* item5 = new QStandardItem(ui->le_D->text());
			items.push_back(item5);
			QStandardItem* item6 = new QStandardItem(ui->le_unit->text());
			items.push_back(item6);
			QStandardItem* item7 = new QStandardItem("");
			items.push_back(item7);
			MessageButton* delButton = new MessageButton(ui->le_new_name->text(), "Remove");
			connect(delButton, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(delParamClicked(const QString&)));
			auto model = static_cast<QStandardItemModel*>(ui->table_param->model());
			model->appendRow(items);
			ui->table_param->setIndexWidget(model->index(model->rowCount() - 1, 6), delButton);

			ui->le_new_name->setText("");
			ui->le_A->setText("0");
			ui->le_B->setText("0");
			ui->le_C->setText("0");
			ui->le_D->setText("0");
			ui->le_unit->setText("");
    	  } else {
            QMessageBox msgBox;
			msgBox.setText("Unaccepted input.");
			msgBox.setInformativeText("D value (" + ui->le_D->text() + ") must be a number!");
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setDefaultButton(QMessageBox::Ok);
			msgBox.exec();
		  }
    	} else {
		  QMessageBox msgBox;
		  msgBox.setText("Unaccepted input.");
		  msgBox.setInformativeText("C value (" + ui->le_C->text() + ") must be a number!");
		  msgBox.setStandardButtons(QMessageBox::Ok);
		  msgBox.setDefaultButton(QMessageBox::Ok);
		  msgBox.exec();
	    }
      } else {
        QMessageBox msgBox;
        msgBox.setText("Unaccepted input.");
        msgBox.setInformativeText("B value (" + ui->le_B->text() + ") must be a number!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
      }
    } else {
      QMessageBox msgBox;
      msgBox.setText("Unaccepted input.");
      msgBox.setInformativeText("A value (" + ui->le_A->text() + ") must be a number!");
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
    }
  } else {
    QMessageBox msgBox;
    msgBox.setText("Unaccepted input.");
    msgBox.setInformativeText("Parameter name must be set!");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
  }
}

void DialogSedParam::on_btn_save_clicked() {
  std::list<std::string>   keys{};
  std::vector<std::string> params{};

  auto model = static_cast<QStandardItemModel*>(ui->table_param->model());

  for (int i = 0; i < model->rowCount(); ++i) {

    if (model->item(i, 0)->text().toStdString() != "") {
      bool ok;
      model->item(i, 1)->text().toDouble(&ok);
      if (ok) {
        model->item(i, 2)->text().toDouble(&ok);
        if (ok) {
          model->item(i, 3)->text().toDouble(&ok);
          if (ok) {
            model->item(i, 4)->text().toDouble(&ok);
            if (ok) {
          keys.push_back(model->item(i, 0)->text().toStdString());
          params.push_back(model->item(i, 0)->text().toStdString() + "=" +
        		  model->item(i, 1)->text().toStdString() + "*L+" +
				  model->item(i, 2)->text().toStdString() + "+" +
				  model->item(i, 3)->text().toStdString() + "*LOG("+
				  model->item(i, 4)->text().toStdString() + "*L)"+
				  "[" + model->item(i, 5)->text().toStdString() + "]");

            } else {
                      QMessageBox msgBox;
                      msgBox.setText("Unaccepted input.");
                      msgBox.setInformativeText("D value (" + model->item(i, 4)->text() + ") on line " + QString::number(i + 1) +
                                                " must be a number!");
                      msgBox.setStandardButtons(QMessageBox::Ok);
                      msgBox.setDefaultButton(QMessageBox::Ok);
                      msgBox.exec();
                      return;
                    }
          } else {
                    QMessageBox msgBox;
                    msgBox.setText("Unaccepted input.");
                    msgBox.setInformativeText("C value (" + model->item(i, 3)->text() + ") on line " + QString::number(i + 1) +
                                              " must be a number!");
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    msgBox.exec();
                    return;
                  }
        } else {
          QMessageBox msgBox;
          msgBox.setText("Unaccepted input.");
          msgBox.setInformativeText("B value (" + model->item(i, 2)->text() + ") on line " + QString::number(i + 1) +
                                    " must be a number!");
          msgBox.setStandardButtons(QMessageBox::Ok);
          msgBox.setDefaultButton(QMessageBox::Ok);
          msgBox.exec();
          return;
        }
      } else {
        QMessageBox msgBox;
        msgBox.setText("Unaccepted input.");
        msgBox.setInformativeText("A value (" + model->item(i, 1)->text() + ") on line " + QString::number(i + 1) +
                                  " must be a number!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
      }
    } else {
      QMessageBox msgBox;
      msgBox.setText("Unaccepted input.");
      msgBox.setInformativeText("The Name on line " + QString::number(i + 1) + " must be set!");
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
      return;
    }
  }

  keys.unique();
  if (params.size() == keys.size()) {
    QStringList s3;
    s3 << QString::fromStdString("--file") << QString::fromStdString(m_file_path);
    s3 << QString::fromStdString("--remove-key") << QString::fromStdString("PARAMETER");
    for (const auto& param : params) {
      s3 << QString::fromStdString("--add-argument") << QString::fromStdString("\"PARAMETER:" + param + "\"");
    }

    QString cmd = QString("SedHeaderHandler ") + s3.join(" ");

    logger.info() << "Processing cmd:" << cmd.toStdString();

    m_P->setReadChannelMode(QProcess::MergedChannels);
    m_P->start(cmd);

    ui->btn_cancel->setEnabled(false);
    ui->btn_save->setEnabled(false);

  } else {
    QMessageBox msgBox;
    msgBox.setText("Conflicting names.");
    msgBox.setInformativeText("Each parameter must have a different name!");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
  }
}

void DialogSedParam::processingFinished(int, QProcess::ExitStatus status) {
  if (QProcess::NormalExit == status) {
    accept();
  } else {
    QMessageBox msgBox;
    msgBox.setText("Error in the computations.");
    msgBox.setInformativeText("The generation of the Physical Parameter configuration did not succeed. Try to remove "
                              "all the parameters, then re-open the dialog and add them again.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
  }
}

void DialogSedParam::on_btn_cancel_clicked() {
  reject();
}

}  // namespace PhzQtUI
}  // namespace Euclid
