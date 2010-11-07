#-------------------------------------------------
#
# Project created by QtCreator 2010-07-08T01:01:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = HyperTree
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    ./../../src/fractallib/HyperTree.h \
    ./../../src/fractallib/G/GUnitTest.h \
    ./../../src/fractallib/G/GLogger.h \
    ../../src/fractallib/About.h \
    ../../src/fractallib/Exceptions.h \
    HyperTreeTest.h


SOURCES += main.cpp \
    ./../../src/fractallib/HyperTree.cpp \
    ./../../src/fractallib/G/GLogger.cpp
