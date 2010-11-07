#ifndef GUARDS_H
#define GUARDS_H

#include "Terms.h"
#include "Exceptions.h"
#include "ParsedSequence.h"
#include "Template.h"

namespace FL {

class Guard;
class Template;

/*! \addtogroup FLAlgorithm
  */
/*@{*/

/*! \class Guard
  * \brief ����� �������
  *
  * ��������� �������, ������ ����� ��������� ����������
  *
  * ��������� ������� ����� ���
  * \code
  * function(args) (< | > | == | != | <= | >=) (function(args) | float_number)
  * \endcode
  * ����� args - ��� ��������� �����
  * function() - ���� �� ������������ ������ (��. FL::Terms).
  *
  * ������� ����� ���� ���������� ���������� and � or, � ����� ����� �������� ����� ������ not.
  * ������� ���������� ����� ���� ����� ��� ������ ������.
  *
  * ������ � ����� EBNF:
  * \code
  * math_op  = "==" | "!=" | ">=" | "<=" | ">" | "<"
  * logic_op = "and" | "or"
  * guard    = term math_op (term | double) (logic_op bnf) | "not" bnf | "(" bnf ")"
  * \endcode
  */
class Guard
{
public:
    //! �������� ��������� � �������
    enum ConditionOp {Less, Greater, Equals, NotEquals, LessOrEquals, GreaterOrEquals};

    //! ���� ������� ������
    struct Condition {
        FL::Terms::TermCall left; //!< ����� ����� (������ ����)
        ConditionOp op;           //!< �������� ��������� ����� � ������ �����
        GVariant right;           //!< ������ ����� (���� ��� �����)

        Condition() {
        }

        Condition(const Condition &cond) {
            left = cond.left;
            op = cond.op;
            right = cond.right;
        }

        //! ���������, ����������� �� ������� � ������������������
        bool check(ParseContext &context, GCollection<SequenceElement*> &subseq);
    private:
        //! ��������� �������� �������
        double calcFunc(ParseContext &context, Terms::TermCall &func, GCollection<SequenceElement*> &subseq);
    };
public:
    Guard(FL::Template *owner): m_template(owner) {}

    //! ���������, ����������� �� ������� ������ � ������������������
    /*! \param context �������� �����������
      */
    bool check(ParseContext &context);

    vector<Condition>& conditions() { return m_conditions; }
private:
    vector<Condition> m_conditions;
    FL::Template *m_template;
};

/*@}*/

} // namespace

#endif // GUARDS_H
