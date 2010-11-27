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

#ifndef TSDYNAMICPARSER_H
#define TSDYNAMICPARSER_H

#include "../G/GDelegate.h"
#include "../groupcheckers/GroupChecker.h"
#include "../Pattern.h"
#include "../ParseContext.h"
#include "../ParseResult.h"
#include "../ProjectSettings.h"

namespace FL
{

/*! \class Forecast
  * \brief Forecast of ParseTree behaviour
  */
class Forecast
{
public:
    enum ForecastError
    {
        OK,
        INVALID_PARAMETER,
        INVALID_POSITION,
        INTERSECTION
    };

public:
    //! Default construtor
    Forecast(ParseTree *tree = NULL, Pattern *pattern = NULL,
             int position = 0, int level = 0,
             double minDuration = 0, double maxDuration = 0,
             double minValue = 0, double maxValue = 0)
        : m_parseTree(tree), m_pattern(pattern),
          m_position(position), m_level(level),
          m_minDuration(minDuration), m_maxDuration(maxDuration),
          m_minValue(minValue), m_maxValue(maxValue)
    {
    }

    ParseTree* parseTree() { return m_parseTree; }
    void setParseTree(ParseTree *tree) { m_parseTree = tree; }

    Pattern* pattern() { return m_pattern; }
    void setPattern(Pattern *value) { m_pattern = value; }

    int position() const { return m_position; }
    void setPosition(int value) { m_position = value; }

    int level() const { return m_level; }
    void setLevel(int value) { m_level = value; }

    double minDuration() const { return m_minDuration; }
    void setMinDuration(double value) { m_minDuration = value; }

    double maxDuration() const { return m_maxDuration; }
    void setMaxDuration(double value) { m_maxDuration = value; }

    double minValue() const { return m_minValue; }
    void setMinValue(double value) { m_minValue = value; }

    double maxValue() const { return m_maxValue; }
    void setMaxValue(double value) { m_maxValue = value; }


    //! Make forecast data be part of parse tree
    ForecastError apply(TimeSeries *ts, int tsBegin, int tsEnd)
    {
        // Check parameters
        if (ts == NULL || m_parseTree == NULL || m_pattern == NULL || m_level < 0)
            return INVALID_PARAMETER;
        if (tsBegin < 0 || tsBegin > tsEnd)
            return INVALID_POSITION;

        // If there is position intersection with other node on the same level than it is error
        ParseTree::ConstLayer *layerLevel = m_parseTree->getLevel(m_level);
        ParseTree::ConstLayer::const_iterator node;
        for_each_(node, *layerLevel)
        {
            if (isSegmentsOverlay(tsBegin, tsEnd, (*node)->tsBegin, (*node)->tsEnd))
                return INTERSECTION;
        }

        // Create node
        ParseTreeNode *newNode = new ParseTreeNode(
                m_pattern->description()->name(),
                NULL, m_level, tsBegin, tsEnd);

        // Find children and parent
        ParseTree::ConstLayer &childrenLayer =
                m_parseTree->roots(tsBegin, tsEnd);
        for_each_(node, childrenLayer)
        {
            // Does it have level less then newNode?
            //    Yes - it's child
            //    No  - it one of possible parents
            if ((*node)->level < newNode->level)
                newNode->children.push_back(*node);
            else
                newNode->parent = *node;
        }

        // Add node
        m_parseTree->addNode(newNode);
        return OK;
    }
private:
    ParseTree *m_parseTree;
    Pattern *m_pattern;
    int m_position;
    int m_level;
    double m_minDuration;
    double m_maxDuration;
    double m_minValue;
    double m_maxValue;
};

/*! \class ForecastSet
  * \brief Group of forecasts
  */
class ForecastSet
{
public:
    ForecastSet(ParseTree *tree, TimeSeries *ts)
        : m_parseTree(tree), m_ts(ts)
    {
    }

    void add(Forecast *forecast)
    {
        if (forecast != NULL)
        {
            forecast->setParseTree(parseTree());
            m_forecasts.push_back(forecast);
        }
    }

    void remove(int index)
    {
        if (index >= 0 && index < int(m_forecasts.size()))
        {
            Forecast *tmp = *(m_forecasts.begin() + index);
            m_forecasts.erase(m_forecasts.begin() + index);
            delete tmp;
        }
    }

    void remove(Forecast *forecast)
    {
        remove(indexOf(forecast));
    }

    int indexOf(Forecast *forecast)
    {
        std::vector<Forecast*>::const_iterator i;
        i = std::find(m_forecasts.begin(), m_forecasts.end(), forecast);
        if (i != m_forecasts.end())
            return i - m_forecasts.begin();
        else
            return -1;
    }

    Forecast* at(int index)
    {
        return (index >= 0 && index < int(m_forecasts.size())) ? m_forecasts[index] : NULL;
    }

    Forecast* operator[](int index)
    {
        return at(index);
    }

    int count() const { return int(m_forecasts.size()); }

    ParseTree *parseTree() { return m_parseTree; }

    TimeSeries* timeSeries() { return m_ts; }

private:
    std::vector<Forecast*> m_forecasts;
    ParseTree *m_parseTree;
    TimeSeries *m_ts;
};

/*! \class TSDynamicParser
  * \brief Interface of dynamic parser
  */

class TSDynamicParser
{
public:
    virtual int analyse(ParseTreeSet &trees,
                        ForecastSet &forecasts,
                        PatternCollection &patterns,
                        TimeSeries *ts,
                        Patterns::GroupChecker &gc
                        ) = 0;
protected:
private:
};

}

#endif // TSDYNAMICPARSER_H
