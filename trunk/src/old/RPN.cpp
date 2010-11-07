#include "RPN.h"

using namespace FL::RPN;

/************ PROGRAM ***************/

using namespace FL::RPN;

void Program::saveToFile(string fileName)
{
    ofstream f;
    f.open(fileName.c_str());
    if (!f.is_open())
        throw EFile(ECantOpenFile, fileName);
    // сохраняем сначала текст программы
    f << elements.size();
    for (unsigned int i = 0; i < elements.size(); i++)
        f << elements[i].type << elements[i].concr;
    // сохраняем количество переменных и их начальные значения
    f << vars.size();
    for (unsigned int i = 0; i < elements.size(); i++) {
        if (vars[i].type() != G_VAR_PCHAR)
            f << int(G_VAR_DOUBLE) << double(vars[i]);
        else {
            char *s = (char*)vars[i];
            f << int(G_VAR_PCHAR) << strlen(s) << s;
        }
    }
    // сохраняем стек функций
    // ...
    f.close();
}

void Program::loadFromFile(string fileName)
{
    ifstream f;
    f.open(fileName.c_str());
    if (!f.is_open())
        throw EFile(ECantOpenFile, fileName);
    int size;
    // загружаем сначала текст программы
    f >> size;
    while (size > 0) {
        Command e;
        f >> e.type >> e.concr;
        elements.push_back(e);
    }
    // загружаем количество переменных и их начальные значения
    f >> size;
    while (size > 0) {
        int type;
        f >> type;
        if (type != G_VAR_PCHAR) {
            double d;
            f >> d;
            vars.push_back(GVariant(d));
        } else {
            int ssz;
            f >> ssz;
            char *buf = new char[ssz+1];
            f >> buf;
            vars.push_back(GVariant(buf));
            delete buf;
        }
    }
    // сохраняем стек функций
    // ...
    f.close();
}

#define STACK_2_OP(op) \
    if (!checkStackSize(st, 2)) return false; \
    checkTypes(v1, v2); \
    v1 = st.top().value; st.pop(); \
    v2 = st.top().value; st.pop(); \
    st.push(v1 op v2);

char errorDescriptions[][100] = {
    "No error",
    "Empty program",
    "Broken stack",
    "Unknown symbol",
    "Invalid variable reference",
    "Invalid function reference",
    "Invalid function"
};

string Program::errorDescription(Errors errNo)
{
    //return errorDescriptions[errNo];
    return "Unknown error";
}

bool Program::execute()
{
    reset();
    if (elements.size() == 0) {
        m_lastError = EmptyProgram;
        return false;
    }
    stack<GVariant> st; // стек результатов
    // последовательно выполняем программу
    for (int i = 0; i < size(); i++)
    {
        Command &e = elements[i];
        switch (e.type)
        {
            // переменная - взять ее значение и положить в стек
            case VARIABLE:
                if (e.concr < 0 || e.concr >= int(vars.size())) {
                    m_lastError = InvalidVariableReference;
                    return false;
                }
                st.push(vars[e.concr].value);
                break;
            // оператор - взять необходимое количество операндов из стека, выполнить для них операцию,
            // положить ее результат наверх стека
            case OPERATOR:
                if (!doOperator(e, st))
                    return false;
                break;
            // функция - найти ее среди функций, исполнить с нужным числом переменных, положить ее
            // результат наверх стека
            case FUNCTION:
                if (!doFunction(e, st))
                    return false;
                break;
            default:
                m_lastError = UnknownSymbol;
                return false;

        }
    }
    // пустой или содержащий более одного элемента стек в конце выполнения
    if (st.size() != 1) {
        m_lastError = BrokenStack;
        return false;
    }
    // все хорошо
    m_result = st.top(); st.pop();
    return true;
}

bool Program::doOperator(Command &e, std::stack<GVariant> &st)
{
    GVariant v1, v2;
    switch (e.concr) {
        case OP_PLUS:
            STACK_2_OP(+);
            break;
        case OP_MINUS:
            STACK_2_OP(-);
            break;
        case OP_DIV:
            STACK_2_OP(/);
            break;
        case OP_MULT:
            STACK_2_OP(*);
            break;
        case OP_UNARY_MINUS:
            if (!checkStackSize(st, 1)) return false;
            v1 = st.top().value; st.pop();
            st.push(-v1);
            break;
    }
    return true;
}

bool Program::doFunction(Program::Command &e, stack<GVariant> &st)
{
    if (e.concr < 0 || e.concr >= int(functions.size())) {
        m_lastError = InvalidFunctionReference;
        return false;
    }

    // должна оставаться как минимум одна позиция - количество аргументов функции
    if (st.size() < 1) {
        m_lastError = InvalidFunction;
        return false;
    }

    // ищем функцию
    RPNFunction f = functions[e.concr].func;

    // считываем количество аргументов
    int argCount = st.top(); st.pop();

    // осталось записей меньше числа аргументов?
    if (!checkStackSize(st, argCount)) return false;

    // считываем аргументы
    vector<GVariant> args;
    for (; argCount > 0; argCount--) {
        args.push_back(st.top());
        st.pop();
    }

    // вызываем функцию
    st.push(f(args));
    return true;
}
