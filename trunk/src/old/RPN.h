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

//! ��������� �� �������
typedef GVariant (*RPNFunction)(vector<GVariant>);

/*! \class Program
  * \brief �������� �������� ������� (Reverse Polish Notation, RPN)
  *
  * ����� ����������� (���������):
  * ����� ���� ����������� ����������� ����������� �������, ��������� ��������� � ��������� �������
  * E, E1, E2, ������������� �� ��������� � ����������� ������� E' , E'1, E'2 ��������������;
  * o � ������������ �������� ��������, �����:
  * 1. ���� E � ���������� ��� ���������, �� E' ���� E.
  * 2. ���� E � ��������� ���� E1oE2, �� E' ���� E'1E'2o.
  * 2. ���� E � ��������� ���� (E1), �� E' ���� E'1.
  *
  * ������ elements() ������� �� RPNElement, � ������� ���������� ��� ���������: � ����
  * RPNElement::type ������ ����� ��� (��������, ����������, ������� � �.�.), a ����
  * RPNElement::concr ���� ��� ����������� (����� ��������, ������ �� �������� ����������,
  * ������ �� ������� � �.�.)
  *
  * � ���������������� ���� ����������� ����� �������. ���������������, ��� ������� �������� ���
  * \code
  * -------------------------------------------------------------------------------
  * | (VARIABLE, ref) | ... | (VARIABLE, ref) | (VARIABLE, ref) | (FUNCTION, ref) |
  * -------------------------------------------------------------------------------
  * |    �������� 1   |     |    �������� n   | ����������      |   ������ ��     |
  * |                 | ... |                 | ����������      |    ������       |
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

    //! ������� ������
    struct Command {
        Command(int type = 0, int concr = -1): type(type), concr(concr) {}
        Command(const Command &elem) { type = elem.type; concr = elem.concr; }
        int type;  //!< ��� ��������
        int concr; //!< ��������� ���� (������ �� ������ � ������� ����������, ��� �������� � �.�.)
    };

    //! ����������
    struct Var {
        std::string name;
        GVariant value;
    };

    //! �������
    struct Function {
        std::string name;
        RPNFunction func;
    };

public:
    //! �����������
    Program() {
        reset();
    }

    //! ��������� ���������. ���������� true ���� ��������� �������
    bool execute();

    //! �������� ��� �������� ������� � ���������
    Command& operator[](int index) {   return elements[index];   }

    //! ���������� ���������
    int size() {   return elements.size();   }

    //! ������� ���������� ��������
    void add(int type, int concr) {   elements.push_back(Command(type, concr));   }

    //! ������� ���������� ��������
    void add(const Command &e) {   elements.push_back(Command(e));   }

    //! ��������� ��������� � ���� � ���������������� ����
    void saveToFile(string fileName);

    //! ��������� ���������������� ��������� �� �����
    void loadFromFile(string fileName);

    //! ��������� � ������
    string errorDescription(Errors errNo);

    //! ��������� ������
    Errors lastError() { return m_lastError; }

    //! ��������� ���������
    GVariant result() { return m_result; }

    //! ��������
    vector<Command> elements;

    //! �������
    vector<Function> functions;

    //! ����������
    vector<Var> vars;
private:
    //! ��������� ��������
    bool doOperator(Command &e, std::stack<GVariant> &st);

    //! ��������� �������
    bool doFunction(Program::Command &e, stack<GVariant> &st);

    //! ���������� ���������� ���������� ��������� ���������
    void reset() {
        m_result = GVariant(G_VAR_NULL);
        m_lastError = NoError;
    }

    //! ��������� ������ �����
    bool checkStackSize(stack<GVariant> &st, int sz) {
        if (int(st.size()) < sz) {
            m_lastError = BrokenStack;
            return false;
        }
        return true;
    }

    //! ��������� ���� �� �������������
    bool checkTypes(const GVariant &v1, const GVariant &v2) {
        return true;
    }

    //! ��������� ������
    Errors m_lastError;

    //! ��������� ���������
    GVariant m_result;
};

} // namespace

} // namespace

#endif // RPN_H
