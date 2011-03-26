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

#ifndef GLOGGER_H
#define GLOGGER_H

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include "GResources.h"

/*! \addtogroup GLib
  */
/*@{*/

enum GLogLevel {G_LOW = 0, G_MED = 1, G_HI = 2, G_DBG = 3};

/*! @class GLogger
  * @brief Simple logging tool
  *
  * You have to create global variable with name "logg" and use it because all
  * G library assuming it existance. Define variable as follow:
  * @code
  * GLogger logg("./lastrun.log");
  * @endcode
  * or with another suitable file name
  */
class GLogger
{
public:
    enum Output
    {
        G_LOG_FILE    = 0x1,
        G_LOG_CONSOLE = 0x2,
        G_LOG_ALL     = 0x3
    };

public:
    //! Constructor
    /*! @param fileName log file name
      */
    GLogger(const std::string &fileName, Output where = G_LOG_ALL)
        : m_file(fileName.c_str()), m_where(where)
    {
        m_debugLevel = G_DBG;
        lvl(G_LOW).log("Log started\n");
    }

    //! ����������
    ~GLogger()
    {
        lvl(G_LOW).endl().log("Log ended\n");
    }

    //! �������� ������ � ���
    /*! \param value ��������, ������� ���������� ��������
      * \return ������ �� ����
      * \sa log
      */
    template <class T> GLogger& operator<<(T value)
    {
        return this->log<T>(value);
    }

    //! ������� ������ � ���
    /*! \param value ��������, ������� ���������� ��������
      * \return ������ �� ����
      * \sa operator<<
      */
    template <class T> GLogger& log(T value)
    {
        if (m_msgLevel <= m_debugLevel)
        {
            if (m_where & G_LOG_FILE)
            {
                m_file << value;
                m_file.flush();
            }
            if (m_where & G_LOG_CONSOLE)
            {
                std::cout << value;
            }
        }
        return *this;
    }

    //! ��������, ��� ���������� ����� �������� ���������������
    /*! \return ������ �� ����
      */
    GLogger& warning(std::string msg = "")
    {
        return endl().lvl(G_LOW).log(GLoggerWarning).log(msg);
    }

    //! ��������, ��� ���������� ����� �������� �������
    /*! \return ������ �� ����
      */
    GLogger& error(std::string msg = "")
    {
        return endl().lvl(G_LOW).log(GLoggerError).log(msg);
    }

    //! ��������, ��� ���������� ����� �������� �����������
    /*! \return ������ �� ����
      */
    GLogger& info(std::string msg = "")
    {
        return endl().lvl(G_HI).log(GLoggerInfo).log(msg);
    }

    //! ��������, ��� ���������� ����� �������� ����������
    /*! \return ������ �� ����
      */
    GLogger& debug(std::string msg = "")
    {
        return endl().lvl(G_DBG).log(GLoggerDebug).log(msg);
    }

    //! ����������� ���������
    /*! \return ������ �� ����
      */
    GLogger& endl()
    {
        if (m_msgLevel <= m_debugLevel)
        {
            if (m_where & G_LOG_FILE)
                m_file << std::endl;
            if (m_where & G_LOG_CONSOLE)
                std::cout << std::endl;
        }
        lvl(G_LOW);
        return *this;
    }

    //! ������������� ������� ���������� ���������
    /*! \return ������ �� ����
      */
    GLogger& lvl(int lvl)
    {
        m_msgLevel = lvl;
        return *this;
    }

    //! ��������� ��������� ������ ���������
    /*! � ��� ����� ���������� ��������� � ������� ������� ��� ������ ���������
      * \param level �������� ������� ���������
      */
    void setDebugLevel(int level)
    {
        m_debugLevel = level;
        endl().lvl(G_LOW).log("Log: Message level changed to ").log(level);
    }
private:
    //! ���-����
    std::ofstream m_file;
    //! ������� �������� ���������
    int m_msgLevel;
    //! �������� ������� ��������� (���������� ��������� ������ � ������� ���� ��� ������)
    int m_debugLevel;

    //! Where to log
    Output m_where;
};

/*! \var logg
  * �������������� ����������, � ������� �� ��������� ��������� ��� ���������� � ���������� ���������
  * ��� log �� ������������ �.�. ��������� �������� � �������� log() �� ����� <math.h>
  */
extern GLogger logg;

/*@}*/

#endif // GLOGGER_H
