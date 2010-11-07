#-------------------------------------------------
#
# Project created by QtCreator 2010-07-09T16:49:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Base
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../src/fractallib/Poliz.cpp \
    ../../src/fractallib/ParseContext.cpp \
    ../../src/fractallib/HyperTree.cpp \
    ../../src/fractallib/Guards.cpp \
    ../../src/fractallib/FileCSV.cpp \
    ../../src/fractallib/Environment.cpp \
    ../../src/fractallib/Ebnf.cpp \
    ../../src/fractallib/ParseTree.cpp \
    ../../src/fractallib/Pattern.cpp \
    ../../src/fractallib/G/GLogger.cpp \
    ../../src/fractallib/FilePAT.cpp

HEADERS += \
    BaseTest.h \
    ../../src/fractallib/G/GUnitTest.h \
    ../../src/fractallib/TimeSeries.h \
    ../../src/fractallib/Terms.h \
    ../../src/fractallib/SynonymsTable.h \
    ../../src/fractallib/Poliz.h \
    ../../src/fractallib/ParseContext.h \
    ../../src/fractallib/HyperTree.h \
    ../../src/fractallib/Guards.h \
    ../../src/fractallib/FL.h \
    ../../src/fractallib/FileCSV.h \
    ../../src/fractallib/Exceptions.h \
    ../../src/fractallib/Environment.h \
    ../../src/fractallib/Ebnf.h \
    ../../src/fractallib/AxiomLib.h \
    ../../src/fractallib/About.h \
    ../../src/fractallib/ParseResult.h \
    ../../src/fractallib/ParseTree.h \
    ../../src/fractallib/Pattern.h \
    ../../src/fractallib/G/GVariant.h \
    ../../src/fractallib/G/GSettings.h \
    ../../src/fractallib/G/GResources.h \
    ../../src/fractallib/G/GLogger.h \
    ../../src/fractallib/G/GException.h \
    ../../src/fractallib/G/GDelegateImplementation.h \
    ../../src/fractallib/G/GDelegate.h \
    ../../src/fractallib/G/GCollection.h \
    ../../src/fractallib/G/GAbout.h \
    ../../src/axiomlib/src/MultiTS.h \
    ../../src/fractallib/FilePAT.h
