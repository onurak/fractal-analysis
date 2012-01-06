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
    gui/QParseTreeRender.cpp \
    fractallib/patterns/functions/Value.cpp \
    fractallib/patterns/functions/AtOneLine.cpp \
    fractallib/patterns/functions/StartTime.cpp \
    fractallib/patterns/functions/EndTime.cpp \
    fractallib/patterns/functions/Prev.cpp \
    fractallib/patterns/functions/Self.cpp \
    fractallib/patterns/functions/IsPrev.cpp \
    fractallib/patterns/functions/Near.cpp \
    fractallib/parsers/DynamicMultiPass.cpp \
    fractallib/parsers/AbstractDynamicParser.cpp \
    fractallib/trees/Metrics.cpp \
    fractallib/Forecast.cpp \
    fractallib/markers/SimpleIncremental.cpp \
    fractallib/patterns/standard/TreeMatcher.cpp \
    fractallib/markers/ABIncremental.cpp \
    fractallib/patterns/functions/CurNode.cpp \
    fractallib/patterns/functions/IsNode.cpp \
    fractallib/patterns/functions/Length.cpp

HEADERS  += gui/MainWindow.h \
    gui/QParseTreeRender.h \
    fractallib/patterns/functions/Value.h \
    fractallib/patterns/functions/AtOneLine.h \
    fractallib/patterns/functions/StartTime.h \
    fractallib/patterns/functions/EndTime.h \
    fractallib/patterns/functions/Prev.h \
    fractallib/patterns/functions/Self.h \
    fractallib/patterns/functions/IsPrev.h \
    fractallib/patterns/functions/Near.h \
    gui/Utils.h \
    fractallib/parsers/DynamicMultiPass.h \
    fractallib/parsers/AbstractDynamicParser.h \
    fractallib/trees/Metrics.h \
    fractallib/Forecast.h \
    fractallib/trees/metrics/Subtree.h \
    fractallib/trees/metrics/MaxLevelsCount.h \
    fractallib/trees/metrics/MaxTrees.h \
    fractallib/markers/SimpleIncremental.h \
    fractallib/patterns/DescriptionCompiler.h \
    fractallib/patterns/Matcher.h \
    fractallib/patterns/standard/TreeMatcher.h \
    fractallib/markers/ABIncremental.h \
    fractallib/patterns/functions/CurNode.h \
    fractallib/patterns/functions/IsNode.h \
    fractallib/patterns/functions/Length.h

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
    fractallib/patterns/Pattern.cpp \
    fractallib/patterns/Guard.cpp \
    fractallib/patterns/Description.cpp \
    fractallib/patterns/Context.cpp \
    fractallib/patterns/functions/Min.cpp \
    fractallib/patterns/functions/Max.cpp \
    fractallib/patterns/functions/FunctionFactory.cpp \
    fractallib/patterns/functions/Function.cpp \
    fractallib/patterns/standard/GuardRpn.cpp \
    fractallib/patterns/standard/EbnfRpnPattern.cpp \
    fractallib/patterns/standard/DescriptionCompilerEbnf.cpp \
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
    fractallib/patterns/Pattern.h \
    fractallib/patterns/Guard.h \
    fractallib/patterns/Description.h \
    fractallib/patterns/Context.h \
    fractallib/patterns/functions/Min.h \
    fractallib/patterns/functions/Max.h \
    fractallib/patterns/functions/FunctionFactory.h \
    fractallib/patterns/functions/Function.h \
    fractallib/patterns/standard/GuardRpn.h \
    fractallib/patterns/standard/EbnfRpnPattern.h \
    fractallib/patterns/standard/DescriptionCompilerEbnf.h \
    fractallib/trees/Tree.h \
    fractallib/trees/Node.h \
    fractallib/trees/Layer.h \
    fractallib/trees/Forest.h \
    fractallib/io/FilePAT.h \
    fractallib/patterns/CheckInfo.h

RESOURCES += \
    gui/resources.qrc
