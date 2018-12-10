#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <map>
#include <QMainWindow>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

#include "PhzQtUI/OptionModel.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class MainWindow;
}

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

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

    void navigateToHomeWithReset(bool);

    void navigateToParameter(bool);

    void navigateToCatalog(bool);

    void navigateToComputeRedshift(bool);

    void navigateToConfig();

    void quit(bool);



private:
    std::unique_ptr<Ui::MainWindow> ui;
    bool m_model_loaded = false;
    bool m_mapping_loaded = false;

    DatasetRepo m_filter_repository;
    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;
    DatasetRepo m_luminosity_repository;
    std::shared_ptr<OptionModel> m_option_model_ptr{ new OptionModel};


};

}
}

#endif // MAINWINDOW_H
