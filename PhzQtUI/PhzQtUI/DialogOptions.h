#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H

#include <memory>
#include <QDialog>

namespace Euclid {
namespace PhzQtUI {



namespace Ui {
class DialogOptions;
}

/**
 * @brief The DialogOptions class
 * This popup allows the user to Navigate through the options
 * and change the root path.
 */
class DialogOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOptions(QWidget *parent = 0);
    ~DialogOptions();

    void EditRootPath();


private slots:

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
     * @brief SLOT on_btn_browseRoot_clicked: Open the native Folder selector dialog.
     */
    void on_btn_browseRoot_clicked();

    /**
     * @brief SLOT Lock the user to the current tab.
     */
    void startEdition(int i);

    /**
     * @brief SLOT re-activate tabs and popup closing.
     */
    void endEdition();

private:
    std::unique_ptr<Ui::DialogOptions> ui;
    bool m_is_root_edition;
};

}
}

#endif // DIALOGOPTIONS_H
