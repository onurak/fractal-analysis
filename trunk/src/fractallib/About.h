/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ABOUT_H
#define ABOUT_H

/*! \mainpage Documentation of Fractal Library и G Library
  *
  * Fractal Library contains classes for analysis of the fractal structure of time series.
  * Its documentation grouped in \ref FractalLibrary.
  *
  * G Library contains some utils used in Fractal Library. It is independent of
  * Fractal Library and can be used separately. Details can be found in file GAbout.h
  * and in group \ref GLib
  *
  *
  * You can read:
  * - \subpage quickstart
  * - \subpage history
  * - \subpage todolist
  * - \ref FL::Pattern "Description of patterns file format"
  * - \ref FractalLibrary "Documentation of classes of Fractal Library"
  * - \subpage gabout
  */

/*! \page quickstart Quick Start
  *
  * <i> This page must be rewrited </i>
  * \section ap Analysis process
  * Here common analysis process scheme:
  * \image html analysis_process.png
  *
  * <ol>
  * <li> <h2>Loading patterns</h2>
  *
  * %Pattern holds possible fractal structure of time series.
  * It consists of to main parts: structure part, wich describe symbolic structure of pattern,
  * and guard part, wich describe constraints of structure's elements.
  * Detail description of patterns can be found in documentation to FL::Pattern class.
  *
  * %Patterns are usually stored in PAT-files (format of file and class for its loading described
  * in FL::FilePAT). Loaded pattern stored in FL::Pattern class. Few patterns must be collected
  * in special collection class - FL::PatternsCollection.
  * Code snippet for loading all %patterns from PAT-file could look like this:
  * \code
  * FL::PatternCollection patterns;
  * FL::FilePAT pat;
  * if (!pat.loadFromFile(fileName, &patterns))
  *     throw "Can't load from file";
  * \endcode
  *
  * <li>  <h2>Loading time series</h2>
  *
  * Time series is incapsulated in FL::TimeSeries class.
  * It can be loaded from comma separated value (CSV) file using class FL::FileCSV.
  * Since CSV-file usually storing few data columns, user must choose one.
  * Code snippet for loading time series from CsV-file:
  * \code
  * FL::TimeSeries ts;
  * FL::FileCSV csv;
  * csv.loadFromFile(fileName);
  * int column = ChooseColumnWithData(csv.header());
  * csv.loadTimeSeries(&ts, column);
  * \endcode
  *
  * Now analysis can began. Our purpose is to build some parse trees of the time series.
  * Parse tree is a tree that holds information about fractal structure of time series.
  * It builds a correlation between segments of time series and fractal patterns,
  * wich possibly were used to generate this segments on various levels of fractal.
  * Parse tree represented as FL::ParseTree class and set of parse trees is FL::ParseTreeSet.
  *
  * <li> <h2>Markup time series with terminals</h2>
  *
  * First time series must be marked with terminals. In this step we will move from time series
  * to its symbolic representation.
  * Class FL::TSMarker is responded for this.
  * It's abstract and can't be used itself, instead its children should be.
  * Different children represents different marking algorithms e.g. FL::TSMarkerAxiomatic
  * for axiomatic markup algorithm or FL::TSMarkerSimple for up-down marker.
  * To run markup algorithm method FL::TSMarker::analyse(FL::TimeSeries *ts, ParseTreeSet &trees)
  * should be used. Code snippet:
  * \code
  * FL::ParseTreeSet trees;
  * FL::TSMarker *marker = FL::TSMarkerSimple();
  * marker.analyse(timeSeries, trees); // now trees[0] contain parse tree with markup
  * \endcode
  * Parse tree set will be cleared before analysis and resulting markup will be writen instead.
  *
  *
  * <li> <h2>Parsing fractal structure</h2>
  *
  * Now fractal structure can be recognized. Fractal parser should be used for this purpose.
  * It represented as abstract class FL::TSParser and its children, wich realize different
  * parse algorithms (e.g. FL::TSParserSimple for simplest algorithm, FL::TSParserIdeal for
  * slow, simple and full algorithm, FL::TSParserGLR for GLR parsing algorithm).
  * Method
  * \code
  * FL::ParseResult FL::TSParser::parse(FL::ParseTreeSet &trees, FL::PatternCollection &pattern, FL::TimeSeries *ts)
  * \endcode
  * should be used to build single fractal level for all parse trees in set. There are
  * more than one tree could be recognized, so result is union of old tree set (where
  * all trees are reconstructed with new fractal level (if it's exists for concreet tree))
  * and new trees. For better memory usage and fast tree coping hyper trees were used
  * (see FL::HyperTree).
  *
  * Code snippet:
  * \code
  * FL::TSParser *parser = new FL::TSParserIdeal();
  * while ( parser.parse(trees, patterns, ts).parsedCount > 0 )
  *     ;
  * // now trees contains all parse trees recognized by parser
  * \endcode
  *
  * </ol>
  * That is all.
  * Detail info about algorithms can be found in documentation to classes its represented.
  */

