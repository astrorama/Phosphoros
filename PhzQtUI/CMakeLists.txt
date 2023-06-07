elements_subdir(PhzQtUI)


elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzPhotometricCorrection)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzUITools)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzLuminosity)
elements_depends_on_subdirs(PhysicsUtils)
elements_depends_on_subdirs(PhzExecutables)
elements_depends_on_subdirs(PhzGalacticCorrection)
elements_depends_on_subdirs(PhzFilterVariation)
elements_depends_on_subdirs(EmissionLines)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE src/lib/DialogZRanges.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()


find_package(Qt6 REQUIRED COMPONENTS Core Network Gui Widgets Concurrent Xml)

elements_expand_sources(PhUI_SRCS src/lib/*.cpp)
elements_expand_sources(PhUI_HEADERS PhzQtUI/*.h)
elements_expand_sources(PhUI_FORMS auxdir/PhzQtUI/*.ui)
elements_expand_sources(PhUI_RESOURCES auxdir/PhzQtUI/*.qrc)


set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/auxdir/PhzQtUI)

qt_standard_project_setup() 

add_definitions(
        ${Qt6Core_DEFINITIONS} ${QtConcurrent_DEFINITIONS}
        ${Qt6Gui_DEFINITIONS} ${QtWidgets_DEFINITIONS}
        ${Qt6Xml_DEFINITIONS}  ${QtNetwork_DEFINITIONS})

elements_add_library(PhzQtUI ${PhUI_SRCS} ${PhUI_HEADERS_MOC} ${PhUI_FORMS_HEADERS} ${PhUI_RESOURCES_RCC}
                     LINK_LIBRARIES
                        ElementsKernel XYDataset PhzConfiguration PhzPhotometricCorrection
                        PhzModeling PhzDataModel PhzUITools PhzLikelihood PhzLuminosity PhzUtils PhzGalacticCorrection PhzFilterVariation PhzExecutables
                        Qt6::Core Qt6::Network Qt6::Gui Qt6::Widgets Qt6::Xml Qt6::Concurrent
                     INCLUDE_DIRS
                        ElementsKernel XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling
                        PhzDataModel PhzUITools PhzLikelihood PhzLuminosity PhzGalacticCorrection PhzFilterVariation
                        "${QtCore_INCLUDE_DIRS}" "${QtConcurrent_INCLUDE_DIRS}"
                        "${QtNetwork_INCLUDE_DIRS}"
                        "${QtGui_INCLUDE_DIRS}" "${QtWidgets_INCLUDE_DIRS}"
                        "${QtXml_INCLUDE_DIRS}"
       
                     PUBLIC_HEADERS PhzQtUI)

qt_add_resources(PhzQtUI "images" PREFIX "/" BASE ${CMAKE_CURRENT_SOURCE_DIR}/auxdir/PhzQtUI/ FILES ${CMAKE_CURRENT_SOURCE_DIR}/auxdir/PhzQtUI/logoPhUI.png)

elements_add_executable(PhosphorosUI src/program/Main.cpp
                        LINK_LIBRARIES PhzQtUI XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling PhzDataModel PhzUITools PhzLikelihood PhzLuminosity PhzGalacticCorrection PhzFilterVariation)


elements_add_unit_test(QtUI_FileUtils_test tests/src/QtUI_FileUtils_test.cpp
                       LINK_LIBRARIES PhzQtUI TYPE Boost)

elements_add_unit_test(Range_test tests/src/Range_test.cpp
                       LINK_LIBRARIES PhzQtUI TYPE Boost)
elements_add_unit_test(ParameterRule_test tests/src/ParameterRule_test.cpp
                       LINK_LIBRARIES PhzQtUI TYPE Boost)
elements_add_unit_test(ModelSet_test tests/src/ModelSet_test.cpp
                       LINK_LIBRARIES PhzQtUI TYPE Boost)


elements_add_unit_test(FilterMapping_test tests/src/FilterMapping_test.cpp
                       LINK_LIBRARIES PhzQtUI TYPE Boost)


