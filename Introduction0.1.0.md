# Introduction #

**Fractal Library** (**FL**) is a library that is designed for pattern recognition in the structure of time series. Patterns have specific fractal structure which will be described later. This software is the result of coursework "Analysis of Fractal Structure of Time Series" and this Introdution is unofficial document and can be regarded as quick start to understand basic principles of library.

The basic idea of analysis is to transform time series into a sequence of symbols (called _terminals_) and analysis of this sequence. This terminals are represents some simple time series behaviour patterns (like "moving up" or "moving down").
The analysis process consists of several stages:
  1. Markup: convert time series to the sequence of terminals.
  1. Parse: recognize fractal patterns in sequence of terminals and build fractal _Parse Tree_.
Next comes description of each step. This manual use **FLAnalyzer** - GUI frontend for FL. As an example, some patterns of well-known in economics [Elliott Wave Principle](http://en.wikipedia.org/wiki/Elliott_wave_principle) will be used. The exact pattern set for Elliott Waves is another objective of this study.

# Details #

## Two words about the FLAnalyser ##
**FLAnalyzer** is the GUI frontend to FL. Complete documentation will be created later.
It's main window could looks like this:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/main_window.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/main_window.png)

Currently the program continues to be developed so its appearance may change.
To load time series use **File->Open time series**. Only Comma Separated Values (CSV) file format is supported now.

## Markup stage ##
At this step it is necessary to convert time series to sequence of **terminals** - non-fractal recognizable patterns. Depending on marker algorithm different set of terminals available.

At this moment only the simplest marker algorithm realized (it's also called **simple**). It marks each simple segment (_simple segment_ means two adjacent points) of time series with terminal _a_ if this segment is increases (ordinate of the first point of segment is less than or equals to second's one), or with terminal _b_ otherwise. It means that there are two terminals: _a_ (segment moving up) and _b_ (segment moving down). All fractal patterns can be expressed using only this two terminals.

Let's see how it is look like. To set Marker use **Properties** window and choose _abmarker_ in **Marker** combo box and then run markup algorithm using **Analysis->Next level** menu item. All segments of the time series will be highlighted with the terminals it classified as it shown on picture:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/marked_up.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/marked_up.png)

> Note: if you see another picture you might change drawing options in **Properties** window under section **Drawing**.

## Parse stage ##
Next step is to recognize fractal patterns in the given terminals sequence. First pattern's format will be described.

### Fractal Patterns ###
Some info about _fractals_ you can find in [Wikipedia](http://en.wikipedia.org/wiki/Fractal). For simplicity, we assume that the **fractal** is  structure that
  1. has a recursive defenition and
  1. has a self-similarity feature.

**Fractal pattern** (or **nonterminals**) is the structure element that consist of terminals and nonterminals with given constraints.

In the way to express single pattern first it's structure must be defined which in terms of FL called **Description**. It is a symbols (terminals and nonterminals) sequence. Every time analyzer will recognize given sequence it will check given constraints of this symbols.

This constraints is the second part of pattern and its called **Guard function** or just **Guard**. Guard is the boolean expression that returns _true_ if symbols (may be some subset of symbols) of Description are satisfy to given constraints, and _false_ otherwise. If after analyzer's guard checking it returns _true_ than symbols sequence marking with proper pattern of higher level. Example is follow.

Assuming we have two terminals _a_ and _b_ from previous Markup stage.
Fractal pattern that have sematics "big upmoving - few adjacent simple segments of time series where each is increases" could be described as follow:

> `up = a | up up @ true;`

The left side of equality is the _pattern's name_ - it must be alpha-numeric expression started with alpha. Right side before _@_ symbol is the _pattern's description_ writen in subset of Extended Bacus-Naur Form (EBNF). Limitations of EBNF will be described later. This string means that _up_ pattern can consists of _a_ terminal or from two adjacent _up_ nonterminals and if analyzer will find single _a_ or double _up_ than they will be prerecognized as _up_ pattern.

The last part (after _@_ symbol) is the _pattern guard_. Guards are boolean expression that can include
  1. boolean and numeric constants: _true_, _false_, _3.1415_;
  1. boolean operations: _and_, _or_, _not_;
  1. relations: ==, !=, >, <, >=, <=;
  1. math operations: **, /, + -;
  1. functions (that for hystorical reasons called _predicates_);
  1. if expression: if condition then code [<another code>](else.md). Note that it's not statement, it's expression which return result and can be mixed with math and bool expressions.**

There is set of available functions applicable to symbols. Examples of functions are 'min(symbol)' and 'max(symbol)' which returns minimum and maximum of segment of time series that corresponds to pattern, 'previous(symbol)' which return _true_ only if previously recognized pattern was 'symbol' and so on. In future guard's description format will become richer to allow more complicated constraints (e.g. simple arithmetics and conditionals). In out example guard _true_ means that it always passes. Complete patterns format descrption can be found in documentation to FL and will be available in wiki later.

Let's see how it looks like in practice.

### Parsing process ###
Patterns must be put in special PAT-files. Assuming we have one (e.g. C:/temp/example.pat) with single pattern:

```
 PATTERNS:
   up = a @ true;
   up = up up @ true;
```

It's really single pattern, but die to current limitations of file format all alternatives must be writen in few rules.
This file can be loaded in FLAnalyser using **File->Open patterns**. In the picture below shown example time series:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/ts_example.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/ts_example.png)

First time series will be marked up using _abmarker_ as it shown in previous parts:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/ab_marked.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/ab_marked.png)

