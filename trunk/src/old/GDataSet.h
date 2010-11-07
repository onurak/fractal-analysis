#ifndef GDATASET_H
#define GDATASET_H

#include <string>
#include "GException.h"
#include "GCollection.h"
#include "GVariant.h"
#include "GDelegate.h"

/*! \addtogroup GLib
  */
/*@{*/

/** \class GField
  * \brief ���� ������ ������. �������� ������ � ���������� ���������
  */
class GField {
public:
    //! ������� ����, ������������ �� ��� ���������
    typedef GDelegate1<void, GField&> NotifyChange;

    //! �����������. ��������� ����� ���������������� ���� ������ � ���������
    /*! \param name ��� ����
      * \param value ��������� �������� ����
      */
    GField(std::string name = "", GVariant value = G_VAR_NULL) {
        m_name = name;
        m_value = value;
        onChange = NULL;
    }

    //! ��� ����
    /*! \return ��� ����
      */
    std::string name() const { return m_name; }

    //! ������������� ��� ����
    /*! \param value ����� ��� ����
      */
    void setName(std::string value) {
        m_name = value;
    }

    //! ���������� ������� �������� ����
    /*! \return ������� �������� ����
      */
    GVariant value() { return m_value; }

    //! ������������� ����� �������� ���� � ���������� ������� onChange
    /*! \param v ����� �������� ����
      * \return ������ �� ����
      */
    GField& setValue(const GVariant &v) {
        m_value = v;
        onChange(*this);
        return *this;
    }

    //! �������� ������������. ��������� ������������ (���������� ������ �� ����).
    //! ���������� ������� onChange
    /*! \param value ����� �������� ����
      * \return ������ �� ����
      */
    GField& operator=(const GVariant &value) {        
        m_value = value;
        onChange(*this);
        return *this;
    }
public:
    //! �������, ������������ ��� ��������� �������� ����
    NotifyChange onChange;
private:
    //! ��� ����
    std::string m_name;
    //! �������� ����
    GVariant m_value;
};

/** \class GDataSet
  * \brief ����� ����� ������������� ���� � ���������� ��������
  *
  * ��������� ����� ������� ����� ������. ������, ��������, ��� �������� ��������.
  *
  * ��� �������� ����������� ����� ������: ����������� �� ���� � ���������� ������
  * (�� ��������� - ���) � ���������������� � �������� � ������ ����� (�� ��������� - ���).
  *
  * ������������� ���������� �����, ���������� �������������� � ������������, � ����� ������,
  * ��������� ������ ����������� ����:
  * \code
  *  GDataSet params;
  *  params.addField("IntField", G_VAR_INT);
  *  params.addField("FloatField", G_VAR_FLOAT);
  *  params["IntField"] = 10;
  *  params["FloatField"] = 10.5 + params["IntField"];
  * \endcode
  */
class GDataSet
{
public:
    //! ������ ������ GDataSet
    enum GDataSetMode {
        //! �� �������� ���� ���� � ����� ������ ��� ����������
        gdmSkipDuplicates = 1,
        //! ������������������ � �������� � ������ �����
        gdmCaseInsensetive = 2,
        //! ������������ ����� ��� ���������. ������ ���� �������� ������ ��� �������
        //! fieldByName(std::string), operator[](std::string) � valueOf()
        gdmAutocreateFields = 4,
        gdmAll = gdmSkipDuplicates | gdmCaseInsensetive | gdmAutocreateFields
    };
public:
    //! �����������
    /*! \param mode ����� ������
      * \sa GDataSetMode
      */
    GDataSet(GDataSetMode mode = gdmAll) {
        m_mode = mode;
    }

    //! ������� ��� ����
    void removeAll() {
        m_fields.clear();
    }

