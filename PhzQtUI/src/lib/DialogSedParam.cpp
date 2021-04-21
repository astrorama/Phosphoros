
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <fstream>
#include <list>
#include <vector>
#include <QStandardItemModel>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include "ElementsKernel/Logging.h"
#include "PhzQtUI/DialogSedParam.h"
#include "ui_DialogSedParam.h"
#include "FileUtils.h"
#include "PhzQtUI/MessageButton.h"


using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogSedParam");


DialogSedParam::DialogSedParam(DatasetRepo sed_repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSedParam) {
  ui->setupUi(this);
  m_sed_repository = sed_repository;

  m_P = new QProcess;

  connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));
}

DialogSedParam::~DialogSedParam() {}


std::string getParameter(const std::string& file, const std::string& key_word) {
  std::ifstream sfile(file);
  if (!sfile) {
    throw Elements::Exception() << "File does not exist : " << file;
  }

  std::string  value{};
  std::string  line{};
  std::string  dataset_name{};
  std::string  reg_ex_str = "^\\s*#\\s*" + key_word + "\\s*:\\s*(.+)\\s*$";
  boost::regex expression(reg_ex_str);

  while (sfile.good()) {
    std::getline(sfile, line);
    boost::smatch s_match;
    if (!line.empty() && boost::regex_match(line, s_match, expression)) {
      if (value != "") {
         value +=";";
      }
      std::string new_val = s_match[1].str();
      boost::trim(new_val);
      value += new_val;

    }
  }
  return value;
}

std::string getName(const std::string& file) {
  // The data set name can be a parameter with keyword NAME
  std::string dataset_name = getParameter(file, "NAME");

  if (dataset_name == "") {
    // IF not present chack the first non-empty line (Backward comatibility)
    std::ifstream sfile(file);
    std::string line{};
    // Check dataset name is in the file
    // Convention: read until found first non empty line, removing empty lines.
    while (line.empty() && sfile.good()) {
      std::getline(sfile, line);
    }

    boost::regex  expression("^\\s*#\\s*(\\w+)\\s*$");
    boost::smatch s_match;
    if (boost::regex_match(line, s_match, expression)) {
      dataset_name = s_match[1].str();
    } else {
      dataset_name = "";
    }
  }

  return dataset_name;
}




std::string DialogSedParam::getFile(const XYDataset::QualifiedName& sed) const {
  std::string root_path = FileUtils::getSedRootPath(false) + "/";

  for (std::string group : sed.groups()) {
    root_path = root_path + group + "/";
  }

  QDir directory(QString::fromStdString(root_path));
  QStringList seds_in_group = directory.entryList(QStringList() << "*.*", QDir::Files);

  for (QString filename : seds_in_group) {
     if (QFileInfo(filename).baseName().toStdString() == sed.datasetName()) {
        return root_path + filename.toStdString();
     }
  }

  // name in the file
  for (QString filename : seds_in_group) {
      std::string name = getName(root_path + filename.toStdString());
      if (name == sed.datasetName()) {
         return root_path + filename.toStdString();
      }
   }

  return "";
}