/*! \page history History
  * Notation: [+] - added, [!] - changed, [-] - removed, [.] - just comment
  *
  * First SVN release was at 13.04.10.
  *
  * 13.04.10:
  *  - [+] compilation and execution patterns (their EBNF and guards).
  *    EBNF representation - AST (see Pattern, EBNF::Ebnf),
  *    Guard representation - RPN program (see Guard, POLIZ::Poliz)
  *  - [+] representation of analysis results as AST FL::ParseTree
  *  - [+] classes for loading and storing time series data: FL::FileCSV и FL::TimeSeries
  *  - [+] first time series markup algorithm in FL::TSMarkerSimple
  *  - [+] base class for parse tree building: FL::TSParser
  *  - [+] first GUI based on Qt, but "debug only" - glutched, but working. Class for drawing QtRender
  *  - [.] документация. Пока еще не слишком хорошо структурирована, но достаточно полная и полезная
  *  - release-компиляция под Windows, хотя проект прекрасно компилируется под Unix-like (опробовано на Kubuntu Linux)
  *  - [.] для парсинга файлов используется header-only библиотека boost::spirit::qi из boost 1.42.
  *    Она не включена в набор исходников
  *  - [.] Для компиляции GUI необходима библиотека Qt, а также ее утилиты qmake и moc
  *
  * 15.04.10:
  * - [+] добавлены опции EBNF::Ebnf::CheckOptions для указания дополнительных опций проверки принадлежности
  *   выражения РБНФ, из за чего изменился интерфейс EBNF::Ebnf::check(Expression*, CheckOptions);
  * - [!] теперь EBNF::Ebnf при парсинге также назначает индексы элементам в соответствии с шаблоном.
  *   Например: последовательность a b b, распарсенная шаблоном t = a_1 (c_1 | b_1 b_2) пронумеруется как
  *   a_1 b_1 b_2. Индексы записываются непосредственно в узлы EBNF::Expression
  * - [!] исправлены некоторые ошибки в итераторах BaseIterator
  * - [+] добавлена функция ParseTreeNode::asString() в основном для отладки
  * - [+] начата работа над простейшим парсером разметки TSParserSimple
  *
  * 16.04.10:
  * - [!] закончена часть парсера TSParserSimple. Теперь он умеет парсить в дерево, но не видит неоднозначностей
  *   и в целом пока имеет много багов
  *
  * 17.04.10:
  * - [-] убраны коллекции Collections.h, заменены на std::list
  * - [!] доделывается TSParserSimple
  * - [!] тестируются и дорабатываются стражи
  *
  * 18.04.10:
  * - [!] стражи работают, появились первые ФС argcnt, min, max, previous, value
  *
  * 21.04.10:
  * - [+] added class Watcher, redraw();
  *
  * 28.04.10:
  * - [!] Classes TSParser and TSParserSimple now wroking with ParseTreeSet, not with single ParseTree
  *
  * 11.05.10:
  * - [+] начат новый парсер TSParserIdeal - универсальный парсер, который может вытащить абсолютно все
  * возможные деревья разбора за ОЧЕНЬ долгое время (верятно, экспоненциальное).
  * - [+] для удобства добавлены фабрики классов TSParserFactory и TSMarkerFactory
  * - [!] изменен способ передачи параметров функциям в процессе парсинга - теперь все что можно хранится в
  * ParseContext
  * - [!] множество мелких изменений: слегка изменен метод передачи параметров и результатов для TSMarker
  * и TSParser, изменения в интерфейсе, подогнан под изменения класс Watcher и др.
  *
  * 13.05.10:
  * - [!] working on TSParserIdeal
  *
  * 15.05.10:
  * - [!] TSParserIdeal finished!
  *
  * 07.07.10:
  * - [.] Begin of summer work. TODO: translate documentation to english, create convenient
  *   system for storing trees instead of TreeSet (see HyperTree), write GLR parser,
  *   write unit tests, debug new and old code. Sorry for my english.
  * - [+] Start writing smart storage for multiple trees with instersections
  *   (see HyperTree)
  * - [+] First unit testing classes (see GUnitTest, HyperTreeUnitTest) :)
  *
  * 08.07.10:
  * - [!] Working on HyperTree. Added Tree and its subclasses.
  *
  * 09.07.10:
  * - [!] HyperTree and its tests finished
  * - [!] ParseTree modified - now it's child of HyperTree. Chages in ParseTree touch
  *   almost all classes of library, so they must be slightly rewrited.
  * - [!] Start adapting classes for new ParseTree
  * - [!] Much documentation translated
  *
  * 10.07.10:
  * - [!] Name of Template class changed to Pattern, also FileTPL now known as FilePAT,
  *   and default pattern file extension is .PAT now.
  * - [!] Continue adapting for new parse tree
  * - [!] Translation...
  *
  * - [!] TSMarker and its children rebuilded for new ParseTree, finished test TSMarkerTest
  * - [!] TSParser and its children rebuilded for new ParseTree, almost finished  test TSParserTest
  * - [!] More translations
  *
  * 15.08.10:
  * - [.] I've spent month in hospital :(
  * - [+] Begin big modification of Pattern architecture. Now pattern consists of
  *   two main parts: FL::Patterns::Structure and FL::Patterns::Guard. This classes
  *   are abstract, they only define interface. First class store symbolic structure of
  *   pattern and provide methods for checking if some symbols sequence have such structure.
  *   Second provide methods for guard checking. Depending on specific child class
  *   realisation, Structure can be EBNF or can have some other format
  *   (like RegExp or some fuzzy logic parser). Same idea with Guard.
  * - [!] Completely recreated GUI. It's now over yet. There are many major changes
  * - [!] Few bugs fixed in GVariant, GVariant::toString() added
  * - [!] Docs translated
  *
  * 17.09.10:
  * - [+] Much documentation added, \ref quickstart page rewriten.
  * - [.] Architecture rethought.
  *
  * 19.09.10:
  * - [+] Added FL::Patterns::Description, FL::Patterns::DescriptionStructure,
  *   FL::Patterns::DescriptionChecker, FL::Patterns::DescriptionCompiler, FL::Patterns::Guard,
  *   FL::Patterns::GuardStructure, FL::Patterns::GuardChecker, FL::Patterns::GuardCompiler
  * - [!] Now FL::Pattern consists of FL::Patterns::Description and FL::Patterns::Guard
  * - [!] Now Ebnf is EbnfStructure, inherited from FL::Patterns::Description. Also EBNF::Compiler
  *   now child of FL::Patterns::DescriptionCompiler
  * - [!] Same with RPN guards
  * - [!] Namespace Poliz is now known as RPN
  *
  * 21.09.10:
  * - [!] Refactoring code for new class structure
  * - [!] Now Terms are in Terms.cpp, not in Terms.h
  *
  * 22.09.10:
  * - [.] Yeah, it finally can be compiled again!
  * - [!] Term is not a function anymore - it's a class now.
  * - [!] New pattern for FL::Terms::TermFactory used: terms adding themselves to factory, factory
  *   don't have to know about all existing terms at compile time. Don't use terms classes directly -
  *   use TermFactory instead.
  * - [+] Real terms (Min, Max, Value etc.) are in folder fractallib/terms now.
  *   They are subclasses of FL::Terms::Term class.
  * - [-] Terms.cpp
  *
  * 24.09.10:
  * - [+] Small GUI changes (Info box, Settings dialog)
  * - [!] Making ideal parser work again
  *
  * 25.09.10:
  * - [!] Ideal parser works correctly again
  * - [!] Rewriten FL::ParseTree::compare()
  * - [+] Added FL::ParseTree::hasGroup()
  * - [.] Ideal parser works, but it's still too slow
  *
  * 28.09.10:
  * - [!] Small changes and fixes
  *
  * 04.10.10:
  * - [!] Working on new realization of QtRender
  * - [!] FL::FileCSV class fixed
  *
  * 06.10.10:
  * - [!] QtRender completely rewriten
  *
  * 07.10.10:
  * - [!] "using namespace std" at global scope removed (it takes hours!)
  * - [!] FL::Environment rewriten. Now it holds standart INI files
  *
  * 10.10.10:
  * - [!] A lot of small changes, big memory error fixed
  * - [+] Begin working on debug-work-mode
  *
  * 11.10.10:
  * - [!] Big logic error: node's parents in hyper tree are same in different trees 'cause
  *   this nodes are identical in fact. There are must be lot of changes before it'll work again,
  *   so previous verison of ParseTree will be temporary used.
  *
  * 12.10.10:
  * - [!] FL::ParseTree completely removed with old version. To switch between simple parse tree
  *   code and HyperTree-base parse trees macro FL_ENABLE_PARSE_TREE_OPTIMIZATION can be used.
  *   New advanced structure is need to store such a difficult structure as parse tree set.
  *
  * 14.10.10:
  * - [!] Changes in QtRender. Now it supports horizontal scrolling and TreeDrawingSettings::DRAW_TIMESERIES
  * option
  *
  * 15.10.10:
  * - [!] FL::QtRender::DRAW_ROOTS mode implemented - render can highlight roots of tree
  * - [!] Small changes in PAT-files structure (symbols that don't have index don't need
  *   underscope symbol '_' to be followed after - e.g. "up" instead of "up_").
  *   So all example PAT-files are rewriten
  * - [!] Now EbnfChecker and EbnfStructure are different classes
  * - [!] Now GuardCheckerRPN and GuardStructureRPN are different classes
  * - [+] New good idea - computational scripts. What is that can be founded in
  *   documentation to new class FL::Script (soon). Experiments show that it is the
  *   way to analyzer applicable to real time series.
  *
  * 31.10.10:
  * - [.] All files are under GPLv3 license
  * - [.] New site: http://fractal-analysis.googlecode.com
  * - [!] "Term" is now known as "Predicate"
  * - [!] Changes in PAT files grammar: it more structured now
  *
  * 01.10.10:
  * - [!] Completely changed RPN guard's grammar
  * - [!] GVariant now support math operations for booleans
  *
  * 03.10.10:
  * - [!] Due to problems with boost::spirit::qi GuardCompilerRPN was rewriten
  *   using recursive descend parser. Looks like it finnaly works correctly in
  *   any situations and also compilation time reduced. It would be good idea
  *   never use boost::spirit::qi.
  *
  * 05.10.10:
  * - [!] FilePAT grammar rewriten using recursive descend
  * - [!] Difficult problem with RPN program: Indexed Names (like 'a_2') are passing
  *     as two arguments ('a' and '2') now. It means that we can't write like
  *     'max( extractprev(im, a_2) )' because compiler will hard encode argument
  *     count of 'max' function as 1, but 'extractprev' returns Indexed Name
  *     (two arguments).
  *     Solution:
  *       - Predicates return single GVariant again, not vector<GVariant>
  *       - GVariant custom variant type support added, so IndexedName class
  *         can be passed using single GVariant.
  *
  */

