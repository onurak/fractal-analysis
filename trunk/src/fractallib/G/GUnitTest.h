/* Author: Vladimir Belikov
 *
 * This file is part of G Library.
 *
 * G Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * G Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with G Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUNITTEST_H
#define GUNITTEST_H

#include <vector>
#include <string>
#include "GVariant.h"

#define GBeginTest try {

#define GEndTest(id, description, result) \
        result.add(id, GPassed, description); \
    } catch (GException &e) { \
        result.add(id, GFailed, e.msg()); \
    } catch (char *msg) { \
        result.add(id, GFailed, msg); \
    } catch (std::string &msg) { \
        result.add(id, GFailed, msg); \
    } catch (...) { \
        result.add(id, GFailed, description); \
    }


const bool GPassed = true;
const bool GFailed = false;

/*! \struct GUnitTestResult
  * \brief Result of single test
  */
struct GUnitTestResult {
    GVariant id;                    //!< Test identifier (name, no etc.)
    bool passed;                    //!< Test result (true means pass)
    std::string resultDescription;  //!< Option description of test result

    //! Default constructor
    GUnitTestResult(const GVariant &id = GVariant(G_VAR_NULL),
                    bool passed = false,
                    std::string resultDescription = "")
    {
        this->id = id;
        this->passed = passed;
        this->resultDescription = resultDescription;
    }

    //! Copy constructor
    GUnitTestResult(const GUnitTestResult &r) {
        this->id = r.id;
        this->passed = r.passed;
        this->resultDescription = r.resultDescription;
    }

    //! Smart convertion to string
    std::string toString() {
        std::string result;
        result += passed ? "[OK] " : "[FAIL] ";
        result += id.type() != G_VAR_NULL?  (char*)id  :  "Unnamed";
        if (!resultDescription.empty())
            result += ".\n   NOTE: " + resultDescription;
        return result;
    }
};

/*! \class GUnitTestResults
  * \brief Sequence of multiple tests results
  */
class GUnitTestResults {
public:
    //! vector of results itself
    std::vector<GUnitTestResult> values;

    //! Sequence operator
    GUnitTestResult& operator[](int i) { return values[i]; }

    //! Passed tests count
    int passedCount() {
        int result = 0;
        for (unsigned int i = 0; i < values.size(); ++i)
            if (values[i].passed) ++result;
        return result;
    }

    //! Total tests count
    int totalCount() {
        return int(values.size());
    }

    //! Clear any previous results
    void clear() {
        values.clear();
    }

    //! Shortcut for adding results
    void add(const GUnitTestResult &r) {
        values.push_back(r);
    }

    //! Shortcut for adding results
    void add(const GVariant &id = GVariant(G_VAR_NULL),
             bool passed = false,
             std::string resultDescription = "") {
        values.push_back(GUnitTestResult(id, passed, resultDescription));
    }

    //! Convert all results to string
    std::string toString() {
        std::string result;
        bool ok = passedCount() == totalCount();
        result += "** BEGIN OF TESTING **\n";
        result += "Passed/Total tests: ";
        result += (char*)GVariant(passedCount());
        result += '/';
        result += (char*)GVariant(totalCount());
        result += ok ? " [OK]" : " [FAILED]";
        result += "\nTests statistics:\n";
        for (int i = 0; i < totalCount(); ++i)
            result += values[i].toString() + '\n';
        result += "** END OF TESTING **\n\n";
        return result;
    }

    //! Write results to file
    bool writeToFile(const std::string &fileName) {
        std::ofstream file(fileName.c_str());
        if (!file.is_open())
            return false;
        file << toString();
        file.close();
        return true;
    }
};

/*! \class GUnitTest
  * \brief Provide basic features for unit testing
  */
class GUnitTest
{
public:
    //! Default constructor
    GUnitTest() {}

    //! Do all unit tests
    /*! \param res Results of testing will be writed here
      */
    virtual void run(GUnitTestResults &res) = 0;
};

#endif // GUNITTEST_H
