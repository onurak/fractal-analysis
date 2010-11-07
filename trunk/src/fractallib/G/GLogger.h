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
#include "GResources.h"

/*! \addtogroup GLib
  */
/*@{*/

enum GLogLevel {G_LOW = 0, G_MED = 1, G_HI = 2, G_DBG = 3};

/*! \class GLogger
  * \brief Позволяет вести лог работы программы
  *
  * Позволяет выводить как произвольные сообщения, так и стандартного типа:
  * предупреждения, ошибки и информацию.
  *
  * Автоматически создается логер с именем logg, который рекомендуется использовать. В него также
  * выводятся все исключения и внутренние сообщения
  */
class GLogger
{
public:
    //! конструктор
    /*! \param fileName имя лог-файла
      */
    GLogger(std::string fileName): m_file(fileName.c_str())
    {
        m_debugLevel = G_DBG;
        lvl(G_LOW).log("Log started\n");
    }

    //! деструктор
    ~GLogger()
    {
        lvl(G_LOW).endl().log("Log ended\n");
    }

    //! Оператор записи в лог
    /*! \param value значение, которое необходимо записать
      * \return Ссылку на себя
      * \sa log
      */
    template <class T> GLogger& operator<<(T value)
    {
        if (m_msgLevel <= m_debugLevel)
        {
            m_file << value;
            m_file.flush();
        }
        return *this;
    }

    //! Функция записи в лог
    /*! \param value значение, которое необходимо записать
      * \return Ссылку на себя
      * \sa operator<<
      */
    template <class T> GLogger& log(T value)
    {
        if (m_msgLevel <= m_debugLevel)
        {
            m_file << value;
            m_file.flush();
        }
        return *this;
    }

    //! Сообщает, что дальнейший текст является предупреждением
    /*! \return Ссылку на себя
      */
    GLogger& warning(std::string msg = "")
    {
        return endl().lvl(G_LOW).log(GLoggerWarning).log(msg);
    }

    //! Сообщает, что дальнейший текст является ошибкой
    /*! \return Ссылку на себя
      */
    GLogger& error(std::string msg = "")
    {
        return endl().lvl(G_LOW).log(GLoggerError).log(msg);
    }

    //! Сообщает, что дальнейший текст является информацией
    /*! \return Ссылку на себя
      */
    GLogger& info(std::string msg = "")
    {
        return endl().lvl(G_HI).log(GLoggerInfo).log(msg);
    }

    //! Сообщает, что дальнейший текст является отладочным
    /*! \return Ссылку на себя
      */
    GLogger& debug(std::string msg = "")
    {
        return endl().lvl(G_DBG).log(GLoggerDebug).log(msg);
    }

    //! Заканчивает сообщение
    /*! \return Ссылку на себя
      */
    GLogger& endl()
    {
        if (m_msgLevel <= m_debugLevel)
            m_file << std::endl;
        lvl(G_LOW);
        return *this;
    }

    //! Устанавливает уровень следующего сообщения
    /*! \return Ссылку на себя
      */
    GLogger& lvl(int lvl)
    {
        m_msgLevel = lvl;
        return *this;
    }

    //! Установка желаемого уровня сообщений
    /*! В лог будут заноситься сообщения с уровнем меньшим или равным параметру
      * \param level Желаемый уровень сообщений
      */
    void setDebugLevel(int level)
    {
        m_debugLevel = level;
        endl().lvl(G_LOW).log("Log: Message level changed to ").log(level);
    }
private:
    //! лог-файл
    std::ofstream m_file;
    //! Уровень текущего сообщения
    int m_msgLevel;
    //! Желаемый уровень сообщений (печатаются сообщения только с уровнем ниже или равным)
    int m_debugLevel;
};

/*! \var logg
  * Экспортируемая переменная, в которую по умолчанию выводятся все исключения и внутренние сообщения
  * Имя log не используется т.к. возникают проблемы с функцией log() из файла <math.h>
  */
extern GLogger logg;

/*@}*/

#endif // GLOGGER_H
