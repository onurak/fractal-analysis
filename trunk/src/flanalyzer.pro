#-------------------------------------------------
#
# Project created by QtCreator 2011-03-14T00:28:59
#
#-------------------------------------------------

QT       += core gui

TARGET = flanalyzer
TEMPLATE = app


SOURCES += gui/main.cpp\
        gui/MainWindow.cpp \
    gui/QParseTreeRender.cpp

HEADERS  += gui/MainWindow.h \
    gui/QParseTreeRender.h

FORMS    += gui/MainWindow.ui


SOURCES += \
    fractallib/TimeSeries.cpp \
    fractallib/Common.cpp \
    fractallib/compilers/AbstractCompiler.cpp \
    fractallib/exceptions/EException.cpp \
    fractallib/io/FileCSV.cpp \
    fractallib/markers/Axiomatic.cpp \
    fractallib/markers/AbstractMarker.cpp \
    fractallib/markers/AB.cpp \
    fractallib/parsers/SinglePass.cpp \
    fractallib/parsers/MultiPass.cpp \
    fractallib/parsers/AbstractParser.cpp \
    fractallib/patterns/SynonymsTable.cpp \
    fractallib/patterns/Pattern.cpp \
    fractallib/patterns/Guard.cpp \
    fractallib/patterns/Description.cpp \
    fractallib/patterns/Context.cpp \
    fractallib/patterns/functions/Min.cpp \
    fractallib/patterns/functions/Max.cpp \
    fractallib/patterns/functions/FunctionFactory.cpp \
    fractallib/patterns/functions/Function.cpp \
    fractallib/patterns/standart/GuardRpn.cpp \
    fractallib/patterns/standart/EbnfRpnPattern.cpp \
    fractallib/patterns/standart/DescriptionEbnf.cpp \
    fractallib/trees/Tree.cpp \
    fractallib/trees/Node.cpp \
    fractallib/trees/Layer.cpp \
    fractallib/trees/Forest.cpp \
    fractallib/io/FilePAT.cpp \
    G/GVariables.cpp

HEADERS += \
    fractallib/FL.h \
    G/GVariant.h \
    G/GUnitTest.h \
    G/GSettings.h \
    G/GResources.h \
    G/GLogger.h \
    G/GException.h \
    G/GDelegateImplementation.h \
    G/GDelegate.h \
    G/GCollection.h \
    G/GAbout.h \
    console/Testing.h \
    fractallib/TimeSeries.h \
    fractallib/FL.h \
    fractallib/Common.h \
    fractallib/compilers/AbstractCompiler.h \
    fractallib/exceptions/EException.h \
    fractallib/io/FileCSV.h \
    fractallib/markers/AbstractMarker.h \
    fractallib/markers/Axiomatic.h \
    fractallib/markers/AB.h \
    fractallib/parsers/AbstractParser.h \
    fractallib/parsers/SinglePass.h \
    fractallib/parsers/MultiPass.h \
    fractallib/patterns/SynonymsTable.h \
    fractallib/patterns/Pattern.h \
    fractallib/patterns/Guard.h \
    fractallib/patterns/Description.h \
    fractallib/patterns/Context.h \
    fractallib/patterns/functions/Min.h \
    fractallib/patterns/functions/Max.h \
    fractallib/patterns/functions/FunctionFactory.h \
    fractallib/patterns/functions/Function.h \
    fractallib/patterns/standart/GuardRpn.h \
    fractallib/patterns/standart/EbnfRpnPattern.h \
    fractallib/patterns/standart/DescriptionEbnf.h \
    fractallib/trees/Tree.h \
    fractallib/trees/Node.h \
    fractallib/trees/Layer.h \
    fractallib/trees/Forest.h \
    fractallib/io/FilePAT.h \
    fractallib/patterns/CheckInfo.h

RESOURCES += \
    gui/resources.qrc
