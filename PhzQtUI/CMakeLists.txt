elements_subdir(PhzQtUI)


elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzPhotometricCorrection)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzUITools)
elements_depends_on_subdirs(PhzLikelihood)

find_package(Qt4 REQUIRED QtCore QtGui QtXml)

elements_expand_sources(PhUI_SRCS src/lib/*.cpp)
elements_expand_sources(PhUI_HEADERS PhzQtUI/*.h)
elements_expand_sources(PhUI_FORMS auxdir/PhzQtUI/*.ui)
elements_expand_sources(PhUI_RESOURCES auxdir/PhzQtUI/*.qrc)


QT4_WRAP_CPP(PhUI_HEADERS_MOC ${PhUI_HEADERS})
QT4_WRAP_UI(PhUI_FORMS_HEADERS ${PhUI_FORMS})
QT4_ADD_RESOURCES(PhUI_RESOURCES_RCC ${PhUI_RESOURCES})

include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})



elements_add_library(PhzQtUI ${PhUI_SRCS} ${PhUI_HEADERS_MOC} ${PhUI_FORMS_HEADERS} ${PhUI_RESOURCES_RCC}
                     LINK_LIBRARIES ElementsKernel ${QT_LIBRARIES} XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling PhzDataModel PhzUITools PhzLikelihood
                     INCLUDE_DIRS ElementsKernel XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling PhzDataModel PhzUITools PhzLikelihood
                     PUBLIC_HEADERS PhzQtUI)

elements_add_executable(PhosphorosUI src/program/Main.cpp
                        LINK_LIBRARIES PhzQtUI XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling PhzDataModel PhzUITools PhzLikelihood)

