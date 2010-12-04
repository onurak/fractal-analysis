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
#include "Watcher.h"

using namespace FL;

#define SAFE_MODE_SECTION(code) \
    if (m_options[SECTION]["safe_mode"]) \
    { \
        try \
            { \
                code \
            } \
        catch (...) \
        { \
            m_lastExceptionMsg = "Unknown error"; \
            return false; \
        } \
    } \
    else \
    { \
       code \
    } \

Watcher::Watcher(Environment *env)
    : SECTION("FL")
{
    logg.info("FL::Watcher created");
    m_timeSeries = NULL;
    m_marker = NULL;
    m_parser = NULL;
    m_generator = new TSGeneratorElliott();
    if (env)
        loadFromEnv(*env);

    m_markerList.push_back("abmarker");

    m_parserList.push_back("simple");
    m_parserList.push_back("ideal");

    m_groupCheckersList.push_back("single");
    m_groupCheckersList.push_back("universal");
}

Watcher::~Watcher()
{
    if (m_marker)
        delete m_marker;
    if (m_parser)
        delete m_parser;
    m_parseTreeSet.clear();
    if (m_timeSeries)
        delete m_timeSeries;
    if (m_generator)
        delete m_generator;
    logg.info("FL::Watcher destroyed");
}

bool Watcher::markup(int tsBegin, int tsEnd)
{
    if (m_timeSeries == NULL)
    {
        m_lastExceptionMsg = "No time series loaded";
        return false;
    }
    if (m_marker == NULL || m_timeSeries == NULL)
    {
        m_lastExceptionMsg = "No marker set";
        return false;
    }

    clearParseTreesSegment(tsBegin, tsEnd);
    SAFE_MODE_SECTION
            (
                m_marker->analyse(m_timeSeries, m_parseTreeSet, tsBegin, tsEnd);
                return true;
            )
}

bool Watcher::parseFigure(Pattern **t)
{
    return false;
}

bool Watcher::parseLevel(ParseResult *result)
{
    SAFE_MODE_SECTION
        (
            ParseResult res = m_parser->parse(
                    m_parseTreeSet,
                    m_patterns,
                    m_timeSeries,
                    *m_groupChecker,
                    m_options["SECTION"]["debug"] ? TSParser::WorkDebug : TSParser::WorkNormal);
            if (result) *result = res;
            return true;
        )
}

bool Watcher::parseAll(int *levelsCount)
{
    if (m_timeSeries == NULL)
    {
        m_lastExceptionMsg = "No time series loaded";
        return false;
    }
    if (m_parser == NULL)
    {
        m_lastExceptionMsg = "No parser set";
        return false;
    }

    int lvlCount = 0;
    SAFE_MODE_SECTION
        (
            while (m_parser->parse(
                    m_parseTreeSet,
                    m_patterns,
                    m_timeSeries,
                    *m_groupChecker).parsedCount > 0)
                ++lvlCount;
            if (levelsCount)
                *levelsCount = lvlCount;
            return true;
        )
}

bool Watcher::analyse(int *levelsCount)
{
    return markup() && parseAll(levelsCount);
}

void Watcher::clearParseTreesSegment(int tsBegin, int tsEnd)
{
    if (tsEnd == -1)
        tsEnd = m_timeSeries->size()-1;
    ParseTreeSet::Iterator tree;
    for_each_(tree, m_parseTreeSet.trees)
        (*tree)->clearSegment(tsBegin, tsEnd);
}

bool Watcher::setOption(const std::string &option, const GVariant &value)
{
    m_options[SECTION][option] = value;
    if (option == "marker")
    {
        if (!TSMarkerFactory::create(value, &m_marker))
            return false;
    }
    else if (option == "parser")
    {
        if (!TSParserFactory::create(value, &m_parser))
            return false;
    }
    else if (option == "patterns_file")
    {
        SAFE_MODE_SECTION
            (
                m_patterns.clear();
                FilePAT file;
                if (!file.loadFromFile(value, m_patterns))
                {
                    m_lastExceptionMsg = file.lastErrorDescription();
                    m_patterns.clear();
                    return false;
                }
                return true;
            )
    }
    else if (option == "time_series_file")
    {
        return false;
    }
    else if (option == "safe_mode")
    {
    }
    else if (option == "threading_mode")
    {
    }
    else if (option == "group_checker")
    {
        if (!Patterns::GroupCheckerFactory::create(value, &m_groupChecker))
            return false;
    }
    else if (option == "debug")
    {
    }
    else
        return false;

    return true;
}

void Watcher::loadFromEnv(Environment &env)
{
    //m_options.clear();
    if (env.sectionExists(SECTION))
    {
        EnvironmentSection::const_iterator i;
        for (i = env[SECTION].begin(); i != env[SECTION].end(); ++i)
            setOption(i->first, i->second);
    }
    m_options[SECTION].insureExists("safe_mode", "yes");
    m_options[SECTION].insureExists("threading_mode", "none");
}
