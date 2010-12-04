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

#include "PredicateFactory.h"
#include "All.h"

using namespace FL::Predicates;

// Declare static variable of PredicateFactory
std::map<std::string, Predicate*>  PredicateFactory::m_predicates;

Predicate* PredicateFactory::predicateByName(std::string name)
{
    if (m_predicates.size() == 0)
        registerStandartPredicates();
    name = lowerCase(name);
    if (m_predicates.find(name) != m_predicates.end())
        return m_predicates[name];
    return NULL;
}

void PredicateFactory::registerPredicate(Predicate* predicate)
{
    logg.info("PredicateFactory: predicate ") << predicate->name() << " registered";
    m_predicates[lowerCase(predicate->name())] = predicate;
}

void PredicateFactory::registerStandartPredicates()
{
    registerPredicate(new Self());
    registerPredicate(new Prev());
    registerPredicate(new IsPrev());
    registerPredicate(new ArgCnt());
    registerPredicate(new Min());
    registerPredicate(new Max());
    registerPredicate(new Value());
    registerPredicate(new Sum());
    registerPredicate(new AtOneLine());
    registerPredicate(new StartTime());
    registerPredicate(new EndTime());
}
