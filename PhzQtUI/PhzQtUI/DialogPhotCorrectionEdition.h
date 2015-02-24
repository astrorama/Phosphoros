#ifndef DIALOGPHOTCORRECTIONEDITION_H
#define DIALOGPHOTCORRECTIONEDITION_H

#include <QDialog>
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Ui {
class DialogPhotCorrectionEdition;
}

class DialogPhotCorrectionEdition : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPhotCorrectionEdition(QWidget *parent = 0);
    ~DialogPhotCorrectionEdition();
    void setCorrectionsFile(std::string filePath);

private slots:
    void on_btn_Edit_clicked();

    void on_btn_Cancel_clicked();

    void on_btn_Save_clicked();

private:
    Ui::DialogPhotCorrectionEdition *ui;
    std::string m_file_path;
    Euclid::PhzDataModel::PhotometricCorrectionMap m_map;
};

#endif // DIALOGPHOTCORRECTIONEDITION_H
