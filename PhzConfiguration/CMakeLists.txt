elements_subdir(PhzConfiguration 0.1)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzPhotometricCorrection)

elements_add_library(PhzConfiguration src/lib/*.cpp
                     LINK_LIBRARIES  XYDataset PhzDataModel PhzOutput PhzPhotometricCorrection
                     INCLUDE_DIRS XYDataset PhzDataModel
                     PUBLIC_HEADERS PhzConfiguration)

elements_add_unit_test(FilterConfiguration_test tests/src/FilterConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)

elements_add_unit_test(SedConfiguration_test tests/src/SedConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)

elements_add_unit_test(RedshiftConfiguration_test tests/src/RedshiftConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)

elements_add_unit_test(ReddeningConfiguration_test tests/src/ReddeningConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)

elements_add_unit_test(ParameterSpaceConfiguration_test tests/src/ParameterSpaceConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzConfiguration TYPE Boost)

elements_add_unit_test(CreatePhotometryGridConfiguration_test tests/src/CreatePhotometryGridConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration TYPE Boost)

elements_add_unit_test(PhotometryGridConfiguration_test tests/src/PhotometryGridConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration TYPE Boost)

elements_add_unit_test(CatalogConfiguration_test tests/src/CatalogConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration TYPE Boost)

elements_add_unit_test(SpectroscopicRedshiftCatalogConfiguration_test tests/src/SpectroscopicRedshiftCatalogConfiguration_test.cpp
                       LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration TYPE Boost)

elements_add_unit_test(CalculatePhotometricCorrectionConfiguration_test tests/src/CalculatePhotometricCorrectionConfiguration_test.cpp
                       LINK_LIBRARIES PhzDataModel PhzConfiguration TYPE Boost)