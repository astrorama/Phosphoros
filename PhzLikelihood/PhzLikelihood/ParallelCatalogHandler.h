/** 
 * @file ParallelCatalogHandler.h
 * @date February 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_PARALLELCATALOGHANDLER_H
#define	PHZLIKELIHOOD_PARALLELCATALOGHANDLER_H

#include "PhzLikelihood/CatalogHandler.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class ParallelCatalogHandler
 * 
 * @brief Wrapper class around the CatalogHandler class, which enables parallel
 * processing of the sources.
 */
class ParallelCatalogHandler {
  
public:
  
  typedef CatalogHandler::MarginalizationFunction MarginalizationFunction;
  
  /**
   * Defines the signature of the functions which can be used as listeners for
   * the progress of the catalog handling. The first parameter is the
   * number of the current step and the second is the total number of steps.
   */
  typedef std::function<void(size_t step, size_t total)> ProgressListener;
  
  /**
   * Constructs a new ParallelCatalogHandler instance. If the given photometric
   * correction map does not contain corrections for all the filters of the
   * given model photometries an exception is thrown.
   * 
   * @param phot_corr_map
   *    A map with the photometric corrections to be applied for each filter
   * @param phot_grid
   *    A grid with the photometries of the models to perform the fitting with
   * @param marginalization_func
   *    The function to use for marginalizing the multi-dimensional likelihood
   *    grid to a 1D PDF
   * @throws ElementsException
   *    If the phot_corr_map does not contain photometric corrections for all
   *    the filters of the model photometries
   */
  ParallelCatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                         const PhzDataModel::PhotometryGrid& phot_grid,
                         MarginalizationFunction marginalization_func);
  
  /**
   * Iterates through a set of sources and calculates the PHZ parameters for
   * each of them, using all the available threads in parallel. The assumption
   * that all the sources contain the Photometry attribute is made. The progress
   * of the catalog handling can be observed by using a ProgressListener, which
   * will be notified every 0.1 sec. The output is provided by calls to the
   * given OutputHandler. There is one call performed for each source and the
   * order of the calls is the same as the order of the sources in the input
   * catalog. For this reason, these calls are executed after all threads have
   * finished their jobs.
   * 
   * @tparam SourceIter
   *    The type of iterator over the sources. It must be an iterator over
   *    objects of type SourceCatalog::Source
   * @param source_begin
   *    An iterator pointing to the first source
   * @param source_end
   *    An iterator pointing to one after the last source
   * @param out_handler
   *    A reference to the PhzOutput::OutputHandler object, which will be used
   *    for returning the output.
   * @param progress_listener
   *    A function of type ProgressListener which is notified with the progress
   *    of the catalog handling (defaults to no action)
   */
  template<typename SourceIter>
  void handleSources(SourceIter source_begin, SourceIter source_end,
                     PhzOutput::OutputHandler& out_handler,
                     ProgressListener progress_listener=ProgressListener{}) const;
                     
private:
  
  CatalogHandler m_catalog_handler;
  
};

}
}

#include "PhzLikelihood/_impl/ParallelCatalogHandler.icpp"

#endif	/* PHZLIKELIHOOD_PARALLELCATALOGHANDLER_H */

