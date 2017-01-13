elements_subdir(PhzModeling 0.1)

elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(GridContainer)
elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhzDataModel)

find_package(Boost REQUIRED COMPONENTS serialization)

#===== Libraries ===============================================================
elements_add_library(PhzModeling src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost SourceCatalog XYDataset MathUtils PhzDataModel
                  INCLUDE_DIRS Boost SourceCatalog XYDataset MathUtils PhzDataModel GridContainer
                  PUBLIC_HEADERS PhzModeling )

#===== Boost tests =============================================================

elements_add_unit_test(RedshiftFunctor_test tests/src/RedshiftFunctor_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(ExtinctionFunctor_test tests/src/ExtinctionFunctor_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(ModelDatasetGenerator_test tests/src/ModelDatasetGenerator_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(ModelDatasetGrid_test tests/src/ModelDatasetGrid_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(ApplyFilterFunctor_test tests/src/ApplyFilterFunctor_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(ModelFluxAlgorithm_test tests/src/ModelFluxAlgorithm_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(BuildFilterInfoFunctor_test tests/src/BuildFilterInfoFunctor_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(PhotometryAlgorithm_test tests/src/PhotometryAlgorithm_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(PhotometryGridCreator_test tests/src/PhotometryGridCreator_test.cpp
                       LINK_LIBRARIES XYDataset PhzModeling TYPE Boost)
elements_add_unit_test(IntegrateDatasetFunctor_test tests/src/IntegrateDatasetFunctor_test.cpp
                       LINK_LIBRARIES PhzModeling TYPE Boost)