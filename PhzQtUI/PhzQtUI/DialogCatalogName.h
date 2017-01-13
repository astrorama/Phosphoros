#ifndef DIALOGCATALOGNAME_H
#define DIALOGCATALOGNAME_H

#include <memory>
#include <QDialog>
#include <string>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
  class DialogCatalogName;
}

/**
 * @brief The DialogCatalogName class
 * This popup allows the user to copy file/folder into the provided path.
 */
class DialogCatalogName : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCatalogName(QWidget *parent = 0);
    ~DialogCatalogName();

signals:
    /**
     * @brief SIGNAL popupClosing: rised when the dialog is saved and the popup close.
     * The argument is the new name.
     */
    void popupClosing(std::string);

private slots:

    /**
     * @brief SLOT on_btn_create_clicked
     */
    void on_btn_create_clicked();

    void on_btn_cancel_clicked();


private:
    std::unique_ptr<Ui::DialogCatalogName> ui;

};

}
}

#endif // DIALOGCATALOGNAME_H
