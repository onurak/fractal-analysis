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

#ifndef TSGENERATOR_H
#define TSGENERATOR_H

#include "TimeSeries.h"
#include "G/GVariant.h"
#include "G/GDelegate.h"
#include "Exceptions.h"
#include "ParseTree.h"

namespace FL {

class TSGenerator
{
public:
    enum Direction { UP, DOWN };

    enum ConditionType { PREVIOUS, PREVIOUS_NOT };

    struct Condition
    {
        Condition(ConditionType type, GVariant value, GVariant option = G_VAR_NULL)
            : type(type), value(value), option(option) {}
        ConditionType type;
        GVariant value, option;
    };

    struct Item
    {
        void add(Direction dir, double length)
        {
            directions.push_back(dir);
            lengths.push_back(length);
        }

        std::string name;
        int no;
        std::vector<std::string> subitems;
        std::vector<double> lengths;
        std::vector<Direction> directions;
        std::vector<Condition> conditions;
        double minFactor, maxFactor;
        double minSize, maxSize;
    };


    typedef GDelegate1<void, double> NotifyNewValue;
public:
    //! �����������
    TSGenerator();

    //! ����������
    virtual ~TSGenerator()
    {
        m_templates.clear();
    }

    //! ���������� %count �������� � ��������� �� � ����� %ts. ����� ����� ������������ ��������� ������ �������
    void generate(TimeSeries *ts, int count, ParseTree *tree = NULL)
    {
        m_ts = ts;
        m_tree = tree;
        while (count > 0)
            count -= generateValue();
    }

    //! ��������� ������ ���������
    void addTemplate(Item &item)
    {
        m_templates.push_back(item);
    }

    //! ������������� ����������� ��������� ��������
    void setErrorFactor(double value)
    {
        m_errorFactor = value;
    }

    //! ���������� ����������� ��������� ��������
    double errorFactor()
    {
        return m_errorFactor;
    }

    //! ������������� ������� ������������ ��������
    void setDepth(int value)
    {
        if (m_depth != value)
        {
            m_depth = value > 0 ? value : 1;
            m_cashedValues.clear();
            m_cashPos = -1;
        }
    }

    //! ���������� ������� ������������ ��������
    int depth()
    {
        return m_depth;
    }

    //! ������������� ���������
    void reset()
    {
        m_cashedValues.clear();
        m_cashPos = -1;
        m_generatedCount = 0;
    }

    //! �������, ����������� ��� �������������� ��������� ������ ��������
    NotifyNewValue onNewValue;
protected:
    //! ���������� ����� �������� �� ��������� �������� ��� ������-���� ������
    virtual int generateValue();

    //! ������ ����� ������ ���������� ������ ��� ��� ��������� ������ �������� � generateValue()
    void valueGenerated(double value)
    {
        m_ts->add(value);
        onNewValue(value);
    }

    //! ���������� ����� ������������������ ����� �� ��������� �������
    void generateNewTemplates(int count);

    //! ��������� ����: �������� ��������� �� �� ��������
    void expandLayer(std::vector<Item> &seq, int layerDepth);

    //! ������ �� ������������������ �������� ����� �����
    void makeTimeSeries(std::vector<Item> &seq);

    //! ��������� �������
    bool checkCondition(Condition &c, std::vector<Item> &seq);

    //! ���������� ���� ����� ������ �������� ��� ���������
    void generateTemplate(Item &item, std::vector<Item> &seq);

    //! ���������� ������ ������� �� ��� �����
    int templateByName(std::string name)
    {
        std::vector<Item>::iterator t;
        int i = 0;
        for_each_(t, m_templates)
        {
            if (t->name == name)
                return i;
            ++i;
        }
        return -1;
    }

    TimeSeries *m_ts;
    std::map<int, std::vector<Item> > m_previous;
    std::vector<Item> m_templates;
    ParseTree *m_tree;
    std::vector<double> m_cashedValues;
    int m_cashPos;
    double m_errorFactor;
    int m_depth;
    int m_generatedCount; //!< ����� ��������������� ����� ���� � ������� ������������
};

} // namespace

#endif // TSGENERATOR_H
