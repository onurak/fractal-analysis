/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EEXCEPTION_H
#define EEXCEPTION_H

#include "../Common.h"

namespace FL {

    static const int E_OK                = 0;
    static const int E_INVALID_COUNT     = 1;
    static const int E_INVALID_TYPE      = 2;
    static const int E_INVALID_PATTERNS  = 3;
    static const int E_UNKNOWN_ARG       = 4;

} // namespaces

namespace FL { namespace Exceptions {

/*! \class EException
  * \brief Base class for all exceptions of Fractal Library
  */
class EException
{
public:
    //! Default constructor
    EException() : m_id(E_OK), m_arg("") {}

    //! Cnostructor that can set id of error and description or argument
    EException(int id, const std::string& arg);

    //! Copy constructor
    EException(const EException &e): m_id(e.m_id), m_arg(e.arg()) {}

    //! Identifier of error (e.g. subclass)
    int id() const;

    //! Argument of exception (description or somewhat)
    const std::string& arg() const;
protected:
    int m_id;
    std::string m_arg;
};

/*! \class EParsing
  * \brief Exception that occures during parsing or compiling of expression
  *
  * It could be thrown by FL::Compilers::AbstractCompiler class or its children.
  */
class EParsing: public EException
{
public:
    EParsing(int id = E_OK, int line = -1, int column = -1, const std::string &arg = "");
    void set(int id, int line = -1, int column = -1, const std::string &arg = "");
    const EParsing& operator=(const EParsing& e);

    int line() const;
    int column() const;
private:
    int m_line;
    int m_column;
};

/*! \class EAnalyze
  * \brief Exception that occures during time series analysis (murkup or parse processes)
  */
class EAnalyze : public EException
{
public:
    EAnalyze(int id = E_OK, const std::string& arg = "") : EException(id, arg) {}
    void setArg(const char *arg) { m_arg = arg ? arg : ""; }
};

/*! \class EArguments
  * \brief Exception that occures when invalid arguments passed to
  *        FL::Patterns::Functions::Function class and its children.
  */
class EArguments : public EException
{
public:
    //! General purpose constructor
    EArguments(int id = E_OK, const std::string& arg = "") : EException(id, arg) {}

    //! Special constructor for id = FL::E_INVALID_COUNT
    /*! \param name Name of function
      * \param expectedCount Expected count of arguments. If it's positive
      *        than it means exact count, if negative than it is "at least
      *        -expectedCount"
      * \param gotCount Actual number of arguments that passed to function
      */
    EArguments(const std::string &name, int expectedCount, int gotCount);

    //! Special constructor for id = FL::E_INVALID_COUNT
    /*! \param name Name of function
      * \param expectedCountFrom Minimal expected count of arguments
      * \param expectedCountTo Maximal expected count of arguments
      * \param gotCount Actual number of arguments that passed to function
      */
    EArguments(const std::string &name, int expectedCountFrom, int expectedCountTo, int gotCount);

    //! Special constructor for id = FL::E_INVALID_TYPE
    /*! \param name Name of function
      * \param expectedType Expected type of argument
      * \param gotType Actual type of argument. Default is empty string means don't care
      * \param argIndex Optional index of invalid parameter. Default is -1 means don't care
      */
    EArguments(const std::string &name, const std::string &expectedType,
               const std::string &gotType = "", int argIndex = -1);

    //! Special constructor for id = FL::E_UNKNOWN_ARG
    /*! \param name Name of function
      * \param arg String representation of arg
      * \param argIndex Optional index of invalid parameter. Default is -1 means don't care
      */
    EArguments(const std::string &name, const std::string &arg, int argIndex = -1);
};

}} // namespaces

#endif // EEXCEPTION_H
