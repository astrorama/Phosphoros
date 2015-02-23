#ifndef DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
#define DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H

#include <QDialog>
#include <map>

namespace Ui {
class DialogPhotometricCorrectionComputation;
}

class DialogPhotometricCorrectionComputation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPhotometricCorrectionComputation(QWidget *parent = 0);
    ~DialogPhotometricCorrectionComputation();
    void setData(std::string survey, std::string model, std::string grid, std::map<std::string,bool> selectedFilters );

private slots:
    void on_btn_TrainingCatalogFolder_clicked();

private:
    Ui::DialogPhotometricCorrectionComputation *ui;
};

#endif // DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
