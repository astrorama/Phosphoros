elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzPhotometricCorrection)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(MathUtils)

find_package(Boost REQUIRED COMPONENTS program_options)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE src/lib/SpectroscopicRedshiftCatalogConfiguration.cpp
                 PROPERTY COMPILE_FLAGS -Wno-virtual-move-assign)
    set_property(SOURCE src/lib/CalculatePhotometricCorrectionConfiguration.cpp
                 PROPERTY COMPILE_FLAGS -Wno-virtual-move-assign)
    set_property(SOURCE tests/src/CalculatePhotometricCorrectionConfiguration_test.cpp
                 PROPERTY COMPILE_FLAGS -Wno-virtual-move-assign)
    set_property(SOURCE src/program/CalculatePhotometricCorrection.cpp
                 PROPERTY COMPILE_FLAGS -Wno-virtual-move-assign)
  endif()
endif()

elements_add_executable(CreatePhotometryGrid src/program/CreatePhotometryGrid.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)

elements_add_executable(CreatePhzCatalog src/program/CreatePhzCatalog.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzLikelihood PhzOutput)

elements_add_executable(CalculatePhotometricCorrection src/program/CalculatePhotometricCorrection.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzPhotometricCorrection)

elements_add_executable(LsPhotometryGrid src/program/LsPhotometryGrid.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration)

elements_add_executable(LsData src/program/LsData.cpp
                        LINK_LIBRARIES ElementsKernel Boost XYDataset)

elements_add_executable(PrintModelDataset src/program/PrintModelDataset.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration MathUtils PhzModeling)
                        
elements_install_conf_files()