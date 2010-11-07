#ifndef PARSEDSEQUENCE_H
#define PARSEDSEQUENCE_H

#include <string>
#include <map>
#include "G/GCollection.h"
#include "TimeSeries.h"

using namespace std;

namespace FL {

/*! \addtogroup FLAlgorithm
  */
/*@{*/


enum SequenceElementType {
    setTerminal,    //!< терминал (траектория)
    setNonTerminal, //!< нетерминал (выражается терминалы и нетерминалы)
    setUnmarked     //!< немаркированный участок
};

/*! \class SequenceElement
  * \brief Распознаный элемент (терминал или нетерминал) ряда
  */
class SequenceElement {
public:
    //! Конструктор
    /*! \param type Тип элемента
      * \param name Имя элемента
      * \param parent Родительский элемент, который содержит данный, либо NULL
      * \param startIndexTS Начальный номер сегмента временного ряда, охватываемого данным элементом
      * \param endIndexTS Конечный номер сегмента временного ряда, охватываемого данным элементом
      */
    SequenceElement(SequenceElementType type, string name,
                    SequenceElement *parent = NULL,
                    int startIndexTS = -1, int endIndexTS = -1)
    {
        m_type = type;
        m_name = name;
        m_startIndexTS = startIndexTS;
        m_endIndexTS = endIndexTS;
        m_subelements = (type == setNonTerminal) ? (new GCollection<SequenceElement*>) : NULL;
        m_no = -1;
        m_parent = parent;
    }

    SequenceElement(const SequenceElement& elem) {
        m_type = elem.type();
        m_name = elem.name();
        m_startIndexTS = elem.startIndexTS();
        m_endIndexTS = elem.endIndexTS();
        m_subelements = NULL;
        m_no = -1;
        m_parent = elem.parent();
    }

    virtual ~SequenceElement() {
        if (m_subelements)
            delete m_subelements;
    }

    //! Тип элемента
    SequenceElementType type() const { return m_type; }

    //! Обозначение
    string name() const { return m_name; }

    //! Элементы, составляющие данные (только для нетерминалов)
    GCollection<SequenceElement*>* subelements() { return m_subelements; }

    //! Родительский элемент
    SequenceElement* parent() const { return m_parent; }

    //! Диапазон номеров точек временного ряда, принадлежащих элементу (начало диапазона)
    int startIndexTS() const { return m_startIndexTS; }

    //! Устанавливает диапазон номеров точек временного ряда, принадлежащих элементу (начало диапазона)
    void setStartIndexTS(int value) { m_startIndexTS = value; }

    //! Диапазон номеров точек временного ряда, принадлежащих элементу (конец диапазона)
    int endIndexTS() const { return m_endIndexTS; }

    //! Устанавливает диапазон номеров точек временного ряда, принадлежащих элементу (конец диапазона)
    void setEndIndexTS(int value) { m_endIndexTS = value; }

    //! Номер элемента (используется для проверки условий стражей)
    int no() const { return m_no; }
    void setNo(int no) { m_no = no; }

    //! Проверка элементов на тождественность
    bool same(const SequenceElement *e) {
        return m_name == e->m_name && m_no == e->m_no;
    }

private:
    //! Тип элемента
    SequenceElementType m_type;
    //! Обозначение
    string m_name;
    //! Номер элемента (используется для проверки условий стражей)
    int m_no;
    //! Элементы, составляющие данные (только для нетерминалов)
    GCollection<SequenceElement*>* m_subelements;
    //! Родительский элемент
    SequenceElement *m_parent;
    //! Диапазон номеров точек временного ряда, принадлежащих элементу
    int m_startIndexTS, m_endIndexTS;
};

/*! \class ParsedSequence
  * \brief Класс представляет собой многоуровневую рекурсивную структуру, описывающую результат
  *        работы анализатора
  */
class ParsedSequence
{
public:

    GCollection<SequenceElement*>& sequence() { return m_sequence; }

    SequenceElement& operator[](int index) { return *(m_sequence[index]); }

    //! Ищет элемент с заданным именем возвращает его индекс, либо -1 если ничего не найдено
    int find(string name) {
        for (int i = 0; i < m_sequence.count(); i++)
            if (m_sequence[i]->name() == name)
                return i;
        return -1;
    }

    //! Синоним sequence().count()
    int size() {
        return m_sequence.count();
    }

    //! Фактическое количество размеченных элементов (не включая unmarked-элементы)
    int realSize() {
        int result = 0;
        for (int i = 0; i < m_sequence.count(); i++)
            if (m_sequence[i]->type() != setUnmarked)
                result++;
        return result;
    }

    //! Аналог проерки realSize() == 0, но выполняется быстрее
    bool isEmpty() {
        for (int i = 0; i < m_sequence.count(); i++)
            if (m_sequence[i]->type() != setUnmarked)
                return false;
        return true;
    }

    //! Выделяет подпоследовательность элементов, нумерует их. Необходимо для проверки условий стражами
    /*! \param where Начало подпоследовательности
      * \param size Размер подпоследовательности
      * \param subseq Коллекция, в которую будет добавлена подпоследовательность. Коллекция будет предварительно очищена
      * \return Если неудаось выделить запрашиваемую подпоследовательность, то false, иначе true
      */
    bool getSubsequence(int where, int size, GCollection<SequenceElement*> &subseq) {
        if (where + size > m_sequence.count())
            return false;
        map<string, int> enumerator;
        for (int i = where; i < where + size; i++) {
            SequenceElement *elem = new SequenceElement(*(m_sequence[i]));
            elem->setNo(++enumerator[elem->name()]);
            subseq.add(elem);
        }
        return true;
    }

private:
    //! последовательность распознаных элементов, которая составляет класс
    GCollection<SequenceElement*> m_sequence;
};

/*! \fn unmarked(int startIndex, int endIndex)
  * \brief Возвращает новый unmarked-терминал обозначающий, что для участка ряда не найден подходящий шаблон
  * \param startIndex Начальный индекс участка временного ряда
  * \param endIndex Конечный индекс участка временного ряда
  */
inline SequenceElement* unmarked(int startIndex, int endIndex)
{
    return new SequenceElement(
            setUnmarked,
            "UM",
            NULL,
            startIndex,
            endIndex
    );
}

/*@}*/

} // namespace

#endif // PARSEDSEQUENCE_H
