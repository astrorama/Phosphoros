#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void changeMainStackedWidgetIndex(int index);

private slots:

    void on_btn_HomeToModel_clicked();

    void on_btn_HomeToAnalysis_clicked();

    void on_btn_HomeToOption_clicked();


    void navigateToHome();

    void navigateToFilterManagement();

    void navigateToAuxDataManagement();


private:
    Ui::MainWindow *ui;

};



#endif // MAINWINDOW_H
