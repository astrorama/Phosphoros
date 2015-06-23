#include <string>
#include <QString>
#include "PhzQtUI/Range.h"

namespace Euclid {
namespace PhzQtUI {


Range::Range(){
    m_min=0.;
    m_max=0;
    m_step=0;
}

Range::Range(double min,double max, double step):m_min(min), m_max(max),m_step(step){}

std::string Range::getStringRepresentation() const{

 QString string ="[" +QString::number(m_min,'g',2)+" , "+QString::number(m_max,'g',2)+"] step "+QString::number(m_step,'g',4);


    return string.toStdString();
}


std::string Range::getConfigStringRepresentation() const{
  return std::to_string(m_min)+" " +std::to_string(m_max)+" " +std::to_string(m_step);
}


double Range::getMin() const{
    return m_min;
}

void Range::setMin(double min){
    m_min=min;
}

double Range::getMax() const{
    return m_max;
}

void Range::setMax(double max){
    m_max=max;
}

double Range::getStep() const{
    return m_step;
}

void Range::setStep(double step){
    m_step=step;
}

}
}

