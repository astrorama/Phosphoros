#ifndef PHZQTUI_PHZQTUI_OPTIONMODEL_H_
#define PHZQTUI_PHZQTUI_OPTIONMODEL_H_

#include <QString>
#include <QWidget>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
  namespace PhzQtUI {

    typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

    class OptionModel : public QObject {
      Q_OBJECT
      public:
        OptionModel();

        virtual ~OptionModel() = default;

        void loadOption(DatasetRepo filter_repository,
                               DatasetRepo seds_repository,
                               DatasetRepo redenig_curves_repository,
                               DatasetRepo luminosity_repository);

        bool isGlobalInEdition();
        bool isCosmoInEdition();
        QString getRootPath();
        QString getCatPath();
        QString getAuxPath();
        QString getInterPath();
        QString getResPath();
        bool isCatPathDefault();
        bool isAuxPathDefault();
        bool isInterPathDefault();
        bool isResPathDefault();
        bool getOverrideThread();
        size_t getThreadNb();
        size_t getBufferSize();
        double getHubble();
        double getOmegaM();
        double getOmegaLambda();

        DatasetRepo getFilterRepo();
        DatasetRepo getSedRepo();
        DatasetRepo getReddeningRepo();
        DatasetRepo getLuminosityRepo();


        void save();
        void cancel();

        signals:

        void repoReloaded(int setIndex);

      public slots:

        /**
         * If empty use the default value
         */
        void setCatalog(const QString & new_cat_path);

        /**
         * If empty use the default value
         */
        void setAuxiliary(const QString & new_aux_path);

        /**
         * If empty use the default value
         */
        void setIntermediary(const QString & new_iter_path);

        /**
         * If empty use the default value
         */
        void setResult(const QString & new_res_path);



        void setThread(int thread_nb);


        void setDefaultThread(bool custom);


        void setBufferSize(int buffer_size);

        /**
         * If new_h0<=-1 set the default value
         */
        void setHubble(const QString &  new_h0);

        /**
        * If new_omega_m<=-1 set the default value
        */
        void setOmegaM(const QString &  new_omega_m);

        /**
        * If new_omega_lambda<=-1 set the default value
        */
        void setOmegaLambda(const QString &  new_omega_lambda);


      private:
        DatasetRepo m_filter_repository;
        DatasetRepo m_seds_repository;
        DatasetRepo m_redenig_curves_repository;
        DatasetRepo m_luminosity_repository;

        bool m_global_edition = false;
        bool m_cosmo_edition = false;
        QString m_root_path;
        QString m_cat_path_saved;
        QString m_cat_path_edited;
        QString m_aux_path_saved;
        QString m_aux_path_edited;
        QString m_inter_path_saved;
        QString m_inter_path_edited;
        QString m_res_path_saved;
        QString m_res_path_edited;
        bool m_override_thread_saved;
        bool m_override_thread_edited;
        size_t m_thread_number_saved;
        size_t m_thread_number_edited;
        size_t m_buffer_size_saved;
        size_t m_buffer_size_edited;
        double m_hubble_saved;
        double m_hubble_edited;
        double m_omega_m_saved;
        double m_omega_m_edited;
        double m_omega_l_saved;
        double m_omega_l_edited;
    };

  }
}


#endif /* PHZQTUI_PHZQTUI_OPTIONMODEL_H_ */
