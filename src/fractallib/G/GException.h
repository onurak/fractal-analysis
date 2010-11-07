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

/*! \class GError
  * \brief Error notifications handler
  *
  * All errors are automatically log and all subscribers will be notified
  */
class GErrorClass
{
public:
    typedef GDelegate4<void, GErrorType, std::string, int, std::string> NotifyError;
public:
    //! Error notification
    /*! Use it if error or warning happens to notify about it all subscribers
      * \param type Error type
      * \param notifier Some notifier description (like class name)
      * \param errNo Notifier's internal error number
      * \param errDescription Description of error
      */
    void notify(GErrorType type,
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
        logg << errNo << "(by " << notifier << "): " << errDescription;
        onError(type, notifier, errNo, errDescription);
    }

    static GErrorClass& instance()
    {
        static GErrorClass *m_instance = NULL;
        if (m_instance == NULL)
            m_instance = new GErrorClass();
        return *m_instance;
    }

    //! Notification delegate
    NotifyError onError;
private:
    //! Hiden constructor
    GErrorClass() {}
};

//! Use this function for error notifications
inline void GError(GErrorType type,
            const std::string &notifier,
            int errNo,
            const std::string &errDescription)
{
    GErrorClass::instance().notify(type, notifier, errNo, errDescription);
}

/*! \class GException
  * \brief Internal GLibrary exception
  *
  * Note that you shouldn't throw exception outside of G Library.
  * For outer error notifications use GError.
  */
class GException
{
public:
    GException(int errNo, const std::string &msg)
        : m_errNo(errNo), m_msg(msg)
    {
        logg.endl();
        logg << "Error " << errNo << ": " << msg;
    }
private:
    int m_errNo;
    std::string m_msg;
};

/*@}*/

#endif // GEXCEPTION_H
