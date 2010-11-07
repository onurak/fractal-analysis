#ifndef RPN_H
#define RPN_H

#include <string>
#include <vector>
#include <stack>
#include <boost/spirit/include/qi.hpp>
#include "Exceptions.h"
#include "G/GVariant.h"

using namespace std;

namespace FL {

namespace RPN {

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
    enum Errors { NoError = 0,
                  EmptyProgram,
                  InvalidVariableReference,
                  InvalidFunctionReference,
                  InvalidFunction,
                  BrokenStack,
                  UnknownSymbol
    };

    static const int VARIABLE = 0;
    static const int OPERATOR = 1;
    static const int FUNCTION = 2;

    static const int OP_PLUS = 0;
    static const int OP_MINUS = 1;
    static const int OP_DIV = 2;
    static const int OP_MULT = 3;
    static const int OP_UNARY_MINUS = 4;

    //! Элемент записи
    struct Command {
        Command(int type = 0, int concr = -1): type(type), concr(concr) {}
        Command(const Command &elem) { type = elem.type; concr = elem.concr; }
        int type;  //!< Тип элемента
        int concr; //!< Уточнение типа (ссылка на запись в таблице переменных, тип операции и т.д.)
    };

    //! Переменная
    struct Var {
        std::string name;
        GVariant value;
    };

    //! Функция
    struct Function {
        std::string name;
        RPNFunction func;
    };

public:
    //! Конструктор
    Program() {
        reset();
    }

    //! Выполняет программу. Возвращает true если выполнено успешно
    bool execute();

    //! Оператор для быстрого доступа к элементам
    Command& operator[](int index) {   return elements[index];   }

    //! Количество элементов
    int size() {   return elements.size();   }

    //! Быстрое добавление элемента
    void add(int type, int concr) {   elements.push_back(Command(type, concr));   }

    //! Быстрое добавление элемента
    void add(const Command &e) {   elements.push_back(Command(e));   }

    //! Сохраняет программу в файл в скомпилированном виде
    void saveToFile(string fileName);

    //! Загружает скомпилированную программу из файла
    void loadFromFile(string fileName);

    //! Пояснение к ошибке
    string errorDescription(Errors errNo);

    //! Последняя ошибка
    Errors lastError() { return m_lastError; }

    //! Последний результат
    GVariant result() { return m_result; }

    //! Элементы
    vector<Command> elements;

    //! Функции
    vector<Function> functions;

    //! Переменные
    vector<Var> vars;
private:
    //! Выполняет оператор
    bool doOperator(Command &e, std::stack<GVariant> &st);

    //! Выполняет функцию
    bool doFunction(Program::Command &e, stack<GVariant> &st);

    //! Сбрасывает результаты выполнения последней программы
    void reset() {
        m_result = GVariant(G_VAR_NULL);
        m_lastError = NoError;
    }

    //! Проверяет размер стека
    bool checkStackSize(stack<GVariant> &st, int sz) {
        if (int(st.size()) < sz) {
            m_lastError = BrokenStack;
            return false;
        }
        return true;
    }

    //! Проверяет типы на совместимость
    bool checkTypes(const GVariant &v1, const GVariant &v2) {
        return true;
    }

    //! Последняя ошибка
    Errors m_lastError;

    //! Последний результат
    GVariant m_result;
};

} // namespace

} // namespace

#endif // RPN_H
