#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QString>
#include <string>
/**
 * @brief The FileUtils class
 */

namespace Euclid {
namespace PhosphorosUiDm {

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

    /**
     * @brief get the Model Sets Root Path
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/UI/ModelSet
     */
    static std::string getModelRootPath(bool check);

    /**
     * @brief get the Survey Mappings RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/UI/Survey
     */
    static std::string getMappingRootPath(bool check);

    /**
     * @brief get the Filters Root Path
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/UI/Filter
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
     * @brief get the Photometric Corrections RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/PhotometricCorrections
     */
    static std::string getPhotCorrectionsRootPath(bool check);

    /**
     * @brief get the Photometric Grid RootPath
     * @param check
     * if true ensure that the folder exist.
     * @return <rootPath>/PhotometricGrid
     */
    static std::string getPhotmetricGridRootPath(bool check);

};

}
}
#endif // FILEUTILS_H
