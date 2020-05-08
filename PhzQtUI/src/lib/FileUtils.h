#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QString>
#include <string>
#include <map>
#include <boost/program_options.hpp>
#include "Configuration/ConfigManager.h"

/**
 * @brief The FileUtils class
 */

namespace Euclid {
namespace PhzQtUI {

class FileUtils
{
public:
    FileUtils();

    //////////////////////////////////////////////////////////
    //// File system operation
    /**
     * @brief Delete a directory and its content of the disk.
     * If the dir name point to a file, only this file is deleted.
     * @param dirName
     * @return
     */
    static bool removeDir(const QString &dirName);

    /**
     * @brief copy a Folder and its content to the requested place.
     * if the srcFilePath point to a file, only this file is copied.
     * @param srcFilePath
     * @param tgtFilePath
     * @return
     */
    static bool copyRecursively(const QString &srcFilePath,
                                const QString &tgtFilePath);

    //////////////////////////////////////////////////////////
    //// File name manipulation

    /**
     * @brief Check if a string ends with a given sub-string,
     * used to detect if a given extention is present.
     * @param value
     * @param ending
     * @return true if the 'value' ends with 'ending'
     */
    static bool ends_with(const std::string  & value, const std::string & ending);

    /**
     * @brief Remove the end of a given string if it ends with the provided ending.
     * @param name
     * @param ext
     * @return the 'name' with the 'ext' removed if it was present,
     * otherwise the unalterated 'name'.
     */
    static std::string removeExt(const std::string& name, const std::string& ext);

    /**
     * @brief Add at the end of a given string the provided ending if not already present.
     * @param name
     * @param ext
     * @return the 'name' with the 'ext' added if it was not present,
     * otherwise the unalterated 'name'.
     */
    static std::string addExt(const std::string& name, const std::string& ext);

    /**
     * @brief Check if a string starts with a given sub-string,
     * used to detect if a given path is part of a parent path.
     * @param value
     * @param begining
     * @return true if the 'value' strts with 'begining'
     */
    static bool starts_with(const std::string  & value, const std::string & begining);

    /**
     * @brief Remove the begining of a given string if it starts with the provided beginings.
     * @param name
     * @param start
     * @return the 'name' with the 'start' removed if it was present,
     * otherwise the unalterated 'name'.
     */
    static std::string removeStart(const std::string& name, const std::string& start);


    static std::string checkFileColumns(const std::string & file_name, const std::vector<std::string>& requiered_columns);


    //////////////////////////////////////////////////////////
    //// Application base paths as of configuration.
    /**
     * @brief get the default root path
     */
    static Configuration::ConfigManager& getRootPaths();


    /**
     * @brief Get the Phosphoros directory. This variable cannot be altered by
     * the user into the GUI.
     * It gives the base of all the path in the application.
     *
     * @param with_separator
     * If true the path end with a separator.
     *
     * @return The root path
     */
    static std::string getRootPath(bool with_separator);

    /////////////////////////// GUI Specific path /////////////////////////
    /**
     * @brief Get the path into which the GUI configs are stored.
     * it is computed as <rootPath>/config/GUI.
     */
    static std::string getGUIConfigPath();

    /**
     * @brief Get the path into which the Luminosity GUI configs are stored.
     * it is computed as <GUIConfigPath>/LuminosityPrior/<catalog_type>/<model>.
     *
     * @param check
     * If true ensure the directory exists
     *
     * @param catalog_type
     * The Catalog Type name
     *
     * @model
     * The Parameter space model
     */
    static std::string getGUILuminosityPriorConfig(bool check, const std::string & catalog_type, const std::string& model);

    /**
       * @brief get the Model Sets Root Path
       * @param check
       * if true ensure that the folder exist.
       * @return <rootPath>/UI/ModelSet
       */
      static std::string getModelRootPath(bool check);

      /**
         * @brief get the Catalog Config Root Path
         * @param check
         * if true ensure that the folder exist.
         * @return <rootPath>/UI/Catalogs
         */
        static std::string getCatalogConfigRootPath(bool check);




