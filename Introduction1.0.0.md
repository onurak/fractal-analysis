# Introduction #

**Fractal Library** (**FL**) is a library that is designed for pattern recognition in the structure of time series. Patterns have specific fractal structure which will be described later. This software is the result of coursework "Analysis of Fractal Structure of Time Series" and this Introdution is unofficial document and can be regarded as quick start to understand basic principles of library.

The basic idea of analysis is to transform time series into a sequence of symbols (called _terminals_) and analysis of this sequence. This terminals are represents some simple time series behaviour patterns (like "moving up" or "moving down").
The analysis process consists of several stages:
  1. Markup: convert time series to the sequence of terminals.
  1. Preprocessing: make preprocessing on terminals sequence.
  1. Parse: recognize fractal patterns in sequence of terminals and build fractal _Parse Tree_.
Next comes description of each step. This manual use **FLAnalyzer** - GUI frontend for FL. As an example, some patterns of well-known in economics [Elliott Wave Principle](http://en.wikipedia.org/wiki/Elliott_wave_principle) will be used. The exact pattern set for Elliott Waves is another objective of this study.

# Details #

## Two words about the FLAnalyser ##
**FLAnalyzer** is the GUI frontend to FL.
It's main window could looks like this:

![http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/main_window.png](http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/main_window.png)

Currently the program continues to be developed so its appearance may change.
To load time series use **File->Open time series**. Only Comma Separated Values (CSV) file format is supported now.
To demonstrate work of application file "LKOH\_101001\_101016.csv" from standard examples set will be used.

## Markup stage ##
At this step it is necessary to convert time series to sequence of **terminals** - non-fractal recognizable patterns. Depending on marker algorithm different set of terminals available.

At this moment only the simplest marker algorithm realized (it's called **AB**). It marks each simple segment (_simple segment_ means two adjacent points) of time series with terminal _a_ if this segment is increases (ordinate of the first point of segment is less than or equals to second's one), or with terminal _b_ otherwise. It means that there are two terminals: _a_ (segment moving up) and _b_ (segment moving down). All fractal patterns can be expressed using only this two terminals.

Let's see how it is look like. To set Marker use **Properties** window and choose **AB** in **Marker** combo box and then run markup algorithm using **Analyze->Markup** menu item. All segments of the time series will be highlighted with the terminals it classified as it shown on picture:

![http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/marked_up.png](http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/marked_up.png)

## Preprocessing stage ##
The next step is to preprocess to find some more complicated patterns.
First pattern's format will be described.

### Fractal Patterns ###
Some info about _fractals_ you can find in [Wikipedia](http://en.wikipedia.org/wiki/Fractal). For simplicity, we assume that the **fractal** is  structure that
  1. has a recursive defenition and
  1. has a self-similarity feature.

**Fractal pattern** (or **nonterminals**) is the structure element that consist of terminals and nonterminals with given constraints.

In the way to express single pattern first it's structure must be defined which in terms of FL called **Description**. It is a symbols (terminals and nonterminals) sequence. Every time analyzer will recognize given sequence it will check given constraints of this symbols.

This constraints is the second part of pattern and its called **Guard function** or just **Guard**. Guard is the boolean expression that returns _true_ if symbols (may be some subset of symbols) of Description are satisfy to given constraints, and _false_ otherwise. If guard returns _true_ than symbols sequence marking with proper pattern of higher level. Example is follow.

Assuming we have two terminals _a_ and _b_ from previous Markup stage.
Fractal pattern that have sematics "big upmoving - few adjacent simple segments of time series where each is increases" could be described as follow:

> {{{up = a\_1 | up\_2 up\_3 @
> > a\_1: true,
> > up**_: true;

> PATTERNS:
> > up = a | up up @;
> > dn = b | dn dn @;

> PATTERNS:
    1. Raising impulse
> > ImpulseUP = up\_1   dn\_2   up\_3   dn\_4   up\_5 @
> > up\_3: max(up\_3) > max(up\_1),
> > up\_5: max(up\_5) > max(up\_3);_

  1. Lowering impulse

> ImpulseDN = dn\_1  up\_2  dn\_3  up\_4  dn\_5 @
> dn\_3: min(dn\_3) < min(dn\_1),
> dn\_5: min(dn\_5) < min(dn\_3);**

  1. Lowering zigzag correction
> ZigZagDN = dn\_1 up dn @:
> dn\_1: isprev(ImpulseUP);

  1. Raising correction
> ZigZagUP = up\_1 dn up @:
> up\_1: isprev(ImpulseDN);
  1. Lowering impulse
> > ImpulseDN = dn\_1  up\_2  dn\_3  up\_4  dn\_5   |  **ImpulseDN  ZigZagUP  ImpulseDN  ZigZagUP  ImpulseDN** @
> > dn\_3: min(dn\_3) < min(dn\_1),
> > dn\_5: min(dn\_5) < min(dn\_3);
 }}}

The left side of equality is the _pattern's name_ - it must be alpha-numeric expression started with alpha. Right side before _@_ symbol is the _pattern's description_ writen in subset of Extended Bacus-Naur Form (EBNF). Limitations of EBNF will be described later. This string means that _up_ pattern can consists of _a_ terminal or from two adjacent _up_ nonterminals and if analyzer will find single _a_ or double _up_ than they will be prerecognized as _up_ pattern.

The last part (after _@_ symbol) is the _pattern guard_. Guards are boolean expression that can include
 # boolean and numeric constants: _true_, _false_, _3.1415_;
 # boolean operations: _and_, _or_, _not_;
 # relations: ==, !=, >, <, >=, <=;
 # math operations: *, /, + -;
 # functions.

Guards could be defined for each symbol of pattern's description. To match guard to symbol of EBNF, each symbol have index (number after symbol name like up*_2*). Each guard expression consists of two parts:
 # selector wich describes type of EBNF symbol that must be checked by this guard. For example, a_1 will be applied only for symbol a_1, up_* will be applied to both up_2 and up_3  symbols, *_* will be checked for each symbol.
 # guard rule - everything after colon. Example: length(up_3) > length(up_2) and value(self()) < value(prev()). This guard means that length of time series segment marked with up_3 symbol must be greater then length of time series segment marked up with up_2 symbols. Also it have second part after _and_ wich means that value (projection to Y axis) of currently recognizing pattern must be less then projection of previously recognized pattern.

There is set of available functions applicable to symbols. Examples of functions are 'min(symbol)' and 'max(symbol)' which returns minimum and maximum of segment of time series that corresponds to pattern, 'isPrev(symbol)' which return _true_ only if previously recognized pattern was 'symbol' and so on. In out example guard _true_ means that it always passes. Complete patterns format descrption can be found in documentation to FL and will be available in wiki later.

Let's see how it looks like in practice. 

== Continue preprocessing stage ==
Our preprocessing will find all sections of the monotonicity of time series and merge them to single *up* or *dn* terminals.
Create new patterns file (e.g. Mono.pat) and type:
	{{{
	}}}

Now select this file in FLAnalyzer in "Preprocessing patterns" window and call Analyze->Markup. Preprocessing will be applied automatically after markup stage so the main window should look like this:

[http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/preprocessed.png]


== Parse stage ==
The next step is to recognize fractal patterns in the given terminals sequence.
Currently there are two Parsers available:
 * *SinglePass* is the fast singlepass parser. It algorithm is:
   # Start from begining of sequence;
   # While not reach last symbol of sequence:
     * Look for first pattern applicable in current position. If it exists then mark corresponding nodes with this pattern and go to position just
       after this nodes. Otherwise move to one position;
   This parser can find only one possible sequence of symbols but it works fast.
 * *MultiPass* which will find all sequences of patterns that can be recognized, but it very slow and can be used only for small sequences. This parser will be described later.

Here is the example patterns set. It describes simplest Elliott Wave's Impulse-Correction model.
	{{{
	}}}
 
We will use *SinglePass* parser (it must be choosed in *Properties* window). In "Patterns" windows open file with patterns bellow. Choose *Analyze->Build trees*. Recognized patterns will be added to time series view and segments of time series highlighted with green are the top-level patterns (it's a roots of parse tree). Complete recognized structure is on the screenshot:

[http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/parsed.png]

Patterns of different heights are creates _levels_ of fractal. As you can see nodes of zero level is the markup terminals, while the first level symbols are recognized patterns. There could be more levels: for example, if we modify second pattern from patterns set to
	{{{
	}}}
	
then parser will recognize more of fractal levels:

[http://fractal-analysis.googlecode.com/svn/wiki/images/1.0.0/parsed2.png]

Also note that there are _unfinished_ patterns recognized - patterns that are not yet completed, but could be if future points of time series will have behaviour described by pattern's guards.

There are could be more than one combination of patterns placement in structure of time series. To find all posible combinations, *MultiPass* parser could be used. Choose *MultiPass* in "Static Parser" combo box and perform same markup -> build actions sequence. This will return 8 combinations of patterns for demo time series - each can be viewed by switching parse tree index in bottom of the analysis window.

```