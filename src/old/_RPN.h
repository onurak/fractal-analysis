#ifndef RPN_H
#define RPN_H

#include <vector>
#include <stack>
#include <iostream>
#include "AbstractFactory.h"
#include "AbstractParser.h"
#include "Exceptions.h"
#include "G/GVariant.h"

namespace FL {

/*! \namespace FL::RPN
  * \brief Алгоритмы испольнения обратной польской нотации (Reverse Polish Notation, RPN)
  */
namespace RPN {

/*! \addtogroup FLAlgorithm
  */
/*@{*/

//! Указатель на функцию
typedef GVariant (*RPNFunction)(vector<GVariant>);

/*! \class Program
  * \brief Обратная польская нотация (Reverse Polish Notation, RPN)
  *
  * Общее определение (Википедия):
  * Чтобы дать индуктивное определение постфиксной нотации, обозначим выражения в инфиксной нотации
  * E, E1, E2, эквивалентные им выражения в постфиксной нотации E' , E'1, E'2 соответственно;
  * o — произвольный бинарный оператор, тогда:
  * 1. Если E — переменная или константа, то E' есть E.
  * 2. Если E — выражение вида E1oE2, то E' есть E'1E'2o.
  * 2. Если E — выражение вида (E1), то E' есть E'1.
  *
  * Массив elements() состоит из RPNElement, в которых зашифрован тип выражения: в поле
  * RPNElement::type указан общий тип (оператор, переменная, функция и т.д.), a поле
  * RPNElement::concr этот тип уточнаяется (какой оператор, ссылка на значение переменной,
  * ссылка на функцию и т.д.)
  *
  * В скомпилированном виде особенность имеют функции. Подразумевается, что функции хранятся как
  * \code
  * -------------------------------------------------------------------------------
  * | (VARIABLE, ref) | ... | (VARIABLE, ref) | (VARIABLE, ref) | (FUNCTION, ref) |
  * -------------------------------------------------------------------------------
  * |    Аргумент 1   |     |    Аргумент n   | Колчиество      |   Ссылка на     |
  * |                 | ... |                 | аргументов      |    функию       |
  * -------------------------------------------------------------------------------
  * \endcode
  */
class Program
{
public:
    //! Элемент записи
    struct RPNElement {
        RPNElement(int type = 0, int concr = -1): type(type), concr(concr) {}
        RPNElement(const RPNElement &elem) {
            this->type = elem.type; this->concr = elem.concr;
        }
        int type; //!< Тип элемента
        int concr; //!< Уточнение типа (ссылка на запись в таблице переменных, тип операции и т.д.)
    };
public:
    Program() {

    }

    //! Оператор для быстрого доступа к элементам
    RPNElement& operator[](int index) {
        return elements[index];
    }

    //! Количество элементов
    int size() {
        return elements.size();
    }

    //! Быстрое добавление элемента
    void add(int type, int concr) {
        elements.push_back(RPNElement(type, concr));
    }

    //! Быстрое добавление элемента
    void add(const RPNElement &e) {
        elements.push_back(RPNElement(e));
    }

    //! Сохраняет программу в файл в скомпилированном виде
    void saveToFile(string fileName);

    //! Загружает скомпилированную программу из файла
    void loadFromFile(string fileName);

    //! Элементы
    vector<RPNElement> elements;

    //! Функции
    vector<RPNFunction> functions;

    //! Переменные
    vector<GVariant> vars;
};


/*! \class Executor
  * \brief Исполнитель обратной польской нотации (Reverse Polish Notation, RPN)
  */
class Executor
{
public:
    // типы
    static const int VARIABLE = -1;
    static const int FUNCTION = -2;
    static const int OPERATOR = -5;
    // уточнения
    static const int OP_PLUS = -1;
    static const int OP_MINUS = -2;
    static const int OP_MULT = -3;
    static const int OP_DIV = -4;
    static const int OP_UNARY_MINUS = -5;
    // ошибки
    static const int NO_ERROR = 0;
    static const int EMPTY_PROGRAM = 1;
    static const int BROKEN_STACK = 2;
    static const int UNKNOWN_SYMBOL = 3;
    static const int INVALID_VARIABLE_REFERENCE = 4;
    static const int INVALID_FUNCTION_REFERENCE = 5;
    static const int INVALID_FUNCTION = 6;
public:
    //! Конструктор
    /*! \param program Если не равно NULL, то будет исполнена программа program
      */
    Executor(Program *program = NULL);

