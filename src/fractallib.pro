#-------------------------------------------------
#
# Project created by QtCreator 2011-02-11T23:19:20
#
#-------------------------------------------------

QT       -= core gui

TARGET = fractallib
TEMPLATE = lib
CONFIG += staticlib


HEADERS  += \
    fractallib/TimeSeries.h \
    fractallib/patterns/Pattern.h \
    fractallib/patterns/Context.h \
    fractallib/trees/Tree.h \
    fractallib/trees/Node.h \
    fractallib/trees/Layer.h \
    fractallib/trees/Forest.h \
    fractallib/patterns/Guard.h \
    fractallib/patterns/Description.h \
    fractallib/parsers/SinglePass.h \
    fractallib/parsers/AbstractParser.h \
    fractallib/parsers/MultiPass.h \
    fractallib/markers/AbstractMarker.h \
    fractallib/markers/AB.h \
    fractallib/markers/Axiomatic.h \
    fractallib/patterns/functions/Function.h \
    fractallib/patterns/functions/Min.h \
    fractallib/patterns/functions/Max.h \
    fractallib/compilers/AbstractCompiler.h \
    fractallib/Common.h \
    fractallib/FL.h \
    fractallib/patterns/standart/DescriptionEbnf.h \
    fractallib/patterns/standart/GuardRpn.h \
    fractallib/patterns/standart/EbnfRpnPattern.h \
    fractallib/exceptions/EException.h \
    fractallib/io/FileCSV.h \
    fractallib/patterns/functions/FunctionFactory.h \
    fractallib/patterns/SynonymsTable.h \
    G/GVariant.h \
    G/GUnitTest.h \
    G/GSettings.h \
    G/GResources.h \
    G/GLogger.h \
    G/GException.h \
    G/GDelegateImplementation.h \
    G/GDelegate.h \
    G/GCollection.h \
    G/GAbout.h
    

SOURCES += \
    G/GLogger.cpp \
    fractallib/TimeSeries.cpp \
    fractallib/patterns/Pattern.cpp \
    fractallib/patterns/Context.cpp \
    fractallib/trees/Tree.cpp \
    fractallib/trees/Node.cpp \
    fractallib/trees/Layer.cpp \
    fractallib/trees/Forest.cpp \
    fractallib/patterns/Guard.cpp \
    fractallib/patterns/Description.cpp \
    fractallib/parsers/SinglePass.cpp \
    fractallib/parsers/AbstractParser.cpp \
    fractallib/parsers/MultiPass.cpp \
    fractallib/markers/AbstractMarker.cpp \
    fractallib/markers/AB.cpp \
    fractallib/markers/Axiomatic.cpp \
    fractallib/patterns/functions/Function.cpp \
    fractallib/patterns/functions/Min.cpp \
    fractallib/patterns/functions/Max.cpp \
    fractallib/compilers/AbstractCompiler.cpp \
    fractallib/Common.cpp \
    fractallib/patterns/standart/DescriptionEbnf.cpp \
    fractallib/patterns/standart/GuardRpn.cpp \
    fractallib/patterns/standart/EbnfRpnPattern.cpp \
    fractallib/exceptions/EException.cpp \
    fractallib/io/FileCSV.cpp \
    fractallib/patterns/functions/FunctionFactory.cpp \
    fractallib/patterns/SynonymsTable.cpp
