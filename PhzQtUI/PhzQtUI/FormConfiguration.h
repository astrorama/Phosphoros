#ifndef FORMCONFIGURATION_H
#define FORMCONFIGURATION_H

#include <memory>
#include <QWidget>
#include <QModelIndex>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"


namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class FormConfiguration;
}


typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

/**
 * @class FormConfiguration
 * @brief This widget allows the user to manage configuration options.
 */
class FormConfiguration : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit FormConfiguration(QWidget *parent = 0);

    /**
     * @brief Destructor
     */
    ~FormConfiguration();

    /**
     * @brief give the references to the repositories to be updated when
     * change are performed.
     */
     void loadOptionPage(DatasetRepo seds_repository,
         DatasetRepo redenig_curves_repository);

signals:
/**
 * @brief SIGNAL Called when the user want to go back to the home page.
 */


    void navigateToParameter(bool);

    void navigateToCatalog(bool);

    void navigateToComputeRedshift(bool);

    void quit(bool);

private slots:

void on_btn_ToAnalysis_clicked();
void on_btn_ToCatalog_clicked();
void on_btn_ToModel_clicked();
void on_btn_exit_clicked();

/**
 * @brief SLOT on_btn_editGeneral_clicked: turn the root path section in edition.
 */
void on_btn_editGeneral_clicked();

/**
 * @brief SLOT on_btn_cancelGeneral_clicked: cancel the root path section edition.
 */
void on_btn_cancelGeneral_clicked();

/**
 * @brief SLOT on_btn_saveGeneral_clicked: ends the edition and save the root path into the
 * resources file.
 */
void on_btn_saveGeneral_clicked();

/**
 * @brief SLOT Raised when the user click on the Browse Catalog button
 */
void on_btn_browseCat_clicked();

/**
 * @brief SLOT Raised when the user click on the Browse Aux Data button
 */
void on_btn_browseAux_clicked();

/**
 * @brief SLOT Raised when the user click on the Browse Intermediate product button
 */
void on_btn_browseInter_clicked();

/**
 * @brief SLOT Raised when the user click on the Browse Result button
 */
void on_btn_browseRes_clicked();

/**
 * @brief SLOT Raised when the user click the Default Catalog button
 */
void on_btn_defCat_clicked();

/**
 * @brief SLOT Raised when the user click the Default Aux Data button
 */
void on_btn_defAux_clicked();

/**
 * @brief SLOT Raised when the user click the Default Intermediate button
 */
void on_btn_defInter_clicked();

/**
 * @brief SLOT Raised when the user click the Default Result button
 */
void on_btn_defRes_clicked();

/**
 * @brief SLOT Raised when the user start Cosmology edition
 */
void on_btn_edit_cosmo_clicked();

/**
 * @brief SLOT Raised when the user cancel the cosmology edition
 */
void on_btn_cancel_cosmo_clicked();

/**
 * @brief SLOT Raised when the user  save the cosmology edition
 */
void on_btn_save_cosmo_clicked();

/**
 * @brief SLOT Raised when the user click the default cosmology button
 */
void on_btn_default_cosmo_clicked();

/**
 * @brief SLOT Lock the user to the current tab.
 */
void startEdition(int i);

/**
 * @brief SLOT re-activate tabs and popup closing.
 */
void endEdition();

private:
    std::unique_ptr<Ui::FormConfiguration> ui;

    bool do_need_reset = false;
    void checkDirectories();
    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;
};

}
}

#endif // FORMCONFIGURATION_H
