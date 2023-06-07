
#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/MessageButton.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QUuid>

#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include "PhzQtUI/DialogInterpolateSed.h"
#include "PhzQtUI/DialogSedSelector.h"
#include "ui_DialogInterpolateSed.h"
#include <QComboBox>
#include <QDirIterator>
#include <QRegExpValidator>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QTextStream>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogInterpolateSed");

DialogInterpolateSed::DialogInterpolateSed(DatasetRepo sed_repo, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogInterpolateSed) {
  ui->setupUi(this);
  m_seds_repository = sed_repo;

  ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->scrollArea->setWidgetResizable(true);

  // ui->layout_SED->addWidget(createControls(true, false,"Toto"));
}

DialogInterpolateSed::~DialogInterpolateSed() {}

void DialogInterpolateSed::on_btn_plus_clicked() {

  std::unique_ptr<DialogSedSelector> dialog(new DialogSedSelector(m_seds_repository, false));
  dialog->setSed({});

  connect(dialog.get(), SIGNAL(popupClosing(std::vector<std::string>)), this,
          SLOT(sedPopupClosing(std::vector<std::string>)));
  dialog->exec();
}

QFrame* DialogInterpolateSed::createControls(bool first, bool del, std::string sed) {
  //	logger.info()<<"Creating controls for "<<sed;
  auto main_frame  = new QFrame();
  auto main_layout = new QVBoxLayout();
  main_frame->setLayout(main_layout);
  main_frame->setObjectName(QUuid::createUuid().toString());
  if (!first) {
    auto number_frame  = new QFrame();
    auto number_layout = new QHBoxLayout();
    number_frame->setLayout(number_layout);
    auto lbl_number = new QLabel("Number of interpolated SED's :");
    number_layout->addWidget(lbl_number);
    auto spin_nbr = new QSpinBox();
    spin_nbr->setMinimum(0);
    spin_nbr->setValue(ui->sb_default_nb->value());
    number_layout->addWidget(spin_nbr);
    number_layout->addSpacerItem(new QSpacerItem(300, 0));

    main_layout->addWidget(number_frame);
  }
  auto sed_frame  = new QFrame();
  auto sed_layout = new QHBoxLayout();
  sed_frame->setLayout(sed_layout);
  auto lbl_sed = new QLabel("SED :");
  sed_layout->addWidget(lbl_sed);

  auto cb_sed = new QComboBox();
  cb_sed->addItem(QString::fromStdString(sed));

  sed_layout->addWidget(cb_sed);

  if (del) {
    auto btn = new MessageButton(main_frame->objectName(), QString::fromStdString("Remove"));
    btn->setObjectName(QString::fromStdString("btn_") + main_frame->objectName());
    connect(btn, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(onDelButtonClicked(const QString&)));
    sed_layout->addWidget(btn);
  }
  // sed_layout->addSpacerItem(new QSpacerItem(300, 0));
  main_layout->addWidget(sed_frame);

  return main_frame;
}

void DialogInterpolateSed::sedPopupClosing(std::vector<std::string> selected_seds) {

  QList<QFrame*> frames = ui->scrollArea->findChildren<QFrame*>();
  bool           first  = frames.count() == 0;
  for (uint i = 0; i < selected_seds.size(); ++i) {
    ui->layout_SED->addWidget(createControls(first, !first, selected_seds[i]));
    first = false;
  }
}

void DialogInterpolateSed::on_btn_rma_clicked() {
  QList<QFrame*> frames = ui->scrollArea->findChildren<QFrame*>();
  for (int i = 0; i < frames.count(); ++i) {
    //  MessageButton* btn = ui->scrollArea->findChild<MessageButton *>(QString::fromStdString("btn_")
    //  +frames[i]->objectName()); disconnect(btn, SIGNAL(MessageButtonClicked(const QString&)), this,
    //  SLOT(onDelButtonClicked(const QString&)));
    ui->layout_SED->removeWidget(frames[i]);
    frames[i]->deleteLater();
  }
}