    //! Исполняет программу program
    /*! \param program Программа, которую надо исполнить
      * \return Если программа выполнена без ошибок, то true, иначе false
      */
    bool exec(Program *program);

    //! Результат последней выполненой программы, либо GVariant(G_VAR_NULL) если программа еще не была исполнена
    GVariant result() { return m_result; };

    //! Была ли исполнена программа
    bool isExecuted() { return m_isExecuted; }

    //! Ошибка последней выполненой программы, либо Executor::NO_ERROR если ошибки не было или
    //! программа не была исполнена
    int lastError() { return m_lastError; }

    //! Пояснение к ошибке выпонения
    string errorDescription(int errNo);
private:
    //! Сбрасывает результаты выполнения последней программы
    void reset() {
        m_isExecuted = false;
        m_result = GVariant(G_VAR_NULL);
        m_lastError = NO_ERROR;
    }

    //! Проверяет размер стека
    bool checkStackSize(stack<GVariant> &st, int sz) {
        if (int(st.size()) < sz) {
            m_lastError = BROKEN_STACK;
            return false;
        }
        return true;
    }

    //! Выполняет оператор
    bool doOperator(Program::RPNElement &e, stack<GVariant> &st);

    //! Выполняет функцию
    bool doFunction(Program *program, Program::RPNElement &e, stack<GVariant> &st);

    //! Проверяет типы на совместимость
    bool checkTypes(const GVariant &v1, const GVariant &v2) {
        return true;
    }

    bool m_isExecuted;
    int m_lastError;
    GVariant m_result;
};

/*! \class Compiler
  * \brief Компилирует привычную математическую форму записи в RPN
  *
  */
class Compiler: public AbstractParser {
public:
    Compiler(): AbstractParser(), m_p(NULL) { }
    void compile(string text, Program *p);
protected:
    void doLexer();
    void doSyntax();
private:
    //! Текущая компилируемая программа
    Program *m_p;
    //! Стек компиляции
    stack<Program::RPNElement> m_stack;
    //! Константы
    vector<string> m_constants;
    //! Переменные
    vector<string> m_vars;
    //! Термы
    vector<string> m_terms;

    //! положить элемент в стек
    void push(int type = 0, int concr = -1) {
        m_stack.push(Program::RPNElement(type, concr));
    }

    //! Взять элемент из стека
    Program::RPNElement pop() {
        Program::RPNElement result(m_stack.top());
        m_stack.pop();
        return result;
    }

    //! Превратить элемент трансляции в элемент программы
    /*! Преобразует элемент из стека в элемент программы, добавляет при этом при необходимости
      * переменные/функции в программу
      * \param e Элемент трансляции
      * \param repr Его строковое представление (если надо)
      * \return Элемент программы
      */
    Program::RPNElement makeCommand(const Program::RPNElement &e, string repr = "") {
        switch (e.type) {
            case LEX_INT:
                return Program::RPNElement(Executor::VARIABLE, 0);
        }
    }

    //! Добавляет текущую лексему, если она является термом, переменной или константой, в программу,
    //! создает при этом соответствующее имя в программе и в таблице имен компилятора
    void addName() {
        if (m_l == LEX_TERM)
    }

    //! Проеряет приоритет оператора-операнда и оператора на вершине стека и возвращает true,
    //! если оператор на вершине стека надо вытолкнуть в программу
    /*! Общее правило когда возвращается true:
      * 1) (если оператор o1 ассоциированный, либо лево-ассоциированный) приоритет o1 меньше либо
      *    равен приоритету оператора, находящегося на вершине стека
      * 2) (если оператор o1 право-ассоциированый) приоритет o1 меньше приоритета оператора,
            находящегося на вершине стека
      */
    bool needToPop(int op);

    //! Проверяет, является ли лексема типа com оператором выражения
    bool isOperator(int com) {
        return com == LEX_OR    || com == LEX_AND  || com == LEX_NOT  || com == LEX_PLUS ||
               com == LEX_MINUS || com == LEX_MULT || com == LEX_DIV  || com == LEX_EQ   ||
               com == LEX_NEQ   || com == LEX_GRTR || com == LEX_LESS || com == LEX_LEQ  ||
               com == LEX_GEQ;
    }

};

/*@}*/

} //namespace

} // namespace

#endif // RPN_H
