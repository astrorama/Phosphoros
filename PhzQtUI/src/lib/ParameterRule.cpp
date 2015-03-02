#include <iostream>

#include <QDir>

#include "PhzQtUI/ParameterRule.h"
#include "PhzQtUI/FileUtils.h"

#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"

namespace Euclid {
namespace PhzQtUI {


ParameterRule::ParameterRule()
{
}

std::string ParameterRule::getSedRootObject(std::string rootPath) const{

   return FileUtils::removeStart(m_sed_root_object,rootPath);
}

std::string ParameterRule::getReddeningRootObject(std::string rootPath) const{

    return FileUtils::removeStart(m_reddening_root_object,rootPath);

}

long long ParameterRule::getModelNumber() const{


    long sed_factor = 1;
    std::unique_ptr < XYDataset::FileParser > file_parser {new XYDataset::AsciiParser { } };
    XYDataset::FileSystemProvider provider { FileUtils::getSedRootPath(false), std::move(file_parser) };
     auto unordered = provider.listContents(m_sed_root_object);
     if (sed_factor<unordered.size()){
       sed_factor=unordered.size();
     }
     sed_factor-=getExcludedSeds().size();

    long red_factor = 1;
    std::unique_ptr < XYDataset::FileParser >  red_file_parser {new XYDataset::AsciiParser { } };
    XYDataset::FileSystemProvider red_provider { FileUtils::getRedCurveRootPath(false), std::move(red_file_parser) };
    unordered = red_provider.listContents(m_reddening_root_object);
    if (red_factor<unordered.size()){
      red_factor=unordered.size();
    }
    red_factor-=getExcludedReddenings().size();

    long long z_factor = 1;
    if (m_redshift_range.getStep()>0 && m_redshift_range.getMax()>m_redshift_range.getMin()){
        z_factor=(m_redshift_range.getMax()-m_redshift_range.getMin())/m_redshift_range.getStep()+1;
    }

    long long ebv_factor = 1;
    if (m_ebv_range.getStep()>0 && m_ebv_range.getMax()>m_ebv_range.getMin()){
        ebv_factor=(m_ebv_range.getMax()-m_ebv_range.getMin())/m_ebv_range.getStep()+1;
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
}

