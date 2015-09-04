#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QString>
#include <string>
#include <map>
#include "PhzConfiguration/PhosphorosPathConfiguration.h"
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


    //////////////////////////////////////////////////////////
    //// Application base paths as of configuration.
    /**
     * @brief get the data root path. This value can be altered by the user and
     * is used to build the other path.
     * @return the data root path
     */
    static std::string getRootPath();

    static std::string getRootPath(bool with_separator);


    static PhzConfiguration::PhosphorosPathConfiguration getRootPaths();

    static std::string getGUIConfigPath();


    static std::string getAuxRootPath();

    static void savePath(const std::map<std::string,std::string>& path_list);

    static std::map<std::string,std::string> readPath();

    /**
     * @brief get the Model Sets Root Path
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/UI/ModelSet
     */
    static std::string getModelRootPath(bool check);



    /**
     * @brief get the Catalog Root Path
     * @param check
     * if true ensure that the folder exist.
     */
    static std::string getCatalogRootPath(bool check, const std::string& catalog_type);

    static std::string getIntermediaryProductRootPath(bool check, const std::string& catalog_type);

    static std::string getResultRootPath(bool check, const std::string& catalog_type, const std::string& cat_file_name);


    /**
     * @brief get the Filters Root Path
     * @param check
     * if true ensure that thUI/Filter
     */
    static std::string getFilterRootPath(bool check);

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
     * @brief get the Luminosity Function Curves RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/LuminosityFunctionCurves
     */
    static std::string getLuminosityCurveRootPath(bool check);

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


    //////////////////////////////////////////////////////////
    //// Last used path.
    static std::string getLastUsedPath();

    static void setLastUsedPath(const std::string& path);

    /////////////////////////////////////////////////////////
    //// Preferences
    static std::map<std::string,std::map<std::string,std::string>> readUserPreferences();

    static void writeUserPreferences(std::map<std::string,std::map<std::string,std::string>> preferences);

    static void setUserPreference(const std::string& catalog, const std::string& key, const std::string& value );

    static void clearUserPreference(const std::string& catalog, const std::string& key);

    static std::string getUserPreference(const std::string& catalog, const std::string& key);

};

}
}
#endif // FILEUTILS_H
