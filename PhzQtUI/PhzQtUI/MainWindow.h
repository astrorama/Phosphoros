#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <map>
#include <QMainWindow>

namespace Euclid {
namespace PhzQtUI {

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

    void on_btn_HomeToCatalog_clicked();

    void navigateToNewCatalog(std::string);

    void navigateToHome();



private:
    std::unique_ptr<Ui::MainWindow> ui;
    bool m_model_loaded = false;
    bool m_mapping_loaded = false;

};

}
}

#endif // MAINWINDOW_H
