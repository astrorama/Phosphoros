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


    void on_btn_browseCat_clicked();
    void on_btn_browseAux_clicked();
    void on_btn_browseInter_clicked();
    void on_btn_browseRes_clicked();

    void on_btn_defCat_clicked();
    void on_btn_defAux_clicked();
    void on_btn_defInter_clicked();
    void on_btn_defRes_clicked();




    /**
     * @brief SLOT Lock the user to the current tab.
     */
    void startEdition(int i);

    /**
     * @brief SLOT re-activate tabs and popup closing.
     */
    void endEdition();



private:
    void checkDirectories();
    std::unique_ptr<Ui::DialogOptions> ui;

};

}
}

#endif // DIALOGOPTIONS_H
