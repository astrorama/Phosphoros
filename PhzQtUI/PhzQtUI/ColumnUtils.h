#ifndef COLUMNSUTILS_H
#define COLUMNSUTILS_H
#include <QString>
#include <map>
#include <string>
#include <vector>
#include "ElementsKernel/Logging.h"

/**
 * @brief The ColumnUtils class
 */

namespace Euclid {
namespace PhzQtUI {

static  Elements::Logging cutils_logger = Elements::Logging::getLogger("ColumnUtils");


class ColumnUtils {
public:
  enum Category {
  FLUX,
  ERROR,
  SHIFT
};

  ColumnUtils(std::vector<QString> list):m_column_list{list}{};
  
  void changeColumnList(std::vector<QString> list){
      m_column_list = list;
  }
  
  void changeColumnList(std::set<std::string> list){
      std::vector<QString> qlist{};
      for (auto iter = list.begin(); iter!=list.end();++iter){
         qlist.push_back(QString::fromStdString(*iter));
      }
      changeColumnList(qlist);
  }
  
  float categoryWeight(QString& column_name, Category filterFor=Category::FLUX) const {
      const std::vector<QString> flux_items{"FLUX","F","MAG","M"};
      const std::vector<QString> err_items{"ERR"};
      const std::vector<QString> shift_items{"SHIFT"};
      const std::vector<QString> excluded_items{"FLAG", "REDSHIFT","PDF"};
      
      QString col=column_name.toUpper();
      
      size_t flux_res=0;
      size_t err_res=0;
      size_t shift_res=0;
      size_t excluded_res=0;
      for (auto iter = flux_items.begin(); iter!=flux_items.end(); ++iter){
          if (col.startsWith(*iter) || col.endsWith(*iter) || ( iter->length()>1 && col.contains(*iter)) ){
              flux_res+=1;
          }
      }
      
      for (auto iter = err_items.begin(); iter!=err_items.end(); ++iter){
          if (col.startsWith(*iter) || col.endsWith(*iter) || ( iter->length()>1 && col.contains(*iter)) ){
              err_res+=1;
          }
      }
      
      for (auto iter = shift_items.begin(); iter!=shift_items.end(); ++iter){
          if (col.contains(*iter)){
              shift_res+=1;
          }
      }
      
      for (auto iter = excluded_items.begin(); iter!=excluded_items.end(); ++iter){
          if (col.contains(*iter)){
              excluded_res+=1;
          }
      }
      
      switch (filterFor){
          case Category::FLUX: 
          {
            if (flux_res>0 && err_res==0 && shift_res==0 && excluded_res==0){
                return 5.0;
            } else if (err_res>0 && shift_res==0 && excluded_res==0){
                return 3.0;
            } else if (shift_res>0 && excluded_res==0){
                return 2.0;
            } else if (excluded_res>0){
                return 1.0;
            } else {
                return 4.0;
            }
          }
          case Category::ERROR:
          {
            if ( err_res>0 && shift_res==0 && excluded_res==0){
                return 5.0;
            } else  if ( flux_res>0 && shift_res==0 && excluded_res==0){
                return 3.0;
            }  if (  shift_res>0 && excluded_res==0){
                return 2.0;
            }  if (  excluded_res>0){
                return 1.0;
            } else {
                return 4.0;
            }
          }
          default:
            if ( shift_res>0 && excluded_res==0){
                return 5.0;
            } else if ( flux_res>0 && err_res==0 && excluded_res==0){
                return 3.0;
            } else if ( err_res>0 && excluded_res==0){
                return 2.0;
            } else if ( excluded_res>0){
                return 1.0;
            } else {
                return 4.0;
            }
            
      }
  }
  
   float similarity(QString& filter_name, QString& column_name) const{
      if (filter_name.length()==0) {
          return 0.0;
      }
      
      QString col=column_name.toUpper();
      QString filter = filter_name.toUpper();
      size_t filter_length = filter_name.length();
      size_t weight = 0;
      size_t max_weight = 0;
      // cutils_logger.info()<< "Column : " << col.toStdString();
      for (size_t length=1; length<=filter_length; ++length){
          for (size_t shift=0; shift<=filter_length-length; ++shift){
              auto bit = filter.left(shift+length).right(length);
              // cutils_logger.info()<< "Filter bit : " << bit.toStdString();
              max_weight+=length;
              if (col.contains(bit)){
                 // cutils_logger.info()<< "Column : " << col.toStdString() <<" contains  bit : " << bit.toStdString();
                 weight+=length;
              } else {
                 // cutils_logger.info()<< "Column : " << col.toStdString() <<" do not contains  bit : " << bit.toStdString();
              }
          }
      }
      // cutils_logger.info()<< weight <<"/"<<max_weight;
      return float(weight)/max_weight;
  }
  
  
  std::vector<QString> getOrderedList(QString filter_name, Category filterFor=Category::FLUX){
     std::multimap<float, QString> internal_map{};
     for (auto iter = m_column_list.begin(); iter!=m_column_list.end(); ++iter){
         auto weight = categoryWeight(*iter, filterFor) + similarity(filter_name, *iter);
         internal_map.insert(std::pair<float, QString>(weight,*iter));
         // cutils_logger.info()<<weight <<" " <<(*iter).toStdString();
     }
     
     std::vector<QString> result{};
     for (auto map_iter = internal_map.begin(); map_iter!=internal_map.end(); ++map_iter){
         result.emplace(result.begin(), map_iter->second);
        
     }
     
     // cutils_logger.info()<<"first column "  <<result[0].toStdString();
     return result;
  }
  
  bool hasData(){
    return m_column_list.size()>0;
  }
  
private:
  std::vector<QString> m_column_list;  

};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // COLUMNSUTILS_H