void DialogSedParam::setSed(const XYDataset::QualifiedName&  sed) {
  m_file_path = getFile(sed);

  logger.info() << m_file_path;

  ui->lbl_sed_name->setText(QString::fromStdString(sed.qualifiedName()));
  std::string keyword = "PARAMETER";
  auto string_params = QString::fromStdString(m_sed_repository->getProvider()->getParameter(sed, keyword));



  QStandardItemModel* grid_model = new QStandardItemModel();
   grid_model->setColumnCount(4);
   grid_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
   grid_model->setHeaderData(1, Qt::Horizontal, tr("A"));
   grid_model->setHeaderData(2, Qt::Horizontal, tr("B"));
   grid_model->setHeaderData(3, Qt::Horizontal, tr(""));

   auto params = string_params.split(";");

   std::vector<MessageButton*> message_buttons;
   for (auto param : params) {
     if (param.contains("=")) {
         QList<QStandardItem*> items;
         auto bits = param.split("=");
         QStandardItem* item = new QStandardItem(bits[0]);
         items.push_back(item);
         if (bits[1].contains("+")) {
           auto sub_bits = bits[1].split("+");
           if  (sub_bits[0].contains("*L")) {
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
         } else if (bits[1].contains("*L")) {
            QStandardItem* item2 = new QStandardItem(bits[1].replace("*L", ""));
            items.push_back(item2);
            QStandardItem* item3 = new QStandardItem(QString::number(0));
            items.push_back(item3);
         } else {
             QStandardItem* item2 = new QStandardItem(QString::number(0));
             items.push_back(item2);
             QStandardItem* item3 = new QStandardItem(bits[1]);
             items.push_back(item3);
         }

         QStandardItem* item4 = new QStandardItem("");
         items.push_back(item4);

         MessageButton *delButton = new MessageButton(bits[0], "Remove");
         message_buttons.push_back(delButton);

         connect(delButton, SIGNAL(MessageButtonClicked(const QString&)), this,
                                     SLOT(delParamClicked(const QString&)));

         grid_model->appendRow(items);
     }

   }

   ui->table_param->setModel(grid_model);

   for (int counter =0; counter < grid_model->rowCount(); ++counter) {
     auto item_btn = grid_model->index(counter, 3);
     ui->table_param->setIndexWidget(item_btn, message_buttons[counter]);
   }


   ui->le_A->setText("0");
   ui->le_B->setText("0");

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
          QStandardItem* item = new QStandardItem(ui->le_new_name->text());
          items.push_back(item);
          QStandardItem* item2 = new QStandardItem(ui->le_A->text());
          items.push_back(item2);
          QStandardItem* item3 = new QStandardItem(ui->le_B->text());
          items.push_back(item3);
          QStandardItem* item4 = new QStandardItem("");
          items.push_back(item4);
          MessageButton *delButton = new MessageButton(ui->le_new_name->text(), "Remove");
          connect(delButton, SIGNAL(MessageButtonClicked(const QString&)), this,
                                               SLOT(delParamClicked(const QString&)));
          auto model = static_cast<QStandardItemModel*>(ui->table_param->model());
          model->appendRow(items);
          ui->table_param->setIndexWidget(model->index(model->rowCount()-1, 3), delButton);

          ui->le_new_name->setText("");
          ui->le_A->setText("0");
          ui->le_B->setText("0");
        } else {
          QMessageBox msgBox;
          msgBox.setText("Unaccepted input.");
          msgBox.setInformativeText("B value ("+ ui->le_B->text() +") must be a number!");
          msgBox.setStandardButtons(QMessageBox::Ok);
          msgBox.setDefaultButton(QMessageBox::Ok);
          msgBox.exec();

        }
    } else {
       QMessageBox msgBox;
       msgBox.setText("Unaccepted input.");
       msgBox.setInformativeText("A value ("+ ui->le_A->text() +") must be a number!");
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
  std::list<std::string> keys {};
  std::vector<std::string> params {};

  auto model = static_cast<QStandardItemModel*>(ui->table_param->model());

  for (int i = 0; i < model->rowCount(); ++i) {

     if (model->item(i, 0)->text().toStdString() != "") {
        bool ok;
        model->item(i, 1)->text().toDouble(&ok);
        if (ok) {
          model->item(i, 2)->text().toDouble(&ok);
            if (ok) {

              keys.push_back(model->item(i, 0)->text().toStdString());
              params.push_back(model->item(i, 0)->text().toStdString() +
                  "=" + model->item(i, 1)->text().toStdString() +
                  "*L+"+model->item(i, 2)->text().toStdString());

            } else {
              QMessageBox msgBox;
              msgBox.setText("Unaccepted input.");
              msgBox.setInformativeText("B value (" + model->item(i, 2)->text() +
                  ") on line " + QString::number(i+1) + " must be a number!");
              msgBox.setStandardButtons(QMessageBox::Ok);
              msgBox.setDefaultButton(QMessageBox::Ok);
              msgBox.exec();
              return;
            }
        } else {
          QMessageBox msgBox;
          msgBox.setText("Unaccepted input.");
          msgBox.setInformativeText("A value ("+ model->item(i, 1)->text() +") on line "+QString::number(i+1)+" must be a number!");
          msgBox.setStandardButtons(QMessageBox::Ok);
          msgBox.setDefaultButton(QMessageBox::Ok);
          msgBox.exec();
          return;
        }
     } else {
        QMessageBox msgBox;
        msgBox.setText("Unaccepted input.");
        msgBox.setInformativeText("The Name on line "+QString::number(i+1)+" must be set!");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
     }

       }

  keys.unique();
  if (params.size() == keys.size()) {
    QStringList s3;
    s3 << QString::fromStdString("--file") <<  QString::fromStdString(m_file_path);
    s3 << QString::fromStdString("--remove-key") <<  QString::fromStdString("PARAMETER");
    for (const auto& param : params) {
      s3 << QString::fromStdString("--add-argument") << QString::fromStdString("\"PARAMETER:" + param + "\"");
    }

    QString cmd =  QString("SedHeaderHandler ")+s3.join(" ");

    logger.info()<< "Processing cmd:" << cmd.toStdString();

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

void DialogSedParam::processingFinished(int, QProcess::ExitStatus) {
   accept();
}

void DialogSedParam::on_btn_cancel_clicked() {
  reject();
}

}
}
