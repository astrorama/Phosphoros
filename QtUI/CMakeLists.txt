elements_subdir(QtUI)

find_package(Qt4 REQUIRED QtCore QtGui QtXml)

elements_expand_sources(PhUI_SRCS src/lib/*.cpp)
elements_expand_sources(PhUI_HEADERS QtUI/*.h)
elements_expand_sources(PhUI_FORMS auxdir/QtUI/*.ui)
elements_expand_sources(PhUI_RESOURCES auxdir/QtUI/*.qrc)


QT4_WRAP_CPP(PhUI_HEADERS_MOC ${PhUI_HEADERS})
QT4_WRAP_UI(PhUI_FORMS_HEADERS ${PhUI_FORMS})
QT4_ADD_RESOURCES(PhUI_RESOURCES_RCC ${PhUI_RESOURCES})

include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})



elements_add_library(PhUI ${PhUI_SRCS} ${PhUI_HEADERS_MOC} ${PhUI_FORMS_HEADERS} ${PhUI_RESOURCES_RCC}
                     LINK_LIBRARIES ElementsKernel ${QT_LIBRARIES}
                     INCLUDE_DIRS ElementsKernel
                     PUBLIC_HEADERS QtUI)
                     
elements_add_executable(PhosphorosUI src/program/Main.cpp
                        LINK_LIBRARIES QtUI)


