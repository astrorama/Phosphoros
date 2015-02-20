#include <iostream>

#include <QDir>

#include "QtUI/ParameterRule.h"
#include "QtUI/FileUtils.h"

namespace PhosphorosUiDm {


ParameterRule::ParameterRule()
{
}

std::string ParameterRule::getSedRootObject(std::string rootPath) const{

    std::string res = FileUtils::removeStart(m_sed_root_object,rootPath);

    if (res.length()==0){
        res="<Root>";
    }
    return res;
}

std::string ParameterRule::getReddeningRootObject(std::string rootPath) const{

    std::string res = FileUtils::removeStart(m_reddening_root_object,rootPath);

    if (res.length()==0){
        res="<Root>";
    }
    return res;
}

long long ParameterRule::getModelNumber() const{


    long sed_factor = 0;

    std::list<std::string> next_scan_folder{{getSedRootObject()}};

    while (next_scan_folder.size()>0){
        auto curr_dir = next_scan_folder.front();

        QFileInfo info(QString::fromStdString(curr_dir));
        if (info.isFile()){
             ++sed_factor;
            break;
        }

        QDir curr_qdir(QString::fromStdString(curr_dir));

        QStringList fileNames = curr_qdir.entryList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot );
        foreach (const QString &fileName, fileNames) {
            QFileInfo curr_file(curr_qdir.absoluteFilePath(QString::fromStdString(curr_dir))+QDir::separator()+ fileName) ;
            std::string full_name= curr_file.absoluteFilePath().toStdString();

            if ( curr_file.isDir()==1 ){
                next_scan_folder.push_back(full_name);
            }
            else {
                ++sed_factor;
            }
        }

        next_scan_folder.remove(curr_dir);
    }

    sed_factor-=getExcludedSeds().size();

    long red_factor = 0;

    next_scan_folder={{getReddeningRootObject()}};

    while (next_scan_folder.size()>0){
        auto curr_dir = next_scan_folder.front();

        QFileInfo info(QString::fromStdString(curr_dir));
        if (info.isFile()){
             ++red_factor;
            break;
        }

        QDir curr_qdir(QString::fromStdString(curr_dir));

        QStringList fileNames = curr_qdir.entryList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot );
        foreach (const QString &fileName, fileNames) {
            QFileInfo curr_file(curr_qdir.absoluteFilePath(QString::fromStdString(curr_dir))+QDir::separator()+ fileName) ;
            std::string full_name= curr_file.absoluteFilePath().toStdString();

            if ( curr_file.isDir()==1 ){
                next_scan_folder.push_back(full_name);
            }
            else {
                ++red_factor;
            }
        }

        next_scan_folder.remove(curr_dir);
    }


    red_factor-=getExcludedReddenings().size();

    long long z_factor = 1;
    if (m_redshift_range.getStep()>0 && m_redshift_range.getMax()>m_redshift_range.getMin()){
        z_factor=(m_redshift_range.getMax()-m_redshift_range.getMin())/m_redshift_range.getStep();
    }

    long long ebv_factor = 1;
    if (m_ebv_range.getStep()>0 && m_ebv_range.getMax()>m_ebv_range.getMin()){
        ebv_factor=(m_ebv_range.getMax()-m_ebv_range.getMin())/m_ebv_range.getStep();
    }

    return sed_factor*red_factor*ebv_factor*z_factor;
}


void ParameterRule::setSedRootObject(std::string sed_root_object){
    m_sed_root_object=sed_root_object;
}

void ParameterRule::setReddeningRootObject(std::string reddening_root_object){
    m_reddening_root_object=reddening_root_object;
}

const std::list<std::string>& ParameterRule::getExcludedSeds() const{
    return m_excluded_sed;
}
const std::list<std::string>& ParameterRule::getExcludedReddenings() const{
    return m_excluded_reddening;
}

void ParameterRule::setExcludedSeds( std::list<std::string> excluded_sed){
    m_excluded_sed=std::move(excluded_sed);
}
void ParameterRule::setExcludedReddenings( std::list<std::string> excluded_reddening){
    m_excluded_reddening=std::move(excluded_reddening);
}

const Range& ParameterRule::getEbvRange() const{
    return m_ebv_range;
}
const Range& ParameterRule::getZRange() const{
    return m_redshift_range;
}

void ParameterRule::setEbvRange(Range ebv_range){
    m_ebv_range=std::move(ebv_range);
}
void ParameterRule::setZRange(Range z_range){
    m_redshift_range=std::move(z_range);
}

}

