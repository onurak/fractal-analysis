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
#ifndef WATCHER_H
#define WATCHER_H

#include "Environment.h"
#include "Exceptions.h"
#include "io/FileCSV.h"
#include "io/FilePAT.h"
#include "markers/TSMarkerFactory.h"
#include "parsers/TSParserFactory.h"
#include "groupcheckers/GroupCheckerFactory.h"
#include "TimeSeries.h"
#include "TSGenerator.h"
#include "TSGeneratorElliott.h"
#include "G/GDelegate.h"


namespace FL {

/*! \class Watcher
  * \brief Class for easy usage of fractal library
  *
  * First you can setup some options using
  * bool setOption(const std::string &option, const GVariant &value) method.
  * Available options:
  *     - threading_mode: hint how multithreading should be used. Possible values:
  *         - none:   don't use it
  *         - single: use it, but only run library in separate thread if it possible
  *         - multi:  use as many threads as it will be need
  *     - safe_mode: must watcher catch all FL exception
  *         - yes: catch all exceptions, return result as result of functions
  *                (true on success, false otherwise)
  *         - no:  don't catch
  *     - marker: type of markup algorithm (class name or alias). Depends on available
  *               marker set. There is always exists "TSMarkerSimple" (or just "abmarker")
  *     - parser: type of parse algorithm (class name or alias). Depends on available
  *               parser set. There is always exists "TSParserSimple" (or just "simple")
  *     - group_checker: type of group checking algorithm (class name or alias).
  *               Depends on available cherckers set. There is always exists
  *               "GroupCheckerSingle" (or just "single") and "GroupCheckerUniversal"
  *               (or just "universal");
  *
  *     - patterns_file: file to load patterns
  *     - time_series_file: file to load time series
  *     - debug: if its true than markers and parser will ran in debug mode
  *
  * Than you can use markup and parse methods:
  * markup, parseLevel, parseAll
  */
class Watcher
{
public:
    //! Default constructor
    /*! \param env Environment to load settings or null
      * \sa loadFromEnv
      */
    Watcher(Environment *env = NULL);

    //! Destructor
    virtual ~Watcher();

    //! Markup time series with terminals
    /*! Run marker specified with "marker" option. Previous parse trees will be deleted
      * \return true in case of success, false otherwise. If safe_mode option is
      * disabled, than in case of error exception might be thrown
      */
    bool markup();

    //! Parse to nearest figure. Not implemented yet.
    bool parseFigure(Pattern **t = NULL);

    //! Parse one fractal level
    /*! If time series isn't marked up than parsing will fail
      * \return true in case of success, false otherwise. If safe_mode option is
      * disabled, than in case of error exception might be thrown
      */
    bool parseLevel(ParseResult *result = NULL);

    //! Parse all fractal levels
    /*! If time series isn't marked up than parsing will fail
      * \return true in case of success, false otherwise. If safe_mode option is
      * disabled, than in case of error exception might be thrown
      */
    bool parseAll(int *levelsCount = NULL);

    //! Markup time series and parse all fractal levels
    /*! Same as markup() && parseAll(levelsCount).
      * \return true in case of success, false otherwise. If safe_mode option is
      * disabled, than in case of error exception might be thrown
      */
    bool analyse(int *levelsCount = NULL);

    //! Load all settings from environment
    /*! Settings that are not exists in new environment will not be reset.
      *
      * \param env Environment to load
      */
    void loadFromEnv(Environment &env);

    //! Save settings to environment
    /*! Settings will be saved under FL section
      * \param env Environment to save
      */
    void saveToEnv(Environment &env)
    {
        env.appendFrom(m_options, true);
    }

    //! Return value of specified option
    /*! \param option Option name
      */
    GVariant option(std::string option)
    {
        return m_options[SECTION][option];
    }

    //! Set value for specified option
    /*! List of available options can be found in documentation for class
      * \param option Option name
      * \param value Option value
      * \return true true if option set successful, false otherwise.
      *  Error can be caused if option value is unsupported, or option name is unknown.
      */
    bool setOption(const std::string &option, const GVariant &value);

    //! Return current analysing time series
    /*! It can be set by setTimeSeries(TimeSeries *value) method or by setting
      * time_series_file option
      */
    TimeSeries* timeSeries()
    {
        return m_timeSeries;
    }

    //! Set analysing time series. Previous time series will be deleted.
    void setTimeSeries(TimeSeries *value)
    {
        if (m_timeSeries)
            delete m_timeSeries;
        m_timeSeries = value;
        m_parseTreeSet.clear();
    }

    //! Return current parse tree set
    ParseTreeSet& parseTreeSet()
    {
        return m_parseTreeSet;
    }

    //! Return current marker or NULL
    TSMarker* marker()
    {
        return m_marker;
    }

    //! Return current parser or NULL
    TSParser* parser()
    {
        return m_parser;
    }

    //! Return used pattern set
    PatternCollection& patterns()
    {
        return m_patterns;
    }

    //! Return last exception message (if safe_mode option is enabled)
    std::string lastExceptionMsg()
    {
        return m_lastExceptionMsg;
    }

    //! Get time series generator
    TSGenerator* generator()
    {
        return m_generator;
    }

    //! Get known markers list
    const std::vector<std::string>& markerList()
    {
        return m_markerList;
    }

    //! Get known parsers list
    const std::vector<std::string>& parserList()
    {
        return m_parserList;
    }
private:
    //! Current time series
    TimeSeries *m_timeSeries;

    //! Current parse tree set
    ParseTreeSet m_parseTreeSet;

    //! Current patterns set
    PatternCollection m_patterns;

    //! Current marker
    TSMarker *m_marker;

    //! Current parser
    TSParser *m_parser;

    //! Curent group checker
    Patterns::GroupChecker *m_groupChecker;

    //! Settings
    Environment m_options;

    //! Last exception message
    std::string m_lastExceptionMsg;

    //! Time series generator
    TSGenerator *m_generator;

    //! List of known markers
    std::vector<std::string> m_markerList;

    //! List of known parsers
    std::vector<std::string> m_parserList;

    //! List of known group checkers
    std::vector<std::string> m_groupCheckersList;

    //! Default section name in settings
    std::string SECTION;

};

} // namespace

#endif // WATCHER_H
