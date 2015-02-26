#ifndef DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
#define DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H

#include <QDialog>
#include <list>
#include "PhzQtUI/FilterMapping.h"

namespace Ui {
class DialogPhotometricCorrectionComputation;
}

class DialogPhotometricCorrectionComputation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPhotometricCorrectionComputation(QWidget *parent = 0);
    ~DialogPhotometricCorrectionComputation();
    void setData(std::string survey, std::string id_column, std::string model,
        std::string grid,
        std::list<Euclid::PhosphorosUiDm::FilterMapping> selected_filters );
signals:
void correctionComputed(const std::string &);

private slots:

    void on_btn_TrainingCatalog_clicked();

    void on_cb_SpectroColumn_currentIndexChanged(const QString &arg1);

    void on_txt_Iteration_textChanged(const QString &);

    void on_txt_Tolerence_textChanged(const QString &);

    void on_btn_OutputFileName_clicked();

    void on_bt_Run_clicked();

    void on_txt_FileName_textChanged(const QString &);

private:
    Ui::DialogPhotometricCorrectionComputation *ui;
    std::list<Euclid::PhosphorosUiDm::FilterMapping> m_selected_filters;
    std::string m_id_column;
    void setRunEnability();
};

#endif // DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
