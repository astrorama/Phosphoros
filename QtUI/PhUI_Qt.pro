#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T11:06:23
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtUI_Qt
TEMPLATE = app

# enable C++ 11
QMAKE_CXXFLAGS += -std=c++0x


SOURCES += src/program/Main.cpp\
    src/lib/MainWindow.cpp \
    src/lib/ModelSetTable.cpp \
    src/lib/ModelSetModel.cpp \
    src/lib/Range.cpp \
    src/lib/ParameterRule.cpp \
    src/lib/ModelSet.cpp \
    src/lib/ParameteRruleModel.cpp \
    src/lib/ParameterRuleTable.cpp \
    src/lib/DialogModelSet.cpp \
    src/lib/DirectoryTreeModel.cpp \
    src/lib/DialogOptions.cpp \
    src/lib/FilterMapping.cpp \
    src/lib/SurveyFilterMapping.cpp \
    src/lib/SurveyModel.cpp \
    src/lib/FilterModel.cpp \
    src/lib/DialogFilterMapping.cpp \
    src/lib/DialogCreatesSubGroup.cpp \
    src/lib/DialogImportAuxData.cpp \
    src/lib/FileUtils.cpp \
    src/lib/DialogPhotCorrectionEdition.cpp \
    src/lib/DialogPhotometricCorrectionComputation.cpp \
    src/lib/FormAuxDataManagement.cpp \
    src/lib/FormSurveyMapping.cpp \
    src/lib/FormModelSet.cpp \
    src/lib/FormAnalysis.cpp 

HEADERS  += QtUI/MainWindow.h \
    QtUI/ModelSetTable.h \
    QtUI/ModelSetModel.h \
    QtUI/Range.h \
    QtUI/ParameterRule.h \
    QtUI/ModelSet.h \
    QtUI/ParameterRuleModel.h \
    QtUI/ParameterRuleTable.h \
    QtUI/DialogModelSet.h \
    QtUI/DirectoryTreeModel.h \
    QtUI/DialogOptions.h \
    QtUI/FilterMapping.h \
    QtUI/SurveyFilterMapping.h \
    QtUI/SurveyModel.h \
    QtUI/FilterModel.h \
    QtUI/DialogFilterMapping.h \
    QtUI/DialogCreatesSubGroup.h \
    QtUI/DialogImportAuxData.h \
    QtUI/FileUtils.h \
    QtUI/DialogPhotCorrectionEdition.h \
    QtUI/DialogPhotometricCorrectionComputation.h \
    QtUI/FormAuxDataManagement.h \
    QtUI/FormSurveyMapping.h \
    QtUI/FormModelSet.h \
    QtUI/FormAnalysis.h

FORMS    += auxdir/QtUI/MainWindow.ui \
    auxdir/QtUI/DialogModelSet.ui \
    auxdir/QtUI/DialogOptions.ui \
    auxdir/QtUI/DialogFilterMapping.ui \
    auxdir/QtUI/DialogCreatesSubGroup.ui \
    auxdir/QtUI/DialogImportAuxData.ui \
    auxdir/QtUI/DialogPhotCorrectionEdition.ui \
    auxdir/QtUI/DialogPhotometricCorrectionComputation.ui \
    auxdir/QtUI/FormAuxDataManagement.ui \
    auxdir/QtUI/FormSurveyMapping.ui \
    auxdir/QtUI/FormModelSet.ui \
    auxdir/QtUI/FormAnalysis.ui

RESOURCES += \
    auxdir/QtUI/PhUIResources.qrc

OTHER_FILES += \
    auxdir/QtUI/logoPhUI.png
