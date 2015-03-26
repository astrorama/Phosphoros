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

elements_add_executable(PhosphorosDisplayTemplate src/program/DisplayTemplate.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzCLI)

elements_add_executable(PrintModelDataset src/program/PrintModelDataset.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration MathUtils PhzModeling)