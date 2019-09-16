elements_subdir(PhzCLI)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhzModeling)

find_package(Boost REQUIRED COMPONENTS program_options)
elements_add_library(PhzCLI src/lib/*.cpp
                     LINK_LIBRARIES PhzConfiguration ElementsKernel
                     PUBLIC_HEADERS PhzCLI)
elements_add_executable(PhosphorosLsAux src/program/LsAux.cpp
                     LINK_LIBRARIES ElementsKernel Boost XYDataset PhzCLI)

elements_alias(PhosphorosDisplayFilters PhosphorosLsAux --type Filters)
elements_alias(PhosphorosDisplaySeds PhosphorosLsAux --type SEDs)
elements_alias(PhosphorosDisplayReddeningCurves PhosphorosLsAux --type ReddeningCurves)
elements_add_executable(PhosphorosDisplayModelGrid src/program/DisplayModelGrid.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzCLI)
elements_add_executable(CreateFlatGridPrior src/program/CreateFlatGridPrior.cpp
                     LINK_LIBRARIES ElementsKernel PhzCLI)

elements_install_conf_files()

elements_install_python_modules()

elements_install_scripts()
elements_add_unit_test(LsAuxDirConfig_test tests/src/LsAuxDirConfig_test.cpp 
                     LINK_LIBRARIES PhzCLI
                     TYPE Boost)

elements_add_python_program(PhosphorosPlotPhotometryComparison PhzCLI.PhosphorosPlotPhotometryComparison)
elements_add_python_program(PhosphorosOrderSeds PhzCLI.OrderSeds)
elements_add_python_program(PhosphorosPlotPosterior PhzCLI.PlotPosterior)
