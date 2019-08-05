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
elements_depends_on_subdirs(EmissionLines)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE src/lib/DialogZRanges.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()


find_package(Qt5 COMPONENTS Core Concurrent Gui Widgets Xml REQUIRED)

elements_expand_sources(PhUI_SRCS src/lib/*.cpp)
elements_expand_sources(PhUI_HEADERS PhzQtUI/*.h)
elements_expand_sources(PhUI_FORMS auxdir/PhzQtUI/*.ui)
elements_expand_sources(PhUI_RESOURCES auxdir/PhzQtUI/*.qrc)

qt5_wrap_cpp(PhUI_HEADERS_MOC ${PhUI_HEADERS} OPTIONS -nn)
qt5_wrap_ui(PhUI_FORMS_HEADERS ${PhUI_FORMS})
qt5_add_resources(PhUI_RESOURCES_RCC ${PhUI_RESOURCES})

add_definitions(
        ${Qt5Core_DEFINITIONS} ${Qt5Concurrent_DEFINITIONS}
        ${Qt5Gui_DEFINITIONS} ${Qt5Widgets_DEFINITIONS}
        ${Qt5Xml_DEFINITIONS})

elements_add_library(PhzQtUI ${PhUI_SRCS} ${PhUI_HEADERS_MOC} ${PhUI_FORMS_HEADERS} ${PhUI_RESOURCES_RCC}
                     LINK_LIBRARIES
                        ElementsKernel XYDataset PhzConfiguration PhzPhotometricCorrection
                        PhzModeling PhzDataModel PhzUITools PhzLikelihood PhzLuminosity PhzUtils PhzExecutables
                        Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml
                     INCLUDE_DIRS
                        ElementsKernel XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling
                        PhzDataModel PhzUITools PhzLikelihood PhzLuminosity
                        "${Qt5Core_INCLUDE_DIRS}" "${Qt5Concurrent_INCLUDE_DIRS}"
                        "${Qt5Gui_INCLUDE_DIRS}" "${Qt5Widgets_INCLUDE_DIRS}"
                        "${Qt5Xml_INCLUDE_DIRS}"
                     PUBLIC_HEADERS PhzQtUI)

elements_add_executable(PhosphorosUI src/program/Main.cpp
                        LINK_LIBRARIES PhzQtUI XYDataset PhzConfiguration PhzPhotometricCorrection PhzModeling PhzDataModel PhzUITools PhzLikelihood PhzLuminosity)


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


