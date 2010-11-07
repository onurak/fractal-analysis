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
  * \brief Страж шаблона
  *
  * Временный вариант, стражи будут полностью переделаны
  *
  * Отдельное условие имеет вид
  * \code
  * function(args) (< | > | == | != | <= | >=) (function(args) | float_number)
  * \endcode
  * Здесь args - это аргументы терма
  * function() - один из определенных термов (см. FL::Terms).
  *
  * Условия могут быть объединены операциями and и or, а также перед условием может стоять not.
  * Порядок вычислений может быть задан при помощи скобок.
  *
  * Запись в форме EBNF:
  * \code
  * math_op  = "==" | "!=" | ">=" | "<=" | ">" | "<"
  * logic_op = "and" | "or"
  * guard    = term math_op (term | double) (logic_op bnf) | "not" bnf | "(" bnf ")"
  * \endcode
  */
class Guard
{
public:
    //! Операция сравнения в условии
    enum ConditionOp {Less, Greater, Equals, NotEquals, LessOrEquals, GreaterOrEquals};

    //! Одно условие стража
    struct Condition {
        FL::Terms::TermCall left; //!< левая часть (всегда терм)
        ConditionOp op;           //!< операция сравнения левой и правой части
        GVariant right;           //!< правая часть (терм или число)

        Condition() {
        }

        Condition(const Condition &cond) {
            left = cond.left;
            op = cond.op;
            right = cond.right;
        }

        //! Проверяет, выполняется ли условие в последовательности
        bool check(ParseContext &context, GCollection<SequenceElement*> &subseq);
    private:
        //! Вычисляет значение функции
        double calcFunc(ParseContext &context, Terms::TermCall &func, GCollection<SequenceElement*> &subseq);
    };
public:
    Guard(FL::Template *owner): m_template(owner) {}

    //! Проверяет, выполняются ли условия стража в последовательности
    /*! \param context Контекст распознания
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
