elements_subdir(PhzPhotometricCorrection 0.1)

elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(GridContainer)
elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhzDataModel)

find_package(Boost REQUIRED COMPONENTS serialization)

#===== Libraries ===============================================================
elements_add_library(PhzPhotometricCorrection src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost SourceCatalog XYDataset MathUtils PhzDataModel
                  INCLUDE_DIRS Boost SourceCatalog XYDataset MathUtils PhzDataModel GridContainer
                  PUBLIC_HEADERS PhzPhotometricCorrection )

#===== Boost tests =============================================================