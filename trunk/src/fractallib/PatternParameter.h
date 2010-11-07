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

#ifndef PATTERNPARAMETER_H
#define PATTERNPARAMETER_H

#include <string>
#include <map>
#include <vector>
#include "G/GVariant.h"

namespace FL { namespace Patterns {

/*! \class Parameter
  * \brief Parameter of parametrized pattern
  *
  *
  */
class Parameter
{
public:
    Parameter(const std::string &name, const GVariant &value)
    {
        setName(name);
        setValue(value);
    }

    const std::string& name() const
    {
        return m_name;
    }

    void setName(const std::string &newName)
    {
        m_name = newName;
    }

    void setValue(const GVariant &newValue)
    {
        m_value = newValue;
    }

    const GVariant& value() const
    {
        return m_value;
    }

private:
    std::string m_name;
    GVariant m_value;
};

/*! \class ParameterSet
  * \brief Set of pattern's parameters
  */
class ParameterSet
{
public:
    ~ParameterSet()
    {
        clear();
    }

    void clear()
    {
        m_params.clear();
        m_paramsCache.clear();
    }

    void add(Parameter *p)
    {
        if (p != NULL)
        {
            m_params.push_back(p);
            m_paramsCache[p->name()] = p;
        }
    }

    Parameter* add(const std::string &name, const GVariant &value)
    {
        if (m_paramsCache[name] == NULL)
        {
            Parameter *p = new Parameter(name, value);
            add(p);
            return p;
        }
        else
            return NULL;
    }

    bool exists(const std::string &name)
    {
        return m_paramsCache[name] == NULL;
    }

    Parameter* get(const std::string &name)
    {
        return m_paramsCache[name];
    }

    void set(const std::string &name, const GVariant* newValue)
    {
        Parameter *p = m_paramsCache[name];
        if (p != NULL)
            p->setValue(newValue);
    }

    const GVariant& valueOf(const std::string &name)
    {
        Parameter *p = m_paramsCache[name];
        if (p != NULL)
            return p->value();
        else
            return GVARNULL;
    }

private:
    std::vector<Parameter*> m_params;
    std::map<std::string, Parameter*> m_paramsCache;
};

}} // namespaces

#endif // PATTERNPARAMETER_H
