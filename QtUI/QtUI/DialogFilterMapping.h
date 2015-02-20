#ifndef DIALOGFILTERMAPPING_H
#define DIALOGFILTERMAPPING_H

#include <QDialog>
#include "FilterMapping.h"

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
     * the FilterMapping to be modified.
     * @param columns
     * A (possibly empty) list of columns to be used to prepopulate the column drop downs.
     */
    void setFilter(const PhosphorosUiDm::FilterMapping& filter, const std::list<std::string>& columns);

signals:
    /**
     * @brief SIGNAL popupClosing: rised when the FilterMapping is saved and the popup close.
     * The argument is the modifed FilterMapping.
     */
    void popupClosing(PhosphorosUiDm::FilterMapping);


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
    Ui::DialogFilterMapping *ui;
};

#endif // DIALOGFILTERMAPPING_H
