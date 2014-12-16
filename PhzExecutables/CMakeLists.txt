elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzOutput)

find_package(Boost REQUIRED COMPONENTS program_options)

elements_add_executable(CreatePhotometryGrid src/program/CreatePhotometryGrid.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)

elements_add_executable(CreatePhzCatalog src/program/CreatePhzCatalog.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzLikelihood PhzOutput)
                        

elements_install_conf_files()