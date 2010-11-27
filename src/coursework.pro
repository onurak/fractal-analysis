# -------------------------------------------------
# Project created by QtCreator 2010-01-14T12:14:37
# -------------------------------------------------
TARGET = coursework
TEMPLATE = app
HEADERS += gui/MainWindow.h \
    axiomlib/src/MultiTS.h \
    fractallib/G/GVariant.h \
    fractallib/G/GSettings.h \
    fractallib/G/GResources.h \
    fractallib/G/GDelegateImplementation.h \
    fractallib/G/GDelegate.h \
    fractallib/G/GException.h \
    fractallib/G/GCollection.h \
    fractallib/G/GLogger.h \
    fractallib/G/GAbout.h \
    fractallib/G/GUnitTest.h \
    fractallib/Pattern.h \
    fractallib/flqt.h \
    fractallib/Environment.h \
    fractallib/Exceptions.h \
    fractallib/QtRender.h \
    fractallib/AxiomLib.h \
    fractallib/About.h \
    fractallib/markers/TSMarker.h \
    fractallib/markers/TSMarkerSimple.h \
    fractallib/markers/TSMarkerAxiomatic.h \
    fractallib/markers/TSMarkerFactory.h \
    fractallib/io/FileCSV.h \
    fractallib/io/FilePAT.h \
    fractallib/tools/spirit.h \
    fractallib/CoordSystem.h \
    fractallib/FL.h \
    fractallib/TimeSeries.h \
    fractallib/ParseTree.h \
    fractallib/ParseContext.h \
    fractallib/Watcher.h \
    fractallib/SynonymsTable.h \
    fractallib/parsers/TSParser.h \
    fractallib/parsers/TSParserSimple.h \
    fractallib/parsers/TSParserIdeal.h \
    fractallib/parsers/TSParserFactory.h \
    fractallib/ParseResult.h \
    fractallib/TSGenerator.h \
    fractallib/TSGeneratorElliott.h \
    fractallib/HyperTree.h \
    fractallib/PatternGuard.h \
    fractallib/PatternDescriptionEbnf.h \
    fractallib/PatternDescription.h \
    fractallib/PatternGuardRPN.h \
    fractallib/predicates/Min.h \
    fractallib/predicates/Max.h \
    fractallib/predicates/Value.h \
    fractallib/predicates/Previous.h \
    fractallib/predicates/ArgCnt.h \
    fractallib/predicates/All.h \
    fractallib/Logging.h \
    fractallib/MemoryManager.h \
    fractallib/ProjectSettings.h \    
    fractallib/ParseTreeMetric.h \
    fractallib/Predicates.h \
    gui/PatternEditor.h \
    gui/GuiCommon.h \
    gui/DebugWindow.h \
    gui/WorkerThreads.h \
    fractallib/predicates/Value.h \
    fractallib/predicates/Previous.h \
    fractallib/predicates/Min.h \
    fractallib/predicates/Max.h \
    fractallib/predicates/ArgCnt.h \
    fractallib/predicates/All.h \
    fractallib/predicates/PredicateFactory.h \
    fractallib/groupcheckers/GroupChecker.h \
    fractallib/groupcheckers/GroupCheckerUniversal.h \    
    fractallib/groupcheckers/GroupCheckerSingle.h \
    fractallib/groupcheckers/GroupCheckerFactory.h \
    gui/FLGraphicsView.h \
    fractallib/predicates/ExtractPrev.h \
    fractallib/predicates/Sum.h \
    fractallib/GuardCompilerRPN.h \
    fractallib/predicates/Sum.h \
    fractallib/predicates/ExtractPrev.h \
    gui/TreeFilter.h \
    fractallib/PatternParameter.h \
    fractallib/tsdatasource/TSDataSource.h \
    fractallib/tsdatasource/TSDataSourceFileCSV.h \
    fractallib/parsers/TSDynamicParser.h \
    fractallib/parsers/TSDynamicParserIdeal.h \
    gui/OpenDTSFromFileDialog.h
SOURCES += main.cpp \
    fractallib/io/FileCSV.cpp \
    fractallib/io/FilePAT.cpp \
    fractallib/markers/TSMarkerSimple.cpp \
    fractallib/markers/TSMarkerAxiomatic.cpp \
    fractallib/parsers/TSParserSimple.cpp \
    fractallib/parsers/TSParserIdeal.cpp \
    fractallib/G/GLogger.cpp \
    fractallib/Environment.cpp \
    fractallib/QtRender.cpp \
    fractallib/ParseTree.cpp \
    fractallib/Watcher.cpp \
    fractallib/TSGeneratorElliott.cpp \
    fractallib/TSGenerator.cpp \
    fractallib/ParseContext.cpp \
    fractallib/HyperTree.cpp \
    fractallib/Pattern.cpp \
    fractallib/PatternDescriptionEbnf.cpp \
    fractallib/PatternGuardRPN.cpp \
    fractallib/MemoryManager.cpp \
    gui/PatternEditor.cpp \
    gui/GuiCommon.cpp \
    gui/DebugWindow.cpp \
    gui/WorkerThreads.cpp \
    gui/MainWindow_FL_Logic.cpp \
    gui/MainWindow_GUI_Logic.cpp \
    gui/MainWindow.cpp \
    fractallib/predicates/PredicateFactory.cpp \
    fractallib/groupcheckers/GroupCheckerUniversal.cpp \
    fractallib/groupcheckers/GroupCheckerSingle.cpp \
    fractallib/GuardCompilerRPN.cpp \
    gui/TreeFilter.cpp \
    fractallib/tsdatasource/TSDataSourceFileCSV.cpp \
    fractallib/parsers/TSDynamicParserIdeal.cpp \
    gui/OpenDTSFromFileDialog.cpp
FORMS += gui/MainWindow.ui \
    gui/PatternEditor.ui \
    gui/DebugWindow.ui \
    gui/OpenDTSFromFileDialog.ui
RESOURCES += gui/resources.qrc

OTHER_FILES += \
    fractallib/license.txt