/*! \page todolist TODO List
  * Notation: [*] - done, [!] - active task, [+] - future task,
  * [-] - not actual task (e.g. classes used in task are not exists anymore)
  *
  * - [*] Complete guards (file format and its parser)
  * - [*] Replace AbstractParser with boost::spirit::qi
  * - [*] Remove functionality of Pattern::check() and Ebnf::check() to new class
  * - [!] Remove/recreate some classes of G (GDataSet -> std::map, GVariant -> boost::variant). May be completely remove G
  * - [!] Translate documentation to english
  * - [!] Rewrite all boost::spirit::qi parsers (they are temporary now)
  * - [!] Some new classes are don't properly documented
  * - [+] Write TSMarkerAxiomatic
  * - [+] Ebnf не воспринимает правильно шабоны вида "[a_] a_", не парсится выражение "a", только "a a".
  *       Связано с тем, что опциональный символ парсится верно, а второй затем натыкается на пустую последовательность.
  * - [+] Нет нормального способа задавать стражей для альтернатив. Приходится писать два отдельных правила с
  *   одинаковыми именами.
  * - [+] ускорить выполнение стражей. Идеи: ленивые вычисления, статические вычисления, не перевычислять
  *   одинаковые выражения
  * - [+] Write dumping for HyperTree
  * - [+] Write GLR parser
  * - [-] Remove Collections.h
  * - [-] Union Pattern and ParsedSequence classes
  */


  /* Technical elements (cretion of groups of comments etc.) */
  /*! \defgroup FractalLibrary Fractal Library
    *
    * \defgroup FLAlgorithm Algorithms and functions
    * \ingroup FractalLibrary
    *
    * \defgroup FLExceptions Excaptions
    * \ingroup FractalLibrary
    *
    * \defgroup FLFiles Supported file formats
    * \ingroup FractalLibrary
    *
    * \defgroup FLGUI GUI
    * \ingroup FractalLibrary
    *
    * \defgroup FLUtils Utilities
    * \ingroup FractalLibrary
    *
    * \defgroup FLTesting Testing
    * \ingroup FractalLibrary
    */

#endif // ABOUT_H