     /////////////////////// Overridable Paths //////////////////////////////
    /**
      * @brief Get the Catalog Root Path. By default it is computed as
      * <rootPath>/Catalogs/<catalog_type> but can be overrided to
      * <catalogRootPath>/<catalog_type> by the user in the Option popup. When the
      * catalog_type is not set the separator is not added at the end of the path.
      *
      * @param check
      * if true ensure that the folder exist.
      *
      * @param catalog_type
      * The Catalog Type name
      */
      static std::string getCatalogRootPath(bool check, const std::string& catalog_type);

      /**
       *  @brief Get the default Catalog Root Path. It is computed as
       *  <rootPath>/Catalogs
       */
      static std::string getDefaultCatalogRootPath();

      /**
       * @brief Get the Aux data Root Path. By default it is computed as
       * <rootPath>/AuxiliaryData but can be overrided to
       * <AuxDataRootPath>/ by the user in the Option popup.
       */
      static std::string getAuxRootPath();

      /**
       *  @brief Get the default Aux data Root Path. It is computed as
       *  <rootPath>/AuxiliaryData
       */
      static std::string getDefaultAuxRootPath();

     /**
      * @brief Get the Intermediary Product Root Path. By default it is computed as
      * <rootPath>/IntermediateProduct/<catalog_type> but can be overrided to
      * <IntermediateProductRootPath>/<catalog_type> by the user in the Option popup.
      * When the catalog_type is not set the separator is not added at the end of the path.
      *
      * @param check
      * if true ensure that the folder exist.
      *
      * @param catalog_type
      * The Catalog Type name
      */
      static std::string getIntermediaryProductRootPath(bool check, const std::string& catalog_type);

      /**
       *  @brief Get the default Intermediary Root Path. It is computed as
       *  <rootPath>/IntermediateProduct
       */
      static std::string getDefaultIntermediaryProductRootPath();

      /**
       * @brief Get the Results Root Path. By default it is computed as
       * <rootPath>/Results/<catalog_type>/<cat_file_name> but can be overrided to
       * <ResultsRootPath>/<catalog_type>/<cat_file_name> by the user in
       * the Option popup.
       * When the cat_file_name and/or catalog_type is not set the separator is
       * not added at the end of the path.
       *
       * @param check
       * if true ensure that the folder exist.
       *
       * @param catalog_type
       * The Catalog Type name
       *
       * @param cat_file_name
       * The Catalog File name
       */
      static std::string getResultRootPath(bool check, const std::string& catalog_type, const std::string& cat_file_name);

      /**
       *  @brief Get the default Results Root Path. It is computed as
       *  <rootPath>/Results
       */
       static std::string getDefaultResultsRootPath();


     /**
     * @brief save the overriadable path in the option file
     */
    static void savePath(const std::map<std::string,std::string>& path_list);

    /**
     * @brief read the overriadable path from the option file
     */
    static std::map<std::string,std::string> readPath();



///////////////////////////////////////////////



    /**
     * @brief get the Filters Root Path
     * @param check
     * if true ensure that Auxdata/Filter
     */
    static std::string getFilterRootPath(bool check);


    /**
     * @brief get the Luminosity Function Curve Root Path
     * @param check
     * if true ensure the folder exists
     */
    static std::string getLuminosityFunctionCurveRootPath(bool check);


    static std::string getLuminosityFunctionGridRootPath(bool check, const std::string & catalog_type);

    /**
     * @brief get the SEDs RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/SED
     */
    static std::string getSedRootPath(bool check);

    /**
     * @brief get the Reddening Curves RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/RedCurve
     */
    static std::string getRedCurveRootPath(bool check);


    /**
     * @brief get the Photometric Corrections RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/PhotometricCorrections
     */
    static std::string getPhotCorrectionsRootPath(bool check, const std::string& catalog_type);

    /**
     * @brief get the Photometric Grid RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/PhotometricGrid
     */
    static std::string getPhotmetricGridRootPath(bool check, const std::string& catalog_type);

    /**
     * @brief get the Galactic Correction Grid RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/CorrectionGrid
     */
    static std::string getGalacticCorrectionGridRootPath(bool check, const std::string& catalog_type);


    static void buildDirectories();

    //////////////////////////////////////////////////////////
    //// Last used path.
    static std::string getLastUsedPath();

    static void setLastUsedPath(const std::string& path);


    /// configuration
    static std::map<std::string, boost::program_options::variable_value> getPathConfiguration(bool add_cat,bool add_aux,bool add_inter, bool add_res);

};

}
}
#endif // FILEUTILS_H
