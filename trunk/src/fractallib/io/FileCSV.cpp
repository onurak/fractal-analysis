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

#include "FileCSV.h"
#include "../tools/spirit.h"

using namespace FL;


namespace CSV
{
    // current data to fill
    std::vector< std::vector <GVariant> > *m_data = NULL;
    std::vector< std::string > *m_header = NULL;
    bool m_headerLine;

    // triggers
    void onLine(std::vector<std::string> v)
    {
        std::vector<std::string>::const_iterator i;
        if (m_headerLine && m_header != NULL)
        {
            for (i = v.begin(); i != v.end(); ++i)
                m_header->push_back(*i);
            m_headerLine = false;
        } else {
            std::vector<GVariant> vec;
            for (i = v.begin(); i != v.end(); ++i)
                if (*i != "")
                    vec.push_back(GVariant(*i));
            if (vec.size() > 0)
                m_data->push_back(vec);
        }
    }

    void onStart()
    {
        m_headerLine = true;
    }

    // грамматика CSV-файла
    template <typename Iterator>
    struct Grammar : qi::grammar<Iterator>
    {
        Grammar() : Grammar::base_type(start)
        {
            using ascii::char_;
            using qi::eol;
            using qi::eps;

            start = eps[onStart] >> *((value % ';')[onLine] >> eol);
            value = *(char_ - ';' - eol);
        }

        qi::rule<Iterator> start;
        qi::rule<Iterator, std::string()> value;
    };

    // парсер
    void parse(const std::string & fileName,
               std::vector<std::string> *header,
               std::vector< std::vector<GVariant> > *data)
    {
        // ready
        if (data == NULL || header == NULL)
        {
            GError(GCritical, "FileCSV", 0, "CSV parser internal error: Invalid data or header");
            return;
        }
        m_data = data;
        m_header = header;

        // steady
        typedef CSV::Grammar<std::string::const_iterator> CSVGrammar;
        CSVGrammar g;
        // go
        int r = spiritParseFile(fileName, g);
        if (r == EParserCantOpenFile)
        {
            GError(GCritical, "FileCSV", 0, ECantOpenFile + fileName);
            return;
        }
        else if (r >= 0)
        {
            GError(GCritical, "FileCSV", 0, EInvalidFileFormat + fileName);
            return;
        }
        else
            ;
    }

} // namespace


FileCSV::FileCSV(const std::string & fileName)
{
    if (fileName != "")
        loadFromFile(fileName);
}

void FileCSV::loadFromFile(const std::string & fileName)
{
    m_header.clear();
    m_strings.clear();
    CSV::parse(fileName, &m_header, &m_strings);
}

void FileCSV::saveToFile(const std::string & fileName)
{
    std::ofstream file;
    file.open(fileName.c_str());
    if (!file.is_open())
    {
        GError(GCritical, "FileCSV", 0, ECantOpenFile + fileName);
        return;
    }
    saveFile(file);
    file.close();
}

void FileCSV::saveToFile(const std::string & fileName, TimeSeries *ts, int dimension)
{
    if (ts == NULL)
        return;
    if (!ts->validParams[dimension])
        return;
    std::ofstream file;
    file.open(fileName.c_str());
    if (!file.is_open())
    {
        GError(GCritical, "FileCSV", 0, ECantOpenFile + fileName);
        return;
    }

    std::vector<double> vec;
    if (!ts->getTSByIndex(vec, dimension))
        return;

    file << "<VALUE>\n";
    for (unsigned int i = 0; i < vec.size(); ++i)
        file << vec[i] << std::endl;

    file.close();
}

int FileCSV::guessTimeSeries()
{
    /*if (m_strings.size() == 0 || m_header.size() == 0)
        return -1;
    unsigned int hIndex = 0;

    for (; hIndex < m_header.size(); hIndex++) {
        std::string s = lowerCase(m_header[hIndex]);
        if (s == "row" || s == "value" || s == "left front" || s == "<last>")
            break;
    }
    if (hIndex == m_header.size())
        return -1;

    return hIndex;*/

    if (m_header.size() == 1)
        return 0;
    else
        return -1;
}

void FileCSV::loadTimeSeries(FL::TimeSeries *ts, int columnIndex)
{
    ts->data.clear();
    ts->validParams.clear();
    std::vector<double> vec;
    for (unsigned int i = 0; i < m_strings.size(); i++)
        vec.push_back(m_strings[i][columnIndex]);
    ts->validParams.push_back(true);
    ts->data.push_back(vec);
    if (!ts->setTSByIndex(vec, 0))
    {
        GError(GCritical, "FileCSV", 0, EInvalidFileFormat);
        return;
    }
}

/*
bool getNextWord(char *dst, char *src, int &pos, char delim, bool skipSpaces = true)
{
    int i;
    if (skipSpaces)
        while (isspace(src[pos])) pos++;
    if (skipSpaces)
        for (i = 0; src[pos]!=delim && src[pos]!='\0' && !isspace(src[pos]); pos++, i++)
            dst[i] = src[pos];
    else
        for (i = 0; src[pos]!=delim && src[pos]!='\0' && src[pos] != '\n'; pos++, i++)
            dst[i] = src[pos];
    dst[i] = '\0';
    if (src[pos] != delim && i == 0)
        return false;
    if (src[pos] == delim)
        pos++;
    return true;
}

void FileCSV::parseFile(ifstream &file)
{
    const std::string null = "null";
    char buffer[2000], value[256];
    int i, cnt, strNo = 0;
    std::vector<GVariant> vec;

    while (file.getline(buffer, 2000)) {
        i = 0;
        strNo++;
        // заголовок?
        if (strNo == 1) {
            while (getNextWord(value, buffer, i, ';', false))
                m_header.push_back(value);
            continue;
        }
        // очередная строка
        cnt = 0;
        vec.clear();
        while (getNextWord(value, buffer, i, ';')) {
            if (!strcmp(value, "") || lowerCase(value) == null)
                vec.push_back(GVariant(G_VAR_NULL));
            else
                vec.push_back(GVariant(value));
            cnt++;
        }
        if (cnt != (int)m_header.size())
            throw EFile(EInvalidFileFormat, atPos(strNo, i+1));

        m_strings.push_back(vec);
    }
}
*/


void FileCSV::saveFile(std::ostream &file)
{
    for (unsigned int i = 0; i < m_header.size(); ++i)
        file << m_header[i] << (i != m_header.size()-1 ? ';' : '\n');

    for (unsigned int str = 0; str < m_strings.size(); ++str)
        for (unsigned int i = 0; i < m_strings[str].size(); ++i)
            file << m_strings[str][i]
                 << (i != m_strings[str].size()-1 ? ';' : '\n');
}
