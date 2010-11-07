#ifndef GEXCEPTION_H
#define GEXCEPTION_H

#include <string>
#include "GLogger.h"

/*! \addtogroup GLib
  */
/*@{*/

/*! \class GException
  * \brief Базовое исключение, используемое библиотекой G
  *
  * При возникновении оно автоматически заносится в лог
  */
class GException
{
public:
    //! Конструктор
    /*! \param msg Основное сообщение, которое несет исключение
      * \param param параметр исключения. Если он указан, то сообщение примет вид
               msg: param, иначе - просто msg
      */
    EException(std::string msg, std::string param = ""): m_msg(msg) {
        if (!param.empty())
            m_msg += ": " + param;
        logg.error().log(m_msg).endl();
    }

    //! Сообщение, которое несет исключение
    std::string msg() { return m_msg; }
private:
    std::string m_msg;
};

/*@}*/

#endif // GEXCEPTION_H
