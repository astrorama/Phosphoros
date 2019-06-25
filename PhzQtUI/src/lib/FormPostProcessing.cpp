#include <QMessageBox>
#include "PhzQtUI/FormPostProcessing.h"

#include "ui_FormPostProcessing.h"
#include "FileUtils.h"

#include "ElementsKernel/Exception.h"
#include "PreferencesUtils.h"
#include "ElementsKernel/Logging.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormPostProcessing");

FormPostProcessing::FormPostProcessing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPostProcessing) {
    ui->setupUi(this);
}

FormPostProcessing::~FormPostProcessing() {}


//  - Slots on this page
void FormPostProcessing::on_btn_ToAnalysis_clicked() {
  navigateToComputeRedshift(false);
}
void FormPostProcessing::on_btn_ToOption_clicked() {
  navigateToConfig();
}
void FormPostProcessing::on_btn_ToCatalog_clicked() {
  navigateToCatalog(false);
}

void FormPostProcessing::on_btn_ToModel_clicked(){
  navigateToParameter(false);
}

void FormPostProcessing::on_btn_exit_clicked() {
  quit(true);
}

}
}
