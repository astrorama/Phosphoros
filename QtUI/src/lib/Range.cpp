#include <string>
#include<iostream>
#include<sstream>
#include <iomanip>
#include "QtUI/Range.h"

namespace PhosphorosUiDm {


Range::Range(){
    m_min=0.;
    m_max=0;
    m_step=0;
}

Range::Range(double min,double max, double step):m_min(min), m_max(max),m_step(step){}

std::string Range::getStringRepresentation() const{

    std::ostringstream convert;
    convert <<"[";
    convert << std::fixed << std::setprecision(2) << m_min;
    convert <<" :: ";
    convert<< m_max;
    convert <<"] by ";
    convert <<  std::setprecision(4) << (m_step);

    return convert.str();
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

