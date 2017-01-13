elements_subdir(PhzPhotometricCorrection 0.1)

elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)


find_package(gmock)
find_package(gtest)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE tests/src/PhotometricCorrectionCalculator_test.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()

#===== Libraries ===============================================================
elements_add_library(PhzPhotometricCorrection src/lib/*.cpp
                  LINK_LIBRARIES PhzDataModel SourceCatalog PhzLikelihood
                  PUBLIC_HEADERS PhzPhotometricCorrection )

#===== Boost tests =============================================================

elements_add_unit_test(PhotometricCorrectionAlgorithm_test tests/src/PhotometricCorrectionAlgorithm_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(DefaultStopCriteria_test tests/src/DefaultStopCriteria_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)

elements_add_unit_test(FindMedianPhotometricCorrectionsFunctor_test tests/src/FindMedianPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(FindWeightedMedianPhotometricCorrectionsFunctor_test tests/src/FindWeightedMedianPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(FindMeanPhotometricCorrectionsFunctor_test tests/src/FindMeanPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(FindWeightedMeanPhotometricCorrectionsFunctor_test tests/src/FindWeightedMeanPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)

#===== Tests using GMock =======================================================
if(GMOCK_FOUND AND GTEST_FOUND)
  elements_add_unit_test(CalculateScaleFactorMap_test tests/src/CalculateScaleFactorMap_test.cpp
                         LINK_LIBRARIES gmock gtest PhzPhotometricCorrection TYPE Boost
                         INCLUDE_DIRS gmock gtest)
  elements_add_unit_test(FindBestFitModels_test tests/src/FindBestFitModels_test.cpp
                         LINK_LIBRARIES gmock gtest PhzPhotometricCorrection TYPE Boost
                         INCLUDE_DIRS gmock gtest)
  elements_add_unit_test(PhotometricCorrectionCalculator_test tests/src/PhotometricCorrectionCalculator_test.cpp
                         LINK_LIBRARIES gmock gtest PhzPhotometricCorrection TYPE Boost
                         INCLUDE_DIRS gmock gtest)
endif(GMOCK_FOUND AND GTEST_FOUND)