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

#ifndef GEXCEPTION_H
#define GEXCEPTION_H

#include <string>
#include "GLogger.h"
#include "GDelegate.h"

/*! \addtogroup GLib
  */
/*@{*/

enum GErrorType
{
    GWarning, //! Something suspecious, but procedure still can continue execution
    GCritical  //! Critical error, can't execute anymore
};

typedef GDelegate4<void, GErrorType, const std::string&, int, const std::string&> GNotifyError;

/*! \class GException
  * \brief Internal GLibrary exception
  *
  * Note that you shouldn't throw exception outside of G Library.
  * For outer error notifications use GError.
  */
class GException
{
public:
    GException() {}

    GException(int errNo, const std::string &msg)
        : m_errNo(errNo), m_msg(msg)
    {
        report();
    }

    int errNo() const { return m_errNo; }

    const std::string& msg() const { return m_msg; }
protected:
    void report() const
    {
        logg.endl();
        logg << "Error " << m_errNo << ": " << m_msg;
    }
    int m_errNo;
    std::string m_msg;
};

/*! \class GErrorClass
  * \brief Error notifications handler
  *
  * All errors are automatically log and all subscribers will be notified
  */
class GErrorClass
{    
public:
    //! Error notification
    /*! Use it if error or warning happens to notify about it all subscribers
      * \param type Error type
      * \param notifier Some notifier description (like class name)
      * \param errNo Notifier's internal error number
      * \param errDescription Description of error
      */
    static void notify(
            GErrorType type,
            const std::string &notifier,
            int errNo,
            const std::string &errDescription)
    {
        logg.endl();
        switch (type)
        {
            case GWarning: logg.log("Warning "); break;
            default: logg.log("Error "); break;
        }
        logg << errNo << " (by " << notifier << "): " << errDescription;
        logg.endl();
        onError(type, notifier, errNo, errDescription);
    }

    //! Notification delegate
    static GNotifyError onError;
};

//! Use this function for error notifications
inline void GError(GErrorType type,
            const std::string &notifier,
            int errNo,
            const std::string &errDescription)
{
    GErrorClass::notify(type, notifier, errNo, errDescription);
    //throw GException(errNo, errDescription);
}

/*@}*/

#endif // GEXCEPTION_H