The next step is the fractal pattern recognition. Currently there are two Parsers available:
  * **simple** is the fast singlepass parser. It algorithm is:
    1. Start from begining of sequence;
    1. While not reach last symbol of sequence:
      * Look for first pattern applicable in current position. If it exists then mark corresponding nodes with this pattern and go to position just
> > > > after this nodes. Otherwise move to one position;

> > This parser can find only one possible sequence of symbols but it works fast.
  * **ideal** which will find all sequences of patterns that can be recognized, but it very slow and can be used only for small sequences. This parser will be described later.

We will use _simple_ parser (it must be choosed in **Properties** swindow). Choose **Analysis->Next level** menu few time until message "Last level parsed". Each time you parsing level, next _fractal level_ is recognizing. New recognized patterns will be added to time series view and segments of time series highlighted with green are the top-level patterns (it's a roots of parse tree). Every time analyzer recognize _a_ or _up up_ sequences they are marks as _up_ pattern of upper level. Complete recognized structure is on the screenshot:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/simple_parsed.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/simple_parsed.png)

Patterns of different heights are creates _levels_ of fractal. As you can see nodes of zero level is the markup terminals. Next level is all patterns like `up = a @ true` - it is the only structure that parser could recognize. And patterns of second and third levels are like `up = up up @ true`. In fact this is a tree structure is kind of [AST](http://en.wikipedia.org/wiki/Abstract_syntax_tree). That is not correct AST because there are more than one root (roots highlighted with green). In more usual form it could be displayed as:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/ast_example.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/ast_example.png)

These ASTs in terms of FL called **Parse trees**. The one of the main goal of library is to parse all parse trees for given time series and patterns set.

## Elliott Waves example ##

As it were promised simple [Elliott Waves](http://en.wikipedia.org/wiki/Elliott_wave_principle) (EW) example will follow. There is no complete formalisation of EW and one of the objective of research in building good fractal patterns set of this theory. Current (alpha) version of waves will follow soon.

As you can read in e.g. [Wikipedia](http://en.wikipedia.org/wiki/Elliott_wave_principle), the basis of EW is the only four patterns: two impulses (raising and lowering) and two corrections (raising and lowering):

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/elliott_base.jpg](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/elliott_base.jpg)

To extract this structures from real time series two set of patterns will be used:
  * Patterns for time series smoothing:
```
 PATTERNS:
   up = a @ true;
   up = up up @ true;
   down = b @ true;
   down = down down @ true;
```
  * Elliott Waves example patterns:
```
 PATTERNS:
   # Raising impulse
   imup = up_1   down_1   up_2   down_2   up_3 @ 
       min(up_2) > min(up_1) and min(up_3) > min(up_2) and max(up_3) > max(up_2);
	 
   # Lowering impulse
   imdown = down_1  up_1  down_2  up_2  down_3 @
       max(down_2) < max(down_1) and max(down_3) < max(down_2) and min(down_3) < min(down_2);

   # Lowering correction
   crdown = down up down @ previous(imup) == 1;
 
   # Raising correction
   crup = up down up @ previous(imdown) == 1;
```

Sure this is a very poor pattern set and they can't be used for real analysis or trading. But even then example demostrate applicability of algorithm.

In the way to "smooth" demonstrative time series all its fractal levels will be extracted using first patterns set and _simple_ parser:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/smoothed_ts.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/smoothed_ts.png)

Then using second patterns set and _ideal_ parser all possible combinations of this patterns (in example time series there are 14) will be extracted. There are lot of "garbage" combinations because of poor pattern set, but few other combinations looks pretty well:

![http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/not_the_worst.png](http://fractal-analysis.googlecode.com/svn/wiki/images/0.1.0/not_the_worst.png)
