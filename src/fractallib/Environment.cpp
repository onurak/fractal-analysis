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

#include "Environment.h"

using namespace FL;

namespace EnvGrammar
{
    FL::Environment *m_env;
    std::string section, field;

    void onField(std::string value)
    {
        field = value;
    }

    void onSection(std::string value)
    {
        section = value;
    }

    void onValue(std::string value)
    {
        if (value[0] != '"')
            (*m_env)[section][field] = value;
        else
            (*m_env)[section][field] = std::string(value.begin()+1, value.end()-1);
    }


    template <typename Iterator>
    struct Grammar : qi::grammar<Iterator>
    {
        Grammar() : Grammar::base_type(file)
        {
            using ascii::char_;
            using ascii::alnum;
            using qi::eps;
            using qi::eol;
            using namespace boost::spirit;

            file     = *(section >> *(record % eol));
            section  = *space >> char_('[') >> value[onSection] >> ']' >> *space;
            record   = *avspace >> value[onField] >> *avspace >> '=' >>
                       *avspace >> value[onValue] >> *avspace;
            value   %= ( char_('"') >> (*(~char_('"'))) >> char_('"') )
                       |
                       (*(alnum | '.' | '_'));
            space    = char_(' ') | char_('\t') | char_('\n');
            avspace  = char_(' ') | char_('\t');
        }

        qi::rule<Iterator>
            file, section, record, space, avspace;
        qi::rule<Iterator, std::string()>
            value;
    };
} // namespace


bool Environment::addFromFile(const std::string &fileName)
{
    logg.info("Environment loading from ").log(fileName).log("... ");

    EnvGrammar::m_env = this;
    EnvGrammar::Grammar<std::string::const_iterator> g;
    int err = spiritParseFile(fileName, g);

    if (err == -1)
    {
        logg.info("Environment loaded OK");
        return true;
    }
    else
    {
        logg.info("Environment fail to load");
        return false;
    }
}


void EnvironmentSection::appendFrom(Environment &env, bool reassingExisting)
{
    if (env.sectionExists(m_name))
    {
        std::map<std::string, GVariant>::const_iterator p;
        for (p = env[m_name].begin(); p != env[m_name].end(); p++)
        {
            std::string param = p->first;
            if (find(param) != end() && !reassingExisting)
                continue;
            (*this)[param] = p->second;
        }
    }
}
