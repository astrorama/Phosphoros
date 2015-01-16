elements_subdir(PhzPhotometricCorrection 0.1)

elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)


find_package(gmock)
find_package(gtest)

#===== Libraries ===============================================================
elements_add_library(PhzPhotometricCorrection src/lib/*.cpp
                  LINK_LIBRARIES PhzDataModel SourceCatalog PhzLikelihood
                  PUBLIC_HEADERS PhzPhotometricCorrection )

#===== Boost tests =============================================================

elements_add_unit_test(PhotometricCorrectionAlgorithm_test tests/src/PhotometricCorrectionAlgorithm_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(CalculateScaleFactorMap_test tests/src/CalculateScaleFactorMap_test.cpp
                       LINK_LIBRARIES gmock gtest PhzPhotometricCorrection TYPE Boost
                       INCLUDE_DIRS gmock gtest)
elements_add_unit_test(FindBestFitModels_test tests/src/FindBestFitModels_test.cpp
                       LINK_LIBRARIES gmock gtest PhzPhotometricCorrection TYPE Boost
                       INCLUDE_DIRS gmock gtest)

