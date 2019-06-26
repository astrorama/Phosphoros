#ifndef FORMPOSTPROCESSING_H
#define FORMPOSTPROCESSING_H

#include <memory>
#include <QWidget>
#include <QModelIndex>
#include "ParameterRule.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"


namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class FormPostProcessing;
}



/**
 * @brief The FormPostProcessing class
 */
class FormPostProcessing : public QWidget {
    Q_OBJECT

public:
    explicit FormPostProcessing(QWidget *parent = 0);
    ~FormPostProcessing();




signals:

void navigateToCatalog(bool);

void navigateToConfig();

void navigateToComputeRedshift(bool);

void navigateToParameter(bool);

void quit(bool);



public slots:
void on_btn_refresh_clicked();

private slots:

void on_btn_ToAnalysis_clicked();
void on_btn_ToOption_clicked();
void on_btn_ToCatalog_clicked();
void on_btn_ToModel_clicked();
void on_btn_exit_clicked();

void catalogSelectionChanged(QModelIndex, QModelIndex);
void computePdfStat(int);
void plotZVsZref(int);


private:
    std::unique_ptr<Ui::FormPostProcessing> ui;
};

}
}

#endif // FORMPOSTPROCESSING_H
