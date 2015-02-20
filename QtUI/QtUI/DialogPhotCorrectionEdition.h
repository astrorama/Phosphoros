#ifndef DIALOGPHOTCORRECTIONEDITION_H
#define DIALOGPHOTCORRECTIONEDITION_H

#include <QDialog>

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

private:
    Ui::DialogPhotCorrectionEdition *ui;
    std::string m_file_path;
};

#endif // DIALOGPHOTCORRECTIONEDITION_H
