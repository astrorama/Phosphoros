/*
 * DialogZRanges.h
 *
 *  Created on: September 4, 2015
 *      Author: fdubath
 */

#ifndef DialogZRanges_H_
#define DialogZRanges_H_

#include <vector>
#include <memory>
#include <QDialog>
#include <QVBoxLayout>
#include <map>
#include "ElementsKernel/Exception.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogZRanges;
}

/**
 * @brief The DialogZRanges class.
 */
class DialogZRanges : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit DialogZRanges(QWidget *parent = 0);

    /**
     * @brief Destructor
     */
    ~DialogZRanges();

    void setRanges(std::vector<double> zs);
    void setMinMax(double z_min, double z_max);

signals:
  void popupClosing(std::vector<double> groups);


private slots:
void on_btn_cancel_clicked();
void on_btn_save_clicked();

void on_btn_add_clicked();
void onDeleteClicked(size_t index,size_t );



private:

void refresh();
std::unique_ptr<Ui::DialogZRanges> ui;
std::vector<double> m_zs;

};

}
}



#endif /* DialogZRanges_H_*/
