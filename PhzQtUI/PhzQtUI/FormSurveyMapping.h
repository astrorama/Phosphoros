#ifndef FORMSURVEYMAPPING_H
#define FORMSURVEYMAPPING_H

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <QWidget>
#include <QModelIndex>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

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
    explicit FormSurveyMapping(QWidget *parent = 0);
    ~FormSurveyMapping();

    void loadMappingPage();
    void loadMappingPage(std::string new_path);

    void updateSelection();

signals:

void navigateToParameter(bool);

void navigateToConfig();

void navigateToComputeRedshift(bool);

void quit(bool);


private slots:

void on_gridEditionStart();

void on_btn_ToAnalysis_clicked();
void on_btn_ToOption_clicked();
void on_btn_ToModel_clicked();
void on_btn_exit_clicked();


void on_cb_missingPhot_stateChanged(int state);


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



private:
    std::unique_ptr<Ui::FormSurveyMapping> ui;
    bool m_mappingInsert;
    bool m_filterInsert;
    std::set<std::string> m_column_from_file;
    std::string m_default_survey;

    void setFilterMappingInEdition();
    void setFilterMappingInView();
    void loadColumnFromFile(std::string path);
    void fillCbColumns(std::string current_value="");

    std::vector<std::string> getGridFiltersNames() const;
    std::vector<FilterMapping> getMappingFromGrid() const;

};

}
}

#endif // FORMSURVEYMAPPING_H
