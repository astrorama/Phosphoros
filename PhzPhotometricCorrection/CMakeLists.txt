elements_subdir(PhzPhotometricCorrection 0.1)

elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(GridContainer)
elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)

find_package(Boost REQUIRED COMPONENTS serialization)

#===== Libraries ===============================================================
#elements_add_library(PhzPhotometricCorrection src/lib/*.cpp
#                  LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost SourceCatalog XYDataset MathUtils PhzDataModel PhzLikelihood
#                  INCLUDE_DIRS Boost SourceCatalog XYDataset MathUtils PhzDataModel PhzLikelihood GridContainer
#                  PUBLIC_HEADERS PhzPhotometricCorrection )

#===== Boost tests =============================================================
elements_add_unit_test(CalculateScaleFactorMap_test tests/src/CalculateScaleFactorMap_test.cpp
                       LINK_LIBRARIES PhzDataModel PhzLikelihood TYPE Boost)