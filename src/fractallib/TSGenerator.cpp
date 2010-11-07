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

#include "TSGenerator.h"
#include <math.h>
#include <time.h>

using namespace FL;

double randRange(double min, double max)
{
    double rnd = double(rand()) / double(RAND_MAX);
    return min + rnd * (max - min);
}

int randRange(int min, int max)
{
    double rnd = double(rand()) / double(RAND_MAX);
    return min + lround(rnd * (max - min));
}

inline ParseTreeNode* makeParseTreeNode(TSGenerator::Item item, int depth,
     int tsBegin = 0, int tsEnd = 0)
{
    return new ParseTreeNode(
            item.name,
            NULL,
            depth,
            tsBegin,
            tsEnd
            );
}

TSGenerator::TSGenerator()
{
    srand(time(NULL));
    m_ts = NULL;
    m_tree = NULL;
    m_errorFactor = 0;
    m_depth = 0;
    reset();
}

int TSGenerator::generateValue()
{
    if (m_templates.size() == 0)
        return 10000;

    // остались ли сгенерированные с прошлого раза значения, которые мы еще не выдали?
    if (m_cashedValues.size() == 0 || m_cashPos < 0 || m_cashPos >= int(m_cashedValues.size())) {
        if (m_cashedValues.size()) {
            double lastValue = m_cashedValues[m_cashedValues.size()-1];
            m_cashedValues.clear();
            m_cashedValues.push_back(lastValue);
            m_cashPos = 1;
        } else {
            m_cashedValues.clear();
            m_cashedValues.push_back(0);
            m_cashPos = 0;
        }

        generateNewTemplates(2);
    }

    int count = 0;
    while (m_cashPos < int(m_cashedValues.size())) {
        double value = m_cashedValues[m_cashPos++];
        valueGenerated(value);
        ++count;
    }
    return count;
}

void TSGenerator::generateTemplate(Item &item, std::vector<Item> &seq)
{
    std::vector<int> checked;
    int idx;
    while (true)
    {
        if (checked.size() != m_templates.size()) {
            do {
                idx = randRange(0, m_templates.size() - 1);
            } while (find(checked.begin(), checked.end(), idx) != checked.end());
            checked.push_back(idx);

            item = m_templates[idx];
            std::vector<Condition>::iterator cond;
            bool again = false;
            for_each_(cond, item.conditions)
                if (!checkCondition(*cond, seq)) {
                    again = true;
                    break;
                }
            if (!again)
                break;
        } else {
            item = m_templates[randRange(0, m_templates.size() - 1)];
            break;
        }
    }
}

void TSGenerator::generateNewTemplates(int count)
{
    // генерируем count шаблонов верхнего уровня
    std::vector<Item> layer;
    while (count-- > 0) {
        Item tmp;
        generateTemplate(tmp, layer);
        //if (m_tree)
          //  m_tree->insertNode(makeParseTreeNode(tmp, m_depth+2, 0, 20));
        layer.push_back(tmp);
    }

    // раскрываем их m_depth-1 раз
    int depth = m_depth - 1;
    while (depth >= 0) {
        expandLayer(layer, depth+1);
        depth--;
    }

    // превращаем шаблоны во временной ряд
    makeTimeSeries(layer);

    // лог
    /*
    std::vector<double>::iterator value;
    std::vector<Item>::iterator item;
    logg.debug("TSGenerator: generated template sequence:");
    for_each_(item, layer)
        logg << item->name << "   ";
    logg << "EOS.";
    logg.debug("Generated value sequence:");
    for_each_(value, m_cashedValues)
        logg << *value << "   ";
    logg << "EOS.";
    */

}

void TSGenerator::expandLayer(std::vector<Item> &seq, int layerDepth)
{
    std::vector<Item> result;
    std::vector<Item>::iterator item;
    std::vector<std::string>::iterator subitem;

    for_each_(item, seq) {
        for_each_(subitem, item->subitems) {
            Item tmp = m_templates[ templateByName(*subitem) ];
            //if (m_tree)
              //  m_tree->insertNode(makeParseTreeNode(tmp, layerDepth));
            result.push_back(tmp);
        }
    }

    seq = result;
}

void TSGenerator::makeTimeSeries(std::vector<Item> &seq)
{
    std::vector<Item>::iterator item;
    std::vector<double>::iterator length;
    std::vector<Direction>::iterator dir;
    double value;

#define lastCashed m_cashedValues[m_cashedValues.size()-1]

    // раскрываем кажды элемент
    for_each_(item, seq) {
        int tsBegin = m_generatedCount,   tsEnd;
        // выравниваем значение чтобы не было резких скачков
        //double endOfPrevious = lastCashed;
        //double shift = endOfPrevious * randRange(-0.1, 0.1);

        for(length = item->lengths.begin(), dir = item->directions.begin();
            length != item->lengths.end();
            ++length, ++dir)
        {
            if (*dir == UP)
                value = lastCashed + *length;
            else
                value = lastCashed - *length;
            value += randRange(-m_errorFactor, +m_errorFactor);

            m_cashedValues.push_back(value);
            ++m_generatedCount;
        }
        tsEnd = m_generatedCount-1;
        if (m_tree)
            m_tree->addNode(makeParseTreeNode(*item, 1, tsBegin, tsEnd));
    }
    //m_previous[1].insert(m_previous[1].end(), seq.begin(), seq.end());
}

bool TSGenerator::checkCondition(Condition &c, std::vector<Item> &seq)
{
    switch (c.type) {
        case PREVIOUS:
            return seq.size() >  0 && c.value == seq[seq.size()-1].name && int(c.option) == seq[seq.size()-1].no;
        case PREVIOUS_NOT:
            return seq.size() == 0 || c.value != seq[seq.size()-1].name || int(c.option) != seq[seq.size()-1].no;
        default:
            return false;
    }
}
