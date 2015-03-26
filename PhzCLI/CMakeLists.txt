elements_subdir(PhzCLI)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhzModeling)

find_package(Boost REQUIRED COMPONENTS program_options)

elements_add_library(PhzCLI src/lib/*.cpp
                     LINK_LIBRARIES  PhzConfiguration
                     PUBLIC_HEADERS PhzCLI)

elements_add_executable(PhosphorosLsAux src/program/LsAux.cpp
                        LINK_LIBRARIES ElementsKernel Boost XYDataset)

elements_add_executable(PhosphorosDisplayTemplates src/program/DisplayTemplates.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzCLI)

elements_add_executable(PhosphorosDisplayDataset src/program/DisplayDataset.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration MathUtils PhzModeling)

elements_install_conf_files()

elements_install_python_modules()

elements_install_scripts()