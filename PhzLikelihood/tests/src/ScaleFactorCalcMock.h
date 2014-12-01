/** 
 * @file ScaleFactorCalcMock.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef SCALEFACTORCALCMOCK_H
#define	SCALEFACTORCALCMOCK_H

#include <queue>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "SourceCatalog/SourceAttributes/Photometry.h"

namespace Euclid {

class ScaleFactorCalcMock {
  
public:
  
  typedef SourceCatalog::Photometry::const_iterator phot_iter;
  
  ScaleFactorCalcMock() : m_functor_call_queue {new std::queue<std::tuple<phot_iter, phot_iter, phot_iter, double>>{}} {}
  
  virtual ~ScaleFactorCalcMock() {
    if (m_functor_call_queue.unique() && !m_functor_call_queue->empty()) {
      std::stringstream message;
      message << "ScaleFactorCalcMock - ";
      message << "There were " << m_functor_call_queue->size() << " calls to the ";
      message << "ScaleFactorCalc functor that weren't performed";
      BOOST_ERROR(message.str());
    }
  }
  
  double operator()(phot_iter source_begin, phot_iter source_end, phot_iter model_begin) {
    // Check that we expect this function to be called
    if (m_functor_call_queue->empty()) {
      BOOST_ERROR("Unexpected ScaleFactorCalc functor call");
    }
    
    // Get the parameters that we expect to be called with
    phot_iter exp_source_begin = std::get<0>(m_functor_call_queue->front());
    phot_iter exp_source_end = std::get<1>(m_functor_call_queue->front());
    phot_iter exp_model_begin = std::get<2>(m_functor_call_queue->front());
    double result = std::get<3>(m_functor_call_queue->front());
    m_functor_call_queue->pop();
    
    // Check that we have the same number of photometries
    int phot_counter = 0;
    for (phot_iter source = source_begin; source != source_end; ++source) {
      ++phot_counter;
    }
    int exp_phot_counter = 0;
    for (phot_iter source = exp_source_begin; source != exp_source_end; ++source) {
      ++exp_phot_counter;
    }
    if (phot_counter != exp_phot_counter) {
      std::stringstream message;
      message << "ScaleFactorCalcMock - ";
      message << "Given source photometry contains fluxes for " << phot_counter;
      message << " filters but where expected " << exp_phot_counter;
      BOOST_ERROR(message.str());
    }
    
    // Check that we got the same photometries as we expected
    for (phot_iter source=source_begin, exp_source=exp_source_begin, model=model_begin, exp_model=exp_model_begin;
                                              source != source_end; ++source, ++exp_source, ++model, ++exp_model) {
      
      // Check that the source filter name is correct
      if (source.filterName() != exp_source.filterName()) {
        std::stringstream message;
        message << "ScaleFactorCalcMock - ";
        message << "Got source photometry for filter " << source.filterName();
        message << " when expected for filter " << exp_source.filterName();
        BOOST_ERROR(message.str());
      }
      
      // Check that the source Flux value is correct
      if ((*source).flux != (*exp_source).flux) {
        std::stringstream message;
        message << "ScaleFactorCalcMock - ";
        message << "Got source Flux " << (*source).flux << " when expected " << (*exp_source).flux;
        message << " (for filter " << source.filterName() << ")";
        BOOST_ERROR(message.str());
      }
      
      // Check that the source Flux error value is correct
      if ((*source).error != (*exp_source).error) {
        std::stringstream message;
        message << "ScaleFactorCalcMock - ";
        message << "Got source Flux error " << (*source).error << " when expected " << (*exp_source).error;
        message << " (for filter " << source.filterName() << ")";
        BOOST_ERROR(message.str());
      }
      
      // Check that the model filter name is correct
      if (model.filterName() != exp_model.filterName()) {
        std::stringstream message;
        message << "ScaleFactorCalcMock - ";
        message << "Got model photometry for filter " << model.filterName();
        message << " when expected for filter " << exp_model.filterName();
        BOOST_ERROR(message.str());
      }
      
      // Check that the model Flux value is correct
      if ((*model).flux != (*exp_model).flux) {
        std::stringstream message;
        message << "ScaleFactorCalcMock - ";
        message << "Got model Flux " << (*model).flux << " when expected " << (*exp_model).flux;
        message << " (for filter " << model.filterName() << ")";
        BOOST_ERROR(message.str());
      }
      
      // Check that the model Flux error value is correct
      if ((*model).error != (*exp_model).error) {
        std::stringstream message;
        message << "ScaleFactorCalcMock - ";
        message << "Got model Flux error " << (*model).error << " when expected " << (*exp_model).error;
        message << " (for filter " << model.filterName() << ")";
        BOOST_ERROR(message.str());
      }
    }
    
    // Returned the expected value
    return result;
  }
  
  void expect_functor_call(phot_iter source_begin, phot_iter source_end, phot_iter model_begin, double result) {
    m_functor_call_queue->emplace(source_begin, source_end, model_begin, result);
  }
  
private:
  
  std::shared_ptr<std::queue<std::tuple<phot_iter, phot_iter, phot_iter, double>>> m_functor_call_queue;
  
}; // end of class ScaleFactorCalcMock

} // end of namespace Euclid

#endif	/* SCALEFACTORCALCMOCK_H */

