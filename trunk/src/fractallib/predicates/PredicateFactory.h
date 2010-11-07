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

#ifndef PREDICATEFACTORY_H
#define PREDICATEFACTORY_H

#include <map>
#include "../Predicates.h"
#include "All.h"

namespace FL { namespace Predicates {

/*! \class PredicateFactory
  * \brief Predicate factory (get term by its name)
  */
class PredicateFactory
{
private:
    static std::map<std::string, Predicate*> m_predicates;
public:
    static Predicate* predicateByName(std::string name)
    {
        if (m_predicates.size() == 0)
            registerStandartPredicates();
        name = lowerCase(name);
        if (m_predicates.find(name) != m_predicates.end())
            return m_predicates[name];
        return NULL;
    }

    static void registerPredicate(Predicate* predicate)
    {
        logg.info("PredicateFactory: predicate ") << predicate->name() << " registered";
        m_predicates[lowerCase(predicate->name())] = predicate;
    }

    static void registerStandartPredicates()
    {
        registerPredicate(new ArgCnt());
        registerPredicate(new Min());
        registerPredicate(new Max());
        registerPredicate(new Previous());
        registerPredicate(new Value());
        registerPredicate(new ExtractPrev());
        registerPredicate(new Sum());
    }
};

/*! \class PredicateCall
 * \brief Store predicate
 */
class PredicateCall
{
public:
    PredicateCall(std::string name = "", std::string indexedName = "", int indexedNo = -1)
        : m_indexedName(indexedName), m_indexedNo(indexedNo)
    {
        setName(name);
    }

    PredicateCall(const PredicateCall &predicateCall)
    {
        this->operator=(predicateCall);
    }

    PredicateCall& operator=(const PredicateCall& value)
    {
        setName(value.name());
        m_args.clear();
        for (unsigned int i = 0; i < value.m_args.size(); i++)
            m_args.push_back(value.m_args[i]);
        m_indexedName = value.indexedName();
        m_indexedNo = value.indexedNo();
        return *this;
    }

    const GVariant& operator()(Patterns::CheckContext &context)
    {
        return (*m_predicate)(context, m_args);
    }

    const std::string& name() const
    {
        return m_name;
    }

    void setName(const std::string &value)
    {
        m_name = value;
        if (m_name != "")
            m_predicate = PredicateFactory::predicateByName(m_name);
        else
            m_predicate = NULL;
    }

    PredicateArgs& args()
    {
        return m_args;
    }

    bool hasArgs() const
    {
        return m_args.size() != 0;
    }

    Predicate* predicate() const
    {
        return m_predicate;
    }

    int indexedNo() const
    {
        return m_indexedNo;
    }

    void setIndexedNo(int value)
    {
        m_indexedNo = value;
    }

    std::string indexedName() const
    {
        return m_indexedName;
    }

    void setIndexedName(std::string value)
    {
        m_indexedName = value;
    }

    /*
    bool canApplyTo(const SequenceElement &se) const
    {
        if (m_indexedName != "" && m_indexedNo != -1)
            return se.type() != setUnmarked && se.no() == m_indexedNo && se.name() == m_indexedName;
        return true;
    }
    */
private:
    std::string m_name;
    std::string m_indexedName;
    int m_indexedNo;
    PredicateArgs m_args;
    Predicate *m_predicate;
};

}} // namespaces

#endif // PREDICATEFACTORY_H
