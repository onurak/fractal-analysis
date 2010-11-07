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
  * \brief Поле набора данных. Обладает именем и вариантным значением
  */
class GField {
public:
    //! Событие поле, генерируемое им при изменении
    typedef GDelegate1<void, GField&> NotifyChange;

    //! Конструктор. Позволяет сразу инициализировать поле именем и значением
    /*! \param name Имя поля
      * \param value Начальное значение поля
      */
    GField(std::string name = "", GVariant value = G_VAR_NULL) {
        m_name = name;
        m_value = value;
        onChange = NULL;
    }

    //! Имя поля
    /*! \return имя поля
      */
    std::string name() const { return m_name; }

    //! Устанавливает имя поля
    /*! \param value новое имя поля
      */
    void setName(std::string value) {
        m_name = value;
    }

    //! Возвращает текущее значение поля
    /*! \return текущее значение поля
      */
    GVariant value() { return m_value; }

    //! Устанавливает новое значение поля и генерирует событие onChange
    /*! \param v новое значение поля
      * \return Ссылку на себя
      */
    GField& setValue(const GVariant &v) {
        m_value = v;
        onChange(*this);
        return *this;
    }

    //! Оператор присваивания. Семантика традиционная (возвращает ссылку на себя).
    //! Генерирует событие onChange
    /*! \param value новое значение поля
      * \return ссылку на себя
      */
    GField& operator=(const GVariant &value) {        
        m_value = value;
        onChange(*this);
        return *this;
    }
public:
    //! Событие, генерируемое при изменении значения поля
    NotifyChange onChange;
private:
    //! Имя поля
    std::string m_name;
    //! Значение поля
    GVariant m_value;
};

/** \class GDataSet
  * \brief Набор полей произвольного типа с именованым доступом
  *
  * Позволяет вести простой набор данных. Удобно, например, для хранения настроек.
  *
  * При создании указывается режим работы: допускаются ли поля с одинаковым именем
  * (по умолчанию - нет) и чувствительность к регистру в именах полей (по умолчанию - нет).
  *
  * Использование вариантных типов, операторов индексирования и присваивания, а также ссылок,
  * позволяет писать конструкции вида:
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
    //! Режимы работы GDataSet
    enum GDataSetMode {
        //! не ругаться если поле с таким именем уже существует
        gdmSkipDuplicates = 1,
        //! нечувствительность к регустру в именах полей
        gdmCaseInsensetive = 2,
        //! Автосоздание полей при обращении. Данный флаг работает только для функций
        //! fieldByName(std::string), operator[](std::string) и valueOf()
        gdmAutocreateFields = 4,
        gdmAll = gdmSkipDuplicates | gdmCaseInsensetive | gdmAutocreateFields
    };
public:
    //! Конструктор
    /*! \param mode Режим работы
      * \sa GDataSetMode
      */
    GDataSet(GDataSetMode mode = gdmAll) {
        m_mode = mode;
    }

    //! Удаляет все поля
    void removeAll() {
        m_fields.clear();
    }

    //! Добавляет новое поле
    /*! Если поле с таким именем существует и задан режим gdmSkipDuplicates, то выбрасывается
      * исключение GException.
      * \param name имя поля
      * \param type тип значения в поле (может быть изменен установкой нового значения)
      * \return ссылку на созданное поле
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

    //! Возвращает указатель на поле по его имени или NULL
    /*! Если задан режим gdmCaseInsensetive, то поле ищется независимо от регистра
      * \param name имя для поиска
      * \param defaultValue Значение, воззвращаемое если поле будет вновь создано (см. gdmAutocreateFields)
      * \return указатель на поле или NULL если поля с таким именем нет и не задан режим
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

    //! Возвращает ссылку на поле по его имени или выбрасывает исключение GException
    /*! Если задан режим gdmCaseInsensetive, то поле ищется независимо от регистра
      * \param name имя для поиска
      * \return ссылка на поле
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

    //! Возвращает ссылку на поле по его номеру. Следит за вызодом индекса за границу
    /*! \param index номер поля
      * \throws GException
      */
    GField& operator[](int index) {
        return *(m_fields[index]);
    }

    //! Возвращает значение поля по его имени, либо defaultValue если полене найдено
    /*! \param name имя поля
      * \param defaultValue значение, возвращаемое если поле не найдено
      * \return значение поля либо defaultValue
      * \sa GDataSetMode
      */
    GVariant valueOf(std::string name, GVariant defaultValue = GVariant(G_VAR_NULL)) {
        logg.debug("GDataSet: getting value of ").log(name).endl();
        GField *result = fieldByName(name, defaultValue);
        if (!result)
            return defaultValue;
        return result->value();
    }

    //! Возвращает количество полей в наборе
    int count() const { return m_fields.count(); }

    //! Возвращает режим работы
    /*! \sa GDataSetMode
      */
    GDataSetMode mode() { return m_mode; }
public:
    //! Событие, генерируемое при изменении \b любого поля
    GField::NotifyChange onChange;
private:
    GCollection<GField*> m_fields;
    GDataSetMode m_mode;

    void fieldChanged(GField &f) {
        onChange(f);
    }

    //! Добавляет поле, не проверяя, существует ли другое с таким же именем
    GField& justAddNewField(std::string name, GVarType type = G_VAR_NULL) {
        GField *result = new GField(name, GVariant(type));
        result->onChange += delegate(this, &GDataSet::fieldChanged);
        m_fields.add(result);
        return *(m_fields[m_fields.count() - 1]);
    }
};

/*@}*/

#endif // GDATASET_H
