#-------------------------------------------------
#
# Project created by QtCreator 2010-07-13T17:57:38
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = TSParser
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../src/fractallib/TSParserSimple.cpp \
    ../../src/fractallib/TSParserIdeal.cpp \
    ../../src/fractallib/TSMarkerSimple.cpp \
    ../../src/fractallib/TSMarkerAxiomatic.cpp \
    ../../src/fractallib/Poliz.cpp \
    ../../src/fractallib/Pattern.cpp \
    ../../src/fractallib/ParseTree.cpp \
    ../../src/fractallib/ParseContext.cpp \
    ../../src/fractallib/HyperTree.cpp \
    ../../src/fractallib/Guards.cpp \
    ../../src/fractallib/FilePAT.cpp \
    ../../src/fractallib/FileCSV.cpp \
    ../../src/fractallib/Environment.cpp \
    ../../src/fractallib/Ebnf.cpp

HEADERS += \
    ../../src/fractallib/TSParserSimple.h \
    ../../src/fractallib/TSParserIdeal.h \
    ../../src/fractallib/TSParser.h \
    ../../src/fractallib/TSMarkerFactory.h \
    ../../src/fractallib/TSMarkerAxiomatic.h \
    ../../src/fractallib/TSMarker.h \
    ../../src/fractallib/TimeSeries.h \
    ../../src/fractallib/Terms.h \
    ../../src/fractallib/SynonymsTable.h \
    ../../src/fractallib/QtRender.h \
    ../../src/fractallib/Poliz.h \
    ../../src/fractallib/Pattern.h \
    ../../src/fractallib/ParseTree.h \
    ../../src/fractallib/ParseResult.h \
    ../../src/fractallib/ParseContext.h \
    ../../src/fractallib/HyperTree.h \
    ../../src/fractallib/Guards.h \
    ../../src/fractallib/flqt.h \
    ../../src/fractallib/FL.h \
    ../../src/fractallib/FilePAT.h \
    ../../src/fractallib/FileCSV.h \
    ../../src/fractallib/Exceptions.h \
    ../../src/fractallib/Environment.h \
    ../../src/fractallib/Ebnf.h \
    ../../src/fractallib/CoordSystem.h \
    ../../src/fractallib/Collections.h \
    ../../src/fractallib/ClassFactory.h \
    ../../src/fractallib/AxiomLib.h \
    ../../src/fractallib/About.h \
    ../../src/fractallib/TSMarkerSimple.h \
    TSParserTest.h
