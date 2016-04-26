#ifndef DIALOGMODELSET_H
#define DIALOGMODELSET_H

#include <memory>
#include <map>
#include <QDialog>
#include <QItemSelection>
#include <QVBoxLayout>

#include "ParameterRule.h"
#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogModelSet;
}

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

/**
 * @brief The DialogModelSet class
 * This popup allows the user to view/edit the Parameter Rules of a Model Set.
 */
class DialogModelSet : public QDialog
{
    Q_OBJECT

public:
    explicit DialogModelSet(DatasetRepo seds_repository,
        DatasetRepo redenig_curves_repository, QWidget *parent = 0);
    ~DialogModelSet();

    /**
     * @brief Initialise the popup by setting its internal data
     * @param init_parameter_rules
     * the map of ParameterRules to be displayed/modified
     */
    void loadData(int ref, const std::map<int,ParameterRule>&);

    /**
     * @brief When called, set the popup in read-only.
     */
    void setViewMode();


signals:
    /**
      * @brief SIGNAL popupClosing: rised when the the popup close.
      * The argument is the modifed  ParameterRule.
      */
     void popupClosing(int,ParameterRule,bool);

private slots:


     /**
      * @brief SLOT on_buttonBox_rejected: The user close the popup, build the new ParameterRules map
      * and rise the SIGNAL popupClosing
      */
     void on_buttonBox_rejected();

     void on_buttonBox_accepted();



     void onZDeleteClicked(size_t,size_t );

     void on_btn_add_z_range_clicked();

     void onEbvDeleteClicked(size_t,size_t );

     void on_btn_add_ebv_range_clicked();

private:
    std::unique_ptr<Ui::DialogModelSet> ui;
    bool m_view_popup=false;

    int m_current_z_range_id=0;
    int m_current_ebv_range_id=0;

    void turnControlsInEdition();
    void turnControlsInView();
    void populateZRangesAndValues(ParameterRule selected_rule);
    void populateEbvRangesAndValues(ParameterRule selected_rule);

    QFrame* createRangeControls(GridButton* del_button, int range_id, bool enabled );
    QFrame* createRangeControls(GridButton* del_button, int range_id, bool enabled, const Range& range);

    void cleanRangeControl(QVBoxLayout* ranges_layout);
    void SetRangeControlsEnabled(QVBoxLayout* ranges_layout, bool is_enabled);
    std::vector<Range> getRanges(QVBoxLayout* ranges_layout);

    void deleteRangeAt(QVBoxLayout* ranges_layout, size_t range_id);


    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;

    int m_ref;
    std::map<int,ParameterRule> m_rules;
};

}
}

#endif // DIALOGMODELSET_H
