#ifndef DIALOGCATALOGNAME_H
#define DIALOGCATALOGNAME_H

#include <memory>
#include <QDialog>
#include <string>
#include <list>

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

    void setDefaultName(std::string default_name);
    void setExistingNames( std::list<std::string> existing_names);
    std::string getName() const;

private slots:

    /**
     * @brief SLOT on_btn_create_clicked
     */
    void on_btn_create_clicked();

    /**
     * @brief SLOT on_btn_cancel_clicked
     */
    void on_btn_cancel_clicked();


private:
    std::unique_ptr<Ui::DialogCatalogName> ui;
    std::string m_name;
    std::list<std::string> m_existing_names;

};

}
}

#endif // DIALOGCATALOGNAME_H
