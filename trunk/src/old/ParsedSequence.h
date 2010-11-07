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
    setTerminal,    //!< �������� (����������)
    setNonTerminal, //!< ���������� (���������� ��������� � �����������)
    setUnmarked     //!< ��������������� �������
};

/*! \class SequenceElement
  * \brief ����������� ������� (�������� ��� ����������) ����
  */
class SequenceElement {
public:
    //! �����������
    /*! \param type ��� ��������
      * \param name ��� ��������
      * \param parent ������������ �������, ������� �������� ������, ���� NULL
      * \param startIndexTS ��������� ����� �������� ���������� ����, ������������� ������ ���������
      * \param endIndexTS �������� ����� �������� ���������� ����, ������������� ������ ���������
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

    //! ��� ��������
    SequenceElementType type() const { return m_type; }

    //! �����������
    string name() const { return m_name; }

    //! ��������, ������������ ������ (������ ��� ������������)
    GCollection<SequenceElement*>* subelements() { return m_subelements; }

    //! ������������ �������
    SequenceElement* parent() const { return m_parent; }

    //! �������� ������� ����� ���������� ����, ������������� �������� (������ ���������)
    int startIndexTS() const { return m_startIndexTS; }

    //! ������������� �������� ������� ����� ���������� ����, ������������� �������� (������ ���������)
    void setStartIndexTS(int value) { m_startIndexTS = value; }

    //! �������� ������� ����� ���������� ����, ������������� �������� (����� ���������)
    int endIndexTS() const { return m_endIndexTS; }

    //! ������������� �������� ������� ����� ���������� ����, ������������� �������� (����� ���������)
    void setEndIndexTS(int value) { m_endIndexTS = value; }

    //! ����� �������� (������������ ��� �������� ������� �������)
    int no() const { return m_no; }
    void setNo(int no) { m_no = no; }

    //! �������� ��������� �� ���������������
    bool same(const SequenceElement *e) {
        return m_name == e->m_name && m_no == e->m_no;
    }

private:
    //! ��� ��������
    SequenceElementType m_type;
    //! �����������
    string m_name;
    //! ����� �������� (������������ ��� �������� ������� �������)
    int m_no;
    //! ��������, ������������ ������ (������ ��� ������������)
    GCollection<SequenceElement*>* m_subelements;
    //! ������������ �������
    SequenceElement *m_parent;
    //! �������� ������� ����� ���������� ����, ������������� ��������
    int m_startIndexTS, m_endIndexTS;
};

/*! \class ParsedSequence
  * \brief ����� ������������ ����� �������������� ����������� ���������, ����������� ���������
  *        ������ �����������
  */
class ParsedSequence
{
public:

    GCollection<SequenceElement*>& sequence() { return m_sequence; }

    SequenceElement& operator[](int index) { return *(m_sequence[index]); }

    //! ���� ������� � �������� ������ ���������� ��� ������, ���� -1 ���� ������ �� �������
    int find(string name) {
        for (int i = 0; i < m_sequence.count(); i++)
            if (m_sequence[i]->name() == name)
                return i;
        return -1;
    }

    //! ������� sequence().count()
    int size() {
        return m_sequence.count();
    }

    //! ����������� ���������� ����������� ��������� (�� ������� unmarked-��������)
    int realSize() {
        int result = 0;
        for (int i = 0; i < m_sequence.count(); i++)
            if (m_sequence[i]->type() != setUnmarked)
                result++;
        return result;
    }

    //! ������ ������� realSize() == 0, �� ����������� �������
    bool isEmpty() {
        for (int i = 0; i < m_sequence.count(); i++)
            if (m_sequence[i]->type() != setUnmarked)
                return false;
        return true;
    }

    //! �������� ��������������������� ���������, �������� ��. ���������� ��� �������� ������� ��������
    /*! \param where ������ ���������������������
      * \param size ������ ���������������������
      * \param subseq ���������, � ������� ����� ��������� ���������������������. ��������� ����� �������������� �������
      * \return ���� �������� �������� ������������� ���������������������, �� false, ����� true
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
    //! ������������������ ����������� ���������, ������� ���������� �����
    GCollection<SequenceElement*> m_sequence;
};

/*! \fn unmarked(int startIndex, int endIndex)
  * \brief ���������� ����� unmarked-�������� ������������, ��� ��� ������� ���� �� ������ ���������� ������
  * \param startIndex ��������� ������ ������� ���������� ����
  * \param endIndex �������� ������ ������� ���������� ����
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