    //! ��������� ����� ����
    /*! ���� ���� � ����� ������ ���������� � ����� ����� gdmSkipDuplicates, �� �������������
      * ���������� GException.
      * \param name ��� ����
      * \param type ��� �������� � ���� (����� ���� ������� ���������� ������ ��������)
      * \return ������ �� ��������� ����
      * \sa GDataSetMode
      */
    GField& addField(std::string name, GVarType type = G_VAR_NULL) {
        GField *result = fieldByName(name);
        if (!(m_mode & gdmSkipDuplicates)) {
            if (result)
                throw EException(GDataSetFieldAlreadyExists);
            return *result;
        } else {
            if (result)
                return *result;
            return justAddNewField(name, type);
        }
    }

    //! ���������� ��������� �� ���� �� ��� ����� ��� NULL
    /*! ���� ����� ����� gdmCaseInsensetive, �� ���� ������ ���������� �� ��������
      * \param name ��� ��� ������
      * \param defaultValue ��������, ������������� ���� ���� ����� ����� ������� (��. gdmAutocreateFields)
      * \return ��������� �� ���� ��� NULL ���� ���� � ����� ������ ��� � �� ����� �����
      *         gdmAutocreateFields
      * \sa GDataSetMode
      */
    GField* fieldByName(std::string name, GVariant defaultValue = GVariant(G_VAR_NULL)) {
        if (m_mode & gdmCaseInsensetive)
            name = lowerCase(name);
        std::string fieldName;
        for (int i = 0; i < m_fields.count(); i++) {
            fieldName = m_mode & gdmCaseInsensetive ?
                lowerCase(m_fields[i]->name()) : m_fields[i]->name();
            if (fieldName == name)
                return m_fields[i];
        }
        if (m_mode & gdmAutocreateFields) {
            logg.debug("GDataSet: field added: ").log(name).endl();
            return &(justAddNewField(name).setValue(defaultValue));
        }
        return NULL;
    }

    //! ���������� ������ �� ���� �� ��� ����� ��� ����������� ���������� GException
    /*! ���� ����� ����� gdmCaseInsensetive, �� ���� ������ ���������� �� ��������
      * \param name ��� ��� ������
      * \return ������ �� ����
      * \throws GException
      * \sa GDataSetMode, fieldByName
      */
    GField& operator[](std::string name) {
        GField *result = fieldByName(name);
        if (!result) {
            if (m_mode & gdmAutocreateFields)
                return justAddNewField(name);
            else
                throw EException(GDataSetFieldNotExists);
        }
        return *result;
    }

    //! ���������� ������ �� ���� �� ��� ������. ������ �� ������� ������� �� �������
    /*! \param index ����� ����
      * \throws GException
      */
    GField& operator[](int index) {
        return *(m_fields[index]);
    }

    //! ���������� �������� ���� �� ��� �����, ���� defaultValue ���� ������ �������
    /*! \param name ��� ����
      * \param defaultValue ��������, ������������ ���� ���� �� �������
      * \return �������� ���� ���� defaultValue
      * \sa GDataSetMode
      */
    GVariant valueOf(std::string name, GVariant defaultValue = GVariant(G_VAR_NULL)) {
        logg.debug("GDataSet: getting value of ").log(name).endl();
        GField *result = fieldByName(name, defaultValue);
        if (!result)
            return defaultValue;
        return result->value();
    }

    //! ���������� ���������� ����� � ������
    int count() const { return m_fields.count(); }

    //! ���������� ����� ������
    /*! \sa GDataSetMode
      */
    GDataSetMode mode() { return m_mode; }
public:
    //! �������, ������������ ��� ��������� \b ������ ����
    GField::NotifyChange onChange;
private:
    GCollection<GField*> m_fields;
    GDataSetMode m_mode;

    void fieldChanged(GField &f) {
        onChange(f);
    }

    //! ��������� ����, �� ��������, ���������� �� ������ � ����� �� ������
    GField& justAddNewField(std::string name, GVarType type = G_VAR_NULL) {
        GField *result = new GField(name, GVariant(type));
        result->onChange += delegate(this, &GDataSet::fieldChanged);
        m_fields.add(result);
        return *(m_fields[m_fields.count() - 1]);
    }
};

/*@}*/

#endif // GDATASET_H
