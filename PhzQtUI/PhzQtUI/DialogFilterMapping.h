#ifndef DIALOGFILTERMAPPING_H
#define DIALOGFILTERMAPPING_H

#include <set>
#include <string>
#include <vector>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogFilterMapping;
}

/**
 * @brief The DialogFilterMapping class
 * This popup allows the user to edit the provided FilterMapping.
 */
class DialogFilterMapping : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFilterMapping(QWidget *parent = 0);
    ~DialogFilterMapping();

    /**
     * @brief Initialise the popup by setting its internal data
     * @param filter
     * the filter list to be modified.
     */
    void setFilters(const std::vector<std::string>& filters);

signals:
    /**
     * @brief SIGNAL popupClosing: rised when the FilterMapping is saved and the popup close.
     * The argument is the modifed filter list.
     */
    void popupClosing( std::vector<std::string>);


private slots:
    /**
     * @brief SLOT on_btn_cancel_clicked: close the popup
     */
    void on_btn_cancel_clicked();

    /**
     * @brief SLOT on_btn_save_clicked: build the updated FilterMapping and pass it to the
     *  SIGNAL popupClosing and close the popup.
     */
    void on_btn_save_clicked();

private:
   std::unique_ptr<Ui::DialogFilterMapping> ui;
};

}
}

#endif // DIALOGFILTERMAPPING_H
