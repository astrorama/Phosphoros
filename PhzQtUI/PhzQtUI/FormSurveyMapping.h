#ifndef FORMSURVEYMAPPING_H
#define FORMSURVEYMAPPING_H

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <QWidget>
#include <QModelIndex>
#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzQtUI/SurveyModel.h"

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class FormSurveyMapping;
}

/**
 * @brief The FormSurveyMapping class
 * This widget allows the user to create and manage Survey Filter Mappings.
 */
class FormSurveyMapping : public QWidget
{
    Q_OBJECT

public:
    explicit FormSurveyMapping( QWidget *parent = 0);
    ~FormSurveyMapping();

    void loadMappingPage(std::shared_ptr<SurveyModel> survey_model_ptr, DatasetRepo filter_repository, std::string new_path);

    void updateSelection();

signals:

void navigateToParameter(bool);

void navigateToConfig();

void navigateToComputeRedshift(bool);

void quit(bool);


private slots:

void on_btn_ToAnalysis_clicked();
void on_btn_ToOption_clicked();
void on_btn_ToModel_clicked();
void on_btn_exit_clicked();


void on_cb_missingPhot_stateChanged(int state);
void on_cb_upperLimit_stateChanged(int state);


    void filterMappingSelectionChanged(QModelIndex, QModelIndex);


    void mappingGridDoubleClicked(QModelIndex);

    void filterEditionPopupClosing(std::vector<std::string>);

    void on_btn_MapNew_clicked();

    void on_btn_MapDuplicate_clicked();

    void on_btn_map_delete_clicked();

    void on_btn_MapEdit_clicked();

    void on_btn_MapCancel_clicked();

    void on_btn_MapSave_clicked();

    void on_btn_ImportColumn_clicked();

    void on_btn_SelectFilters_clicked();

    void on_cb_SourceId_currentIndexChanged(int index);

    void on_cb_Dec_currentIndexChanged(int index);

    void on_cb_Ra_currentIndexChanged(int index);

    void on_cb_GalEbv_currentIndexChanged(int index);

    void on_txt_nonDetection_textEdited(const QString& text);




private:
    std::unique_ptr<Ui::FormSurveyMapping> ui;
    DatasetRepo m_filter_repository;
    bool m_mappingInsert;
    bool m_filterInsert;
    std::set<std::string> m_column_from_file;
    std::string m_default_survey;
    std::shared_ptr<SurveyModel> m_survey_model_ptr;

    void fillControlsWithSelected();
    void selectFromGrid();

    void setFilterMappingInEdition();
    void setFilterMappingInView();
    void loadColumnFromFile(std::string path);
    void fillCbColumns(std::string current_id_value="",std::string current_ra_value="",std::string current_dec_value="",std::string current_gebv_value="");

    std::vector<std::string> getGridFiltersNames() const;
    std::vector<FilterMapping> getMappingFromGrid() const;


};

}
}

#endif // FORMSURVEYMAPPING_H
