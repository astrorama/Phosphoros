elements_subdir(PhzOutput)

elements_depends_on_subdirs(PhzDataModel SourceCatalog GridContainer ElementsKernel)

#===== Libraries ===============================================================
elements_add_library(PhzOutput src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel Table SourceCatalog PhzDataModel
                     PUBLIC_HEADERS PhzOutput )