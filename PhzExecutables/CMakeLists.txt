elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)

find_package(Boost REQUIRED COMPONENTS program_options)

elements_add_executable(CreatePhotometryGrid src/program/CreatePhotometryGrid.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)

elements_install_conf_files()