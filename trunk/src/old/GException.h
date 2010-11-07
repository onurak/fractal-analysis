#ifndef GEXCEPTION_H
#define GEXCEPTION_H

#include <string>
#include "GLogger.h"

/*! \addtogroup GLib
  */
/*@{*/

/*! \class GException
  * \brief ������� ����������, ������������ ����������� G
  *
  * ��� ������������� ��� ������������� ��������� � ���
  */
class GException
{
public:
    //! �����������
    /*! \param msg �������� ���������, ������� ����� ����������
      * \param param �������� ����������. ���� �� ������, �� ��������� ������ ���
               msg: param, ����� - ������ msg
      */
    EException(std::string msg, std::string param = ""): m_msg(msg) {
        if (!param.empty())
            m_msg += ": " + param;
        logg.error().log(m_msg).endl();
    }

    //! ���������, ������� ����� ����������
    std::string msg() { return m_msg; }
private:
    std::string m_msg;
};

/*@}*/

#endif // GEXCEPTION_H
