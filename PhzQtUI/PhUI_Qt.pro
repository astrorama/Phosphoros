#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T11:06:23
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhzQtUI_Qt
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
    src/lib/FormAnalysis.cpp \
    src/lib/PhotometricCorrectionHandler.cpp

HEADERS  += PhzQtUI/MainWindow.h \
    PhzQtUI/ModelSetTable.h \
    PhzQtUI/ModelSetModel.h \
    PhzQtUI/Range.h \
    PhzQtUI/ParameterRule.h \
    PhzQtUI/ModelSet.h \
    PhzQtUI/ParameterRuleModel.h \
    PhzQtUI/ParameterRuleTable.h \
    PhzQtUI/DialogModelSet.h \
    PhzQtUI/DirectoryTreeModel.h \
    PhzQtUI/DialogOptions.h \
    PhzQtUI/FilterMapping.h \
    PhzQtUI/SurveyFilterMapping.h \
    PhzQtUI/SurveyModel.h \
    PhzQtUI/FilterModel.h \
    PhzQtUI/DialogFilterMapping.h \
    PhzQtUI/DialogCreatesSubGroup.h \
    PhzQtUI/DialogImportAuxData.h \
    PhzQtUI/FileUtils.h \
    PhzQtUI/DialogPhotCorrectionEdition.h \
    PhzQtUI/DialogPhotometricCorrectionComputation.h \
    PhzQtUI/FormAuxDataManagement.h \
    PhzQtUI/FormSurveyMapping.h \
    PhzQtUI/FormModelSet.h \
    PhzQtUI/FormAnalysis.h \
    PhzQtUI/PhotometricCorrectionHandler.h

FORMS    += auxdir/PhzQtUI/MainWindow.ui \
    auxdir/PhzQtUI/DialogModelSet.ui \
    auxdir/PhzQtUI/DialogOptions.ui \
    auxdir/PhzQtUI/DialogFilterMapping.ui \
    auxdir/PhzQtUI/DialogCreatesSubGroup.ui \
    auxdir/PhzQtUI/DialogImportAuxData.ui \
    auxdir/PhzQtUI/DialogPhotCorrectionEdition.ui \
    auxdir/PhzQtUI/DialogPhotometricCorrectionComputation.ui \
    auxdir/PhzQtUI/FormAuxDataManagement.ui \
    auxdir/PhzQtUI/FormSurveyMapping.ui \
    auxdir/PhzQtUI/FormModelSet.ui \
    auxdir/PhzQtUI/FormAnalysis.ui

RESOURCES += \
    auxdir/PhzQtUI/PhUIResources.qrc

OTHER_FILES += \
    auxdir/PhzQtUI/logoPhUI.png
