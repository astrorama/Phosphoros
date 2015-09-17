#ifndef DialogFilterSelector_H
#define DialogFilterSelector_H

#include <set>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogFilterSelector;
}

/**
 * @brief The DialogFilterSelector class
 * This popup allows the user to select a filter.
 */
class DialogFilterSelector : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFilterSelector(QWidget *parent = 0);
    ~DialogFilterSelector();

    void setFilter(std::string filter_name);

signals:
    /**
     * @brief SIGNAL popupClosing: rised when the popup is closed.
     * The argument is the Filter Name.
     */
    void popupClosing(std::string);


private slots:
    /**
     * @brief SLOT on_btn_cancel_clicked: close the popup
     */
    void on_btn_cancel_clicked();

    /**
     * @brief SLOT on_btn_save_clicked: rises the
     *  SIGNAL popupClosing and close the popup.
     */
    void on_btn_save_clicked();

private:
   std::unique_ptr<Ui::DialogFilterSelector> ui;
};

}
}

#endif // DialogFilterSelector_H
