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
    // ��������� ������� ����� ���������
    f << elements.size();
    for (unsigned int i = 0; i < elements.size(); i++)
        f << elements[i].type << elements[i].concr;
    // ��������� ���������� ���������� � �� ��������� ��������
    f << vars.size();
    for (unsigned int i = 0; i < elements.size(); i++) {
        if (vars[i].type() != G_VAR_PCHAR)
            f << int(G_VAR_DOUBLE) << double(vars[i]);
        else {
            char *s = (char*)vars[i];
            f << int(G_VAR_PCHAR) << strlen(s) << s;
        }
    }
    // ��������� ���� �������
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
    // ��������� ������� ����� ���������
    f >> size;
    while (size > 0) {
        Command e;
        f >> e.type >> e.concr;
        elements.push_back(e);
    }
    // ��������� ���������� ���������� � �� ��������� ��������
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
    // ��������� ���� �������
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
    stack<GVariant> st; // ���� �����������
    // ��������������� ��������� ���������
    for (int i = 0; i < size(); i++)
    {
        Command &e = elements[i];
        switch (e.type)
        {
            // ���������� - ����� �� �������� � �������� � ����
            case VARIABLE:
                if (e.concr < 0 || e.concr >= int(vars.size())) {
                    m_lastError = InvalidVariableReference;
                    return false;
                }
                st.push(vars[e.concr].value);
                break;
            // �������� - ����� ����������� ���������� ��������� �� �����, ��������� ��� ��� ��������,
            // �������� �� ��������� ������ �����
            case OPERATOR:
                if (!doOperator(e, st))
                    return false;
                break;
            // ������� - ����� �� ����� �������, ��������� � ������ ������ ����������, �������� ��
            // ��������� ������ �����
            case FUNCTION:
                if (!doFunction(e, st))
                    return false;
                break;
            default:
                m_lastError = UnknownSymbol;
                return false;

        }
    }
    // ������ ��� ���������� ����� ������ �������� ���� � ����� ����������
    if (st.size() != 1) {
        m_lastError = BrokenStack;
        return false;
    }
    // ��� ������
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

    // ������ ���������� ��� ������� ���� ������� - ���������� ���������� �������
    if (st.size() < 1) {
        m_lastError = InvalidFunction;
        return false;
    }

    // ���� �������
    RPNFunction f = functions[e.concr].func;

    // ��������� ���������� ����������
    int argCount = st.top(); st.pop();

    // �������� ������� ������ ����� ����������?
    if (!checkStackSize(st, argCount)) return false;

    // ��������� ���������
    vector<GVariant> args;
    for (; argCount > 0; argCount--) {
        args.push_back(st.top());
        st.pop();
    }

    // �������� �������
    st.push(f(args));
    return true;
}
