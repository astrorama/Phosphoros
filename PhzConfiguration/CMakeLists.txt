elements_subdir(PhzConfiguration 0.1)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)

find_package(Boost REQUIRED COMPONENTS program_options serialization)

elements_add_library(PhzConfiguration src/lib/*.cpp
                     LINK_LIBRARIES Boost XYDataset
                     INCLUDE_DIRS Boost XYDataset
                     PUBLIC_HEADERS PhzConfiguration)

elements_add_unit_test(FilterConfiguration_test tests/src/FilterConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)

elements_add_unit_test(SedConfiguration_test tests/src/FilterConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)
 