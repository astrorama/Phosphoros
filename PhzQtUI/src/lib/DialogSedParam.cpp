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

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogSedParam");

DialogSedParam::DialogSedParam(DatasetRepo sed_repository, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogSedParam) {
  ui->setupUi(this);
  m_sed_repository = sed_repository;

  m_P = new QProcess;
  m_P->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

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
  grid_model->setColumnCount(4);
  grid_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
  grid_model->setHeaderData(1, Qt::Horizontal, tr("A"));
  grid_model->setHeaderData(2, Qt::Horizontal, tr("B"));
  grid_model->setHeaderData(2, Qt::Horizontal, tr("Units"));
  grid_model->setHeaderData(3, Qt::Horizontal, tr(""));

  auto params = string_params.split(";");

  std::vector<MessageButton*> message_buttons;
  for (auto param : params) {
    if (param.contains("=")) {
      QList<QStandardItem*> items;
      auto                  bits = param.split("=");
      QStandardItem*        item = new QStandardItem(bits[0]);
      items.push_back(item);
      auto param_bits = bits[1].split("[");

      if (param_bits[0].contains("+")) {
        auto sub_bits = param_bits[0].split("+");
        if (sub_bits[0].contains("*L")) {
          QStandardItem* item2 = new QStandardItem(sub_bits[0].replace("*L", ""));
          items.push_back(item2);
          QStandardItem* item3 = new QStandardItem(sub_bits[1]);
          items.push_back(item3);
        } else {
          QStandardItem* item2 = new QStandardItem(sub_bits[1].replace("*L", ""));
          items.push_back(item2);
          QStandardItem* item3 = new QStandardItem(sub_bits[0]);
          items.push_back(item3);
        }
      } else if (param_bits[0].contains("*L")) {
        QStandardItem* item2 = new QStandardItem(param_bits[0].replace("*L", ""));
        items.push_back(item2);
        QStandardItem* item3 = new QStandardItem(QString::number(0));
        items.push_back(item3);
      } else {
        QStandardItem* item2 = new QStandardItem(QString::number(0));
        items.push_back(item2);
        QStandardItem* item3 = new QStandardItem(param_bits[0]);
        items.push_back(item3);
      }

      if (param_bits.size() > 0) {
        auto           unit_bit = param_bits[1].split("]");
        QStandardItem* item4    = new QStandardItem(unit_bit[0]);
        items.push_back(item4);
      } else {
        QStandardItem* item4 = new QStandardItem("");
        items.push_back(item4);
      }

      QStandardItem* item5 = new QStandardItem("");
      items.push_back(item5);

      MessageButton* delButton = new MessageButton(bits[0], "Remove");
      message_buttons.push_back(delButton);

      connect(delButton, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(delParamClicked(const QString&)));

      grid_model->appendRow(items);
    }
  }

  ui->table_param->setModel(grid_model);

  for (int counter = 0; counter < grid_model->rowCount(); ++counter) {
    auto item_btn = grid_model->index(counter, 4);
    ui->table_param->setIndexWidget(item_btn, message_buttons[counter]);
  }

  ui->le_A->setText("0");
  ui->le_B->setText("0");
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
        QList<QStandardItem*> items;
        QStandardItem*        item = new QStandardItem(ui->le_new_name->text());
        items.push_back(item);
        QStandardItem* item2 = new QStandardItem(ui->le_A->text());
        items.push_back(item2);
        QStandardItem* item3 = new QStandardItem(ui->le_B->text());
        items.push_back(item3);
        QStandardItem* item4 = new QStandardItem(ui->le_unit->text());
        items.push_back(item4);
        QStandardItem* item5 = new QStandardItem("");
        items.push_back(item5);
        MessageButton* delButton = new MessageButton(ui->le_new_name->text(), "Remove");
        connect(delButton, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(delParamClicked(const QString&)));
        auto model = static_cast<QStandardItemModel*>(ui->table_param->model());
        model->appendRow(items);
        ui->table_param->setIndexWidget(model->index(model->rowCount() - 1, 4), delButton);

        ui->le_new_name->setText("");
        ui->le_A->setText("0");
        ui->le_B->setText("0");
        ui->le_unit->setText("");
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
    QStringList arguments;
    arguments << QString::fromStdString("--file") << QString::fromStdString(m_file_path);
    arguments << QString::fromStdString("--remove-key") << QString::fromStdString("PARAMETER");
    for (const auto& param : params) {
    	arguments << QString::fromStdString("--add-argument") << QString::fromStdString("\"PARAMETER:" + param + "\"");
    }

    QString cmd = QString("SedHeaderHandler");

    logger.info() << "Processing cmd:" << "SedHeaderHandler " << arguments.join(" ").toStdString();

    m_P->setProcessChannelMode(QProcess::MergedChannels);
    m_P->start(cmd, arguments);

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
