elements_subdir(PhzUITools)

elements_depends_on_subdirs(PhzDataModel)

find_package(Boost REQUIRED COMPONENTS serialization)


elements_add_library(PhzUITools src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost PhzDataModel
                  INCLUDE_DIRS Boost PhzDataModel
                  PUBLIC_HEADERS PhzUITools )

