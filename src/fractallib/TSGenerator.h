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
    //! Конструктор
    TSGenerator();

    //! Деструктор
    virtual ~TSGenerator()
    {
        m_templates.clear();
    }

    //! Генерирует %count значений и добавляет их в конец %ts. Также может сформировать эталонное дерево разбора
    void generate(TimeSeries *ts, int count, ParseTree *tree = NULL)
    {
        m_ts = ts;
        m_tree = tree;
        while (count > 0)
            count -= generateValue();
    }

    //! Добавляет шаблон генерации
    void addTemplate(Item &item)
    {
        m_templates.push_back(item);
    }

    //! Устанавливает коэффициент искажения шаблонов
    void setErrorFactor(double value)
    {
        m_errorFactor = value;
    }

    //! Возвращает коэффициент искажения шаблонов
    double errorFactor()
    {
        return m_errorFactor;
    }

    //! Устанавливает глубину генерируемых шаблонов
    void setDepth(int value)
    {
        if (m_depth != value)
        {
            m_depth = value > 0 ? value : 1;
            m_cashedValues.clear();
            m_cashPos = -1;
        }
    }

    //! Возвращает глубину генерируемых шаблонов
    int depth()
    {
        return m_depth;
    }

    //! Перезагружает генератор
    void reset()
    {
        m_cashedValues.clear();
        m_cashPos = -1;
        m_generatedCount = 0;
    }

    //! Событие, возникающее при автоматическом генерации нового значения
    NotifyNewValue onNewValue;
protected:
    //! Генерирует новые значения на основании шаблонов или какого-либо закона
    virtual int generateValue();

    //! Данный метод должен вызываться всякий раз при генерации нового значения в generateValue()
    void valueGenerated(double value)
    {
        m_ts->add(value);
        onNewValue(value);
    }

    //! Генерирует новую последовательность чисел на основании глубины
    void generateNewTemplates(int count);

    //! Расширяет слой: заменяет родителей на их потомков
    void expandLayer(std::vector<Item> &seq, int layerDepth);

    //! Делает из последовательности шаблонов набор чисел
    void makeTimeSeries(std::vector<Item> &seq);

    //! Проверяет условие
    bool checkCondition(Condition &c, std::vector<Item> &seq);

    //! Генерирует один новый шаблон учитывая его параметры
    void generateTemplate(Item &item, std::vector<Item> &seq);

    //! Возвращает индекс шаблона по его имени
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
    int m_generatedCount; //!< Число сгенерированных точек ряда с прошлой перезагрузки
};

} // namespace

#endif // TSGENERATOR_H
