#ifndef GSERIALIZABLE_H
#define GSERIALIZABLE_H

#include "GDataSet.h"

/*! \addtogroup GLib
  */
/*@{*/

/*! \class GSerializer
  * \brief ������� �����-������������. ��������� ��������� ������/������ � GDataSet ��� ���������
  *        �����
  */
class GSerializer
{
public:
    GSerializer(GDataSet &ds);

    GDataSet& ds() { return m_ds; }

    /*! \brief ����� ������������
      * \param name ��� ���������
      * \param value ����������, � ������� ���� ��������/������� ��������
      * \param defaultValue ��������, ������������ ��� ������ ��������� ���� ���� � ���������������
      *        ������ �� ����� �������
      */
    virtual void io(std::string name, GVariant &value,
                    GVariant defaultValue = GVariant(G_VAR_NULL)) = 0;

    /*! \copydoc GSerializer::io(std::string, GVariant&, GVariant) */
    virtual void io(std::string name, int &value, int defaultValue = 0) = 0;
    /*! \copydoc GSerializer::io(std::string, GVariant&, GVariant) */
    virtual void io(std::string name, double &value, double defaultValue = 0) = 0;
    /*! \copydoc GSerializer::io(std::string, GVariant&, GVariant) */
    virtual void io(std::string name, std::string &value, std::string defaultValue = "") = 0;
private:
    GDataSet &m_ds;
};

/*! \class GReader
  * \brief �����-������������ ��� ������
  */
class GReader: public GSerializer
{
public:
    GReader(GDataSet &ds): GSerializer(ds) {}

    void io(std::string name, GVariant &value, GVariant defaultValue = GVariant(G_VAR_NULL)) {
        value = ds().fieldByName(name, defaultValue)->value();
    }

    void io(std::string name, int &value, int defaultValue = 0) {
        value = int(ds().fieldByName(name, defaultValue)->value());
    }

    void io(std::string name, double &value, double defaultValue = 0) {
        value = double(ds().fieldByName(name, defaultValue)->value());
    }

    void io(std::string name, std::string &value, std::string defaultValue = "") {
        value = std::string((char*)(ds().fieldByName(name, defaultValue.c_str())->value()));
    }
};

/*! \class GWriter
  * \brief �����-������������ ��� ������
  */
class GWriter: public GSerializer
{
public:
    GWriter(GDataSet &ds): GSerializer(ds) {}

    void io(std::string name, GVariant &value, GVariant defaultValue = GVariant(G_VAR_NULL)) {
        ds()[name] = value;
    }

    void io(std::string name, int &value, int defaultValue = 0) {
        ds()[name] = value;
    }

    void io(std::string name, double &value, double defaultValue = 0) {
        ds()[name] = value;
    }

    void io(std::string name, std::string &value, std::string defaultValue = "") {
        ds()[name] = value.c_str();
    }
};

/*! \class GSerializable
  * \brief ������� ����� �������������� �������. ��� ���������� ������ �������������� �����������
  *        ����� serialize()
  */
class GSerializable
{
public:
    virtual bool serialize(GSerializer *s) = 0;
};

/*@}*/

#endif // GSERIALIZABLE_H