void DialogInterpolateSed::onDelButtonClicked(const QString& uid) {

  logger.info() << "Delete called for name " << uid.toStdString();
  QFrame* frame = ui->scrollArea->findChild<QFrame*>(uid);

  MessageButton* btn = ui->scrollArea->findChild<MessageButton*>(QString::fromStdString("btn_") + uid);

  disconnect(btn, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(onDelButtonClicked(const QString&)));

  ui->layout_SED->removeWidget(frame);
  frame->deleteLater();
}

void DialogInterpolateSed::on_btn_cancel_clicked() {
  reject();
}

void DialogInterpolateSed::on_btn_create_clicked() {

  auto sed_folder  = QString::fromStdString(FileUtils::getSedRootPath(false));
  auto folder_name = ui->le_folder->text();
  if (folder_name == "") {
    QMessageBox::warning(this, tr("SED Interpolation"),
                         tr("The output folder is missing.\n"
                            "Please provide an output folder"),
                         QMessageBox::Ok, QMessageBox::Ok);
    return;
  }

  if (QDir(sed_folder + "/" + folder_name).exists()) {
    if (QMessageBox::Cancel == QMessageBox::warning(this, tr("SED Interpolation"),
                                                    "The output folder \"" + folder_name + "\" exists.\n" +
                                                        "It will be cleaned before the run!",
                                                    QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)) {
      return;
    }
  }

  QStringList seds{};
  QStringList numbers{};
  int         total      = 0;
  auto        frame_list = ui->scrollArea->findChildren<QFrame*>();

  for (auto frame_iter = frame_list.begin(); frame_iter != frame_list.end(); ++frame_iter) {
    if ((*frame_iter)->objectName() != "") {
      // add the number
      auto* sp = (*frame_iter)->findChild<QSpinBox*>();
      if (sp != nullptr) {
        numbers << QString::number(sp->value());
        total += sp->value();
      } else {
        logger.info() << "No number in Frame " << (*frame_iter)->objectName().toStdString();
      }

      // add the sed
      auto* cb = (*frame_iter)->findChild<QComboBox*>();
      if (cb != nullptr) {
        seds << QString::fromStdString(
            FileUtils::getDataSetFilePath(cb->currentText().toStdString(), FileUtils::getSedRootPath(false)));
      } else {
        logger.warn() << "No SED in Frame " << (*frame_iter)->objectName().toStdString();
      }
    }
  }

  logger.info() << "total new sed =" << total;

  if (total < 2) {
    QMessageBox::warning(
        this, tr("SED Interpolation"),
        tr("Your configuration will not generate new SEDs.\n"
           "Please select at least 2 SEDs and set at least one of the number to a value bigger than 0"),
        QMessageBox::Ok, QMessageBox::Ok);
    return;
  }

  bool copy_seds = ui->cb_cp->isChecked();

  QString     program = "Phosphoros IS";
  QStringList arguments;
  arguments << "--sed-dir" << sed_folder << "--out-dir" << folder_name << "--seds" << seds.join(",") << "--numbers"
            << numbers.join(",");

  if (!copy_seds) {
    arguments << "--copy-sed"
              << "false";
  }

  m_is = new QProcess;

  connect(m_is, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));

  const QString& command = QString("Phosphoros IS ") + arguments.join(" ");
  logger.info("running: " + command.toStdString());
  m_is->start(command);

  ui->btn_cancel->setEnabled(false);
  ui->btn_create->setEnabled(false);
}

void DialogInterpolateSed::processingFinished(int code, QProcess::ExitStatus status) {
  logger.info() << "Run return with code :" << code << " and status " << status;
  if (code + status != 0) {
    QMessageBox::warning(this, tr("SED Interpolation"),
                         tr("An error occure during the computation of the SEDs."
                            "Check that the selected files are all SED files..."),
                         QMessageBox::Ok, QMessageBox::Ok);
    ui->btn_cancel->setEnabled(true);
    ui->btn_create->setEnabled(true);
    return;
  }
  accept();
}

}  // namespace PhzQtUI
}  // namespace Euclid
