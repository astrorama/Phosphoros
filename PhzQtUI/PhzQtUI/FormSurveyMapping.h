#ifndef FORMSURVEYMAPPING_H
#define FORMSURVEYMAPPING_H

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

signals:
    void navigateToHome();
    void navigateToFilterManagement();

private slots:
    void filterMappingSelectionChanged(QModelIndex, QModelIndex);

    void filterSelectionChanged(QModelIndex, QModelIndex);

    void filterEditionPopupClosing(FilterMapping);

    void on_btn_MapToHome_clicked();

    void on_btn_MapNew_clicked();

    void on_btn_MapDuplicate_clicked();

    void on_btn_MapDelete_clicked();

    void on_btn_MapEdit_clicked();

    void on_btn_MapCancel_clicked();

    void on_btn_MapSave_clicked();

    void on_btn_ImportColumn_clicked();

    void on_btn_AddFilter_clicked();

    void on_btn_BtnEditFilter_clicked();

    void on_btn_DeleteFilter_clicked();

    void on_btn_mappingToFilter_clicked();

private:
    Ui::FormSurveyMapping *ui;
    bool m_mappingInsert;
    bool m_filterInsert;
    std::list<std::string> m_column_from_file;

    void setFilterMappingInEdition();
    void setFilterMappingInView();
    void loadColumnFromFile(std::string path);
};

}
}

#endif // FORMSURVEYMAPPING_H
