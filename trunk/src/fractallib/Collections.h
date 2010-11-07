#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include "Exceptions.h"
#include "G/GDelegate.h"

namespace FL {

/*! \struct BaseNode
  * \brief Узел однонаправленного списка
  */
template <class T>
struct BaseNode
{
    BaseNode(T item = NULL, BaseNode *next = NULL)
        : item(item), next(next)
    {}

    T item;
    BaseNode *next;
};

#define TheIterator BaseIterator<ClassType, IterType, DataType>

/*! \class BaseIterator
  * \brief Служит для быстрого перемещения по элементам
  */
template <class ClassType, class IterType, class DataType>
        class BaseIterator {
public:
    //! Конструктор по умолчанию. Создает итератор, не привязанный к списку
    BaseIterator() {
        m_node = NULL;
        m_isAtBegining = true;
    }

    //! Конструктор. Создает копию итератора
    BaseIterator(const TheIterator &iterator) {
        m_node = iterator.m_node;
        m_isAtBegining = iterator.m_isAtBegining;
    }

    //! Конструктор. Создает итератор, привязанный к конкретному списку
    BaseIterator(const ClassType &ct) {
        m_node = ct.m_first;
        m_isAtBegining = true;
    }

    //! Переходит к следующему элементу, возвращает его значение
    DataType next() {
        if (!hasNext()) {
            m_node = NULL;
            throw GException(EInvalidIndex, "Next element not exists");
        }
        if (m_isAtBegining)
            m_isAtBegining = false;
        else
            m_node = m_node->next;
        return m_node->item;
    }

    //! Возвращает значение текущего элемента
    DataType current() const {
        if (!m_node)
            throw GException(EInvalidIndex, "Current element not exists");
        return m_node->item;
    }

    //! Проверяет наличие следующего элемента
    bool hasNext() const {
        return m_node && m_node->next;
    }

    //! Возвращает true если итератор дошёл до конца списка или не привязан к списку
    bool isNull() const { return m_node == NULL; }

    //! Разыменование (возвращает значение элемента, на который указывает итератор)
    DataType operator*() const {
        if (m_node)
            return m_node->item;
        return NULL;
    }

    //! Сравнение итераторов. Итераторы равны, если они указывают на один и тот же элемент
    bool operator==(const TheIterator &iterator) const {
        return m_node == iterator.m_node;
    }

    //! Сравнение итераторов. Итераторы неравны, если они указывают на разные элементы
    bool operator!=(const TheIterator &iterator) const {
        return m_node != iterator.m_node;
    }

    //! Присвоение итератора
    TheIterator& operator=(const TheIterator &iterator) {
        m_node = iterator.m_node;
        return *this;
    }

    //! Продвижение итератора на n позиций
    TheIterator& operator+=(int n) {
        while (n > 0 && hasNext())
            next();
        if (n != 0)
            throw GException(EInvalidIndex, "Can't advance iterator to N positions");
    }

private:
    //! Узел, на который указывает итератор
    IterType *m_node;
    //! Флаг начального положения итератора
    bool m_isAtBegining;
};

/*! \class List
  * \brief Список
  */
template <class T> class List
{
public:
    typedef BaseNode<T> Node;

    typedef GDelegate1<void, int> NotifyList;

    friend class BaseIterator<List, List::Node, T>;
    typedef BaseIterator<List, List::Node, T> Iterator;
public:
    //! Конструктор
    List() {
        m_first = m_last = NULL;
        m_count = 0;
    }

    //! Деструктор
    virtual ~List() {}

    //! Вставка элемента в конец
    void add(T item) {
        Node *newNode = new Node(item);

        if (m_count++ == 0)
            m_first = newNode;
        else
            m_last->next = newNode;
        m_last = newNode;

        onAfterInsert(m_count-1);
    }

    //! Вставка элемента в заданную позицию
    void insert(T item, int index) {
        if (index < 0 || index > m_count)
            throw GException("TreeLayer", EInvalidIndex);

        Node *newNode = new Node(item);
        if (m_count++ == 0) {
            m_first = m_last = newNode;
        } else if (index == m_count) {
            m_last->next = newNode;
            m_last = newNode;
        } else if (index == 0) {
            newNode->next = m_first;
            m_first = newNode;
        } else {
            Node *prev = m_first;
            while (index-- > 1)
                prev = prev->next;
            newNode->next = prev->next;
            prev->next = newNode;
        }

        onAfterInsert(index);
    }

    //! Удаление элемента на заданной позиции
    void remove(int index) {        
        if (index < 0 || index >= m_count)
            throw GException("TreeLayer", EInvalidIndex);

        onBeforeRemove(index);

        Node *stored, *prev = m_first;
        if (index == 0) {
            stored = m_first;
            m_first = m_first->next;
        } else if (index == m_count - 1) {
            stored = m_last;
            while (prev->next != m_last)
                prev = prev->next;
            m_last = prev;
            m_last->next = NULL;
        } else {
            while (index-- > 1)
                prev = prev->next;
            stored = prev->next;
            prev->next = prev->next->next;
        }
        stored->next = NULL;
        delete stored;
    }

    //! Очистка списка
    void clear() {
        while (m_count > 0)
            remove(0);
    }

    //! Оператор индексирования
    T operator[](int index) const {
        if (index < 0 || index >= m_count)
            throw GException("TreeLayer", EInvalidIndex);

        Node *node = m_first;
        while (index-- > 0)
            node = node->next;
        return node->item;
    }

    //! Итератор на первый элемент
    Iterator begin() const {
        return m_count == 0 ? List<T>::Iterator() : List<T>::Iterator(*this);
    }

    //! Итератор на последний элемент
    Iterator end() const {
        return List<T>::Iterator();
    }

    //! Первый элемент
    T first() const {
        return m_count == 0 ? NULL : m_first->item;
    }

    //! Последний элемент
    T last() const {
        return m_count == 0 ? NULL : m_last->item;
    }

    //! Число элементов
    int count() const { return m_count; }

public:
    NotifyList onAfterInsert;
    NotifyList onBeforeRemove;
private:
    Node *m_first;
    Node *m_last;

    int m_count;
};

//! Поиск элемента
template <class ClassType, class DataType>
BaseIterator<ClassType, typename ClassType::Node, DataType>
find(   //<-- имя функции
        BaseIterator<ClassType, typename ClassType::Node, DataType> begin,
        BaseIterator<ClassType, typename ClassType::Node, DataType> end,
        DataType what
     )
{
    BaseIterator<ClassType, typename ClassType::Node, DataType> i = begin;
    while (i != end && i.hasNext())
        if (*i == what)
            return i;
    return BaseIterator<ClassType, typename ClassType::Node, DataType>();
}

} // namespace

#endif // COLLECTIONS_H
