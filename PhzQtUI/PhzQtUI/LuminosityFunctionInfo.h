/*
 * LuminosityFunctionInfo.h
 *
 *  Created on: Sep 2, 2015
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_LUMINOSITYFUNCTIONINFO_H_
#define PHZQTUI_PHZQTUI_LUMINOSITYFUNCTIONINFO_H_

namespace Euclid {
namespace PhzQtUI {

class LuminosityFunctionInfo{
public:


  bool in_mag=true;
  bool is_custom=false;
  double alpha{0.};
  double phi{0.};
  double m{0.};
  double l{0.};
  std::string curve_name{""};

  double z_min;
  double z_max;
  std::string sedGroupName;

  bool isComplete(){
    return (is_custom && curve_name.length()>0) || (!is_custom && phi>0.);
  }

  QString getDescription(){
    if (is_custom){
      return "Custom Curve: " + QString::fromStdString(curve_name);
    } else {
      QString m_l = "";
      if (in_mag){
        m_l= "M*="+QString::number(m,'f', 2);
      } else {
        m_l= "L*="+QString::number(l,'f', 2);
      }

      return "Schechter: a=" +QString::number(alpha,'f', 2) + " phi="+QString::number(phi,'f', 2) + " " +m_l;
    }
  }
};

}
}


#endif /* PHZQTUI_PHZQTUI_LUMINOSITYFUNCTIONINFO_H_ */
