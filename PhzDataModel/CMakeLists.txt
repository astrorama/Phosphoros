elements_subdir(PhzDataModel 0.1)

elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(GridContainer)
elements_depends_on_subdirs(XYDataset)

find_package(Boost REQUIRED COMPONENTS serialization)

#===== Libraries ===============================================================
elements_add_library(PhzDataModel src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost XYDataset
                  INCLUDE_DIRS Boost XYDataset PhzDataModel GridContainer
                  PUBLIC_HEADERS PhzDataModel )
                  
                  
#===== Boost tests =============================================================

elements_add_unit_test(PhzModel_test tests/src/PhzModel_test.cpp
                       LINK_LIBRARIES PhzDataModel TYPE Boost)