elements_subdir(PhzUITools)

elements_depends_on_subdirs(Table)
elements_depends_on_subdirs(PhzDataModel)

find_package(CCfits)

find_package(Boost REQUIRED COMPONENTS serialization)


elements_add_library(PhzUITools src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost Table PhzDataModel CCfits
                  INCLUDE_DIRS Boost Table PhzDataModel CCfits
                  PUBLIC_HEADERS PhzUITools )

