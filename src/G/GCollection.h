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

#ifndef GCOLLECTION_H
#define GCOLLECTION_H

#include <vector>
#include <string>
#include "GResources.h"
#include "GDelegate.h"
#include "GException.h"

/*! \addtogroup GLib
  */
/*@{*/

/** \class GCollection
  * \brief Класс-коллекция наподобие std::vector, но с более удобным доступом к элементам
  */
template <class T>
class GCollection
{
public:
    typedef GDelegate1<void, int> GNotifyChange;
public:
    //! Конструктор
    GCollection() {}

    //! Деструктор. Вызывает clear()
    virtual ~GCollection() {
        onChange = NULL;
        clear();
    }

    //! Удаляет все элементы коллекции
    void clear() {
        while (count() > 0)
            remove(0);
    }

    //! Оператор доступа по индексу. Ведет контроль выхода индекса за пределы коллекции
    /*! \param index Номер элемента
      * \return Элемент с индексом index
      * \throws GException
      */
    T operator[](int index) {
        if (index >= 0 && index < count())
            return m_items[index];
        GError(GCritical, "GCollection", G_INDEX_OUT_OF_BOUNDS, GIndexOutOfBounds);
    }

    //! Добавляет элемент
    /*! \param item Элемент для добавления
      * \return Ссылку на себя
      */
    GCollection& add(T item) {
        m_items.push_back(item);
        onChange(count() - 1);
        return *this;
    }

    //! Удаляет элемент из коллекции. Ведет контроль выхода индекса за пределы коллекции
    /*! \param index Номер элемента
      * \return Ссылку на себя
      * \throws GException
      */
    GCollection& remove(int index) {
        if (index >= 0 && index < count())
        {
            int i;
            typename std::vector<T>::iterator p;
            for (i = 0, p = m_items.begin(); i != index && p != m_items.end(); )
                i++, p++;
            onChange(index);
            delete *p;
            m_items.erase(p);
            return *this;
        }
        GError(GCritical, "GCollection", G_INDEX_OUT_OF_BOUNDS, GIndexOutOfBounds);
        return *this;
    }

    //! Возвращает число элементов в коллекции
    /*! \return число элементов в коллекции
      */
    int count() const {
        return m_items.size();
    }

    //! Поиск элемента
    /*! \return Номер найденного элемента или -1
      */
    int find(T item) {
        for (int i = 0; i < count(); i++)
            if (m_items[i] == item)
                return i;
        return -1;
    }
public:
    //! Событие, возникающее при добавлении/удалении из коллекции объекта.
    /*! При добавлении вызывается после того, как элемент уже добавлен, при удалении - до того, как
      * он удален. Параметр типа int - индекс элемента
      */
    GNotifyChange onChange;
private:
    //! Внутреннее представление элементов
    std::vector<T> m_items;
};

/*@}*/

#endif // GCOLLECTION_H
