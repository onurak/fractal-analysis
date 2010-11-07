#include "RPN.h"

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
        RPNElement e;
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
    v1 = st.top(); st.pop(); \
    v2 = st.top(); st.pop(); \
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

Executor::Executor(Program *program)
{

    if (program == NULL)
        reset();
    else
        exec(program);
}

string Executor::errorDescription(int errNo)
{
    return errorDescriptions[errNo];
}

bool Executor::exec(Program *program)
{
    reset();
    if (program == NULL || program->elements.size() == 0) {
        m_lastError = EMPTY_PROGRAM;
        return false;
    }
    stack<GVariant> st; // ���� �����������
    // ��������������� ��������� ���������
    for (int i = 0; i < program->size(); i++)
    {
        Program::RPNElement &e = (*program)[i];
        switch (e.type)
        {
            // ���������� - ����� �� �������� � �������� � ����
            case VARIABLE:
                if (e.concr < 0 || e.concr >= int(program->vars.size())) {
                    m_lastError = INVALID_VARIABLE_REFERENCE;
                    return false;
                }
                st.push(program->vars[e.concr]);
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
                if (!doFunction(program, e, st))
                    return false;
                break;
            default:
                m_lastError = UNKNOWN_SYMBOL;
                return false;

        }
    }
    // ������ ��� ���������� ����� ������ �������� ���� � ����� ����������
    if (st.size() != 1) {
        m_lastError = BROKEN_STACK;
        return false;
    }
    // ��� ������
    m_result = st.top(); st.pop();
    m_isExecuted = true;
    return true;
}

bool Executor::doOperator(Program::RPNElement &e, stack<GVariant> &st)
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
            v1 = st.top(); st.pop();
            st.push(-v1);
            break;
    }
    return true;
}

bool Executor::doFunction(Program *program, Program::RPNElement &e, stack<GVariant> &st)
{
    if (e.concr < 0 || e.concr >= int(program->functions.size())) {
        m_lastError = INVALID_FUNCTION_REFERENCE;
        return false;
    }
    // ������ ���������� ��� ������� ���� ������� - ���������� ���������� �������
    if (st.size() < 1) {
        m_lastError = INVALID_FUNCTION;
        return false;
    }
    // ���� �������
    RPNFunction f = program->functions[e.concr];
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

/************ ���������� **********/
//! ������ � ������������ ��������
char priorities[20] = {
   4,  4,  1,  1,  1,  1,  1,  1,  0,  0,  4,  2,  2,  3,  3,  0
// (   )   ==  !=  <   >   <=  >=  &&  ||  !   +   -   *   /   =
};

void Compiler::compile(string text, Program *p)
{
    m_p = p;
    while (m_stack.size() != 0)
        m_stack.pop();
    m_terms.clear();
    m_constants.clear();
    m_vars.clear();
    analyzeString(text);
    m_p = NULL;
}

void Compiler::doLexer()
{

}


/***************************************************************************************************
��������

���� ���� ��� ������� ��� ������:
    * ������ ��������� ������.
    * ���� ������ �������� ������, �������� ��� � �������� ������.
    * ���� ������ �������� �������� �������, �������� ��� � ����.
    * ���� ������ �������� ����������� �������, �������� ��� � ����.
    * ���� ������ �������� ����������� �������:
    * �� ��� ���, ���� ������� ��������� ����� �� ������ ������������� ������, ����������� ��������
      �� ����� � �������� ������. ��� ���� ������������� ������ ��������� �� �����, �� � ��������
      ������ �� �����������. ���� ����� ����� ���� �� ������� ����� ����������� ������ �������,
      ����������� ��� � �������� ������. ���� ���� ���������� ������, ��� �� ��������� �������������
      ������, ��� ��������, ��� � ��������� ���� ������� ��������� �����������, ���� �������������
      ������.
    * ���� ������ �������� ����������, �1, �����:
        1) �����
           � (���� �������� o1 ���������������, ���� ����-���������������) ��������� o1 ������ ����
              ����� ���������� ���������, ������������ �� ������� ������
           � (���� �������� o1 �����-��������������) ��������� o1 ������ ���������� ���������,
              ������������ �� ������� ������
           � ����������� ������� �������� ����� c ������� ���� ������ ����������� � �������� ������;
        2) �������� �������� o1 � ����.
    * ����� ������� ������ �����������, ���������� ��� ������� �� ����� � �������� ������.
      � ����� ������ ���� �������� ������ ������� ����������; ���� ��� �� ���, ������ � ���������
      ������������� ������.
***************************************************************************************************/

void Compiler::doSyntax()
{
    Program::RPNElement e;
    switch (m_l) {
        // ������� ����������, �������� � ��� ��������, ��������� �� � �������� ������
        case LEX_INT:
            addName();
            m_p->vars.push_back(GVariant(m_repr.c_str()));
            m_p->add(Executor::VARIABLE, m_p->vars.size()-1);
            break;
        // �������
        case LEX_TERM:
            addName();
            break;
        // ����������� ������
        case LEX_LBR:
            push(LEX_LBR);
            break;
        // ����������� ������
        case LEX_RBR:
            while (m_stack.size() > 0 && (e = pop()).type != LEX_LBR)
                m_p->add(makeCommand(e));
            if (m_stack.size() == 0 && e.type == LEX_LBR)
                throw EParse(m_sourceString, "RPN::Compiler", -1, -1, "Unbalanced brackets");
            // ��������� ���������� ���������� �������, � ����� �� ����
            if (m_stack.top().type == LEX_TERM) {
                m_p->add(Executor::VARIABLE, 0);
                m_p->add(makeCommand(pop()));
            }
            break;
        // ��������
        default:
            if (isOperator(m_l))


            while (needToPop(m_l))
                m_p->add(makeCommand(pop()));
            push(m_l);
            break;
    }
}

bool Compiler::needToPop(int op) {
    if (isOperator(m_stack.top().type) &&
        priorities[zop(op)] <= priorities[zop(m_stack.top().type)])
    {
        return true;
    } else
        return false;

}
