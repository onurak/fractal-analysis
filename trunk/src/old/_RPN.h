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
  * \brief ��������� ����������� �������� �������� ������� (Reverse Polish Notation, RPN)
  */
namespace RPN {

/*! \addtogroup FLAlgorithm
  */
/*@{*/

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
    //! ������� ������
    struct RPNElement {
        RPNElement(int type = 0, int concr = -1): type(type), concr(concr) {}
        RPNElement(const RPNElement &elem) {
            this->type = elem.type; this->concr = elem.concr;
        }
        int type; //!< ��� ��������
        int concr; //!< ��������� ���� (������ �� ������ � ������� ����������, ��� �������� � �.�.)
    };
public:
    Program() {

    }

    //! �������� ��� �������� ������� � ���������
    RPNElement& operator[](int index) {
        return elements[index];
    }

    //! ���������� ���������
    int size() {
        return elements.size();
    }

    //! ������� ���������� ��������
    void add(int type, int concr) {
        elements.push_back(RPNElement(type, concr));
    }

    //! ������� ���������� ��������
    void add(const RPNElement &e) {
        elements.push_back(RPNElement(e));
    }

    //! ��������� ��������� � ���� � ���������������� ����
    void saveToFile(string fileName);

    //! ��������� ���������������� ��������� �� �����
    void loadFromFile(string fileName);

    //! ��������
    vector<RPNElement> elements;

    //! �������
    vector<RPNFunction> functions;

    //! ����������
    vector<GVariant> vars;
};


/*! \class Executor
  * \brief ����������� �������� �������� ������� (Reverse Polish Notation, RPN)
  */
class Executor
{
public:
    // ����
    static const int VARIABLE = -1;
    static const int FUNCTION = -2;
    static const int OPERATOR = -5;
    // ���������
    static const int OP_PLUS = -1;
    static const int OP_MINUS = -2;
    static const int OP_MULT = -3;
    static const int OP_DIV = -4;
    static const int OP_UNARY_MINUS = -5;
    // ������
    static const int NO_ERROR = 0;
    static const int EMPTY_PROGRAM = 1;
    static const int BROKEN_STACK = 2;
    static const int UNKNOWN_SYMBOL = 3;
    static const int INVALID_VARIABLE_REFERENCE = 4;
    static const int INVALID_FUNCTION_REFERENCE = 5;
    static const int INVALID_FUNCTION = 6;
public:
    //! �����������
    /*! \param program ���� �� ����� NULL, �� ����� ��������� ��������� program
      */
    Executor(Program *program = NULL);

    //! ��������� ��������� program
    /*! \param program ���������, ������� ���� ���������
      * \return ���� ��������� ��������� ��� ������, �� true, ����� false
      */
    bool exec(Program *program);

    //! ��������� ��������� ���������� ���������, ���� GVariant(G_VAR_NULL) ���� ��������� ��� �� ���� ���������
    GVariant result() { return m_result; };

    //! ���� �� ��������� ���������
    bool isExecuted() { return m_isExecuted; }

    //! ������ ��������� ���������� ���������, ���� Executor::NO_ERROR ���� ������ �� ���� ���
    //! ��������� �� ���� ���������
    int lastError() { return m_lastError; }

    //! ��������� � ������ ���������
    string errorDescription(int errNo);
private:
    //! ���������� ���������� ���������� ��������� ���������
    void reset() {
        m_isExecuted = false;
        m_result = GVariant(G_VAR_NULL);
        m_lastError = NO_ERROR;
    }

    //! ��������� ������ �����
    bool checkStackSize(stack<GVariant> &st, int sz) {
        if (int(st.size()) < sz) {
            m_lastError = BROKEN_STACK;
            return false;
        }
        return true;
    }

    //! ��������� ��������
    bool doOperator(Program::RPNElement &e, stack<GVariant> &st);

    //! ��������� �������
    bool doFunction(Program *program, Program::RPNElement &e, stack<GVariant> &st);

    //! ��������� ���� �� �������������
    bool checkTypes(const GVariant &v1, const GVariant &v2) {
        return true;
    }

    bool m_isExecuted;
    int m_lastError;
    GVariant m_result;
};

/*! \class Compiler
  * \brief ����������� ��������� �������������� ����� ������ � RPN
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
    //! ������� ������������� ���������
    Program *m_p;
    //! ���� ����������
    stack<Program::RPNElement> m_stack;
    //! ���������
    vector<string> m_constants;
    //! ����������
    vector<string> m_vars;
    //! �����
    vector<string> m_terms;

    //! �������� ������� � ����
    void push(int type = 0, int concr = -1) {
        m_stack.push(Program::RPNElement(type, concr));
    }

    //! ����� ������� �� �����
    Program::RPNElement pop() {
        Program::RPNElement result(m_stack.top());
        m_stack.pop();
        return result;
    }

    //! ���������� ������� ���������� � ������� ���������
    /*! ����������� ������� �� ����� � ������� ���������, ��������� ��� ���� ��� �������������
      * ����������/������� � ���������
      * \param e ������� ����������
      * \param repr ��� ��������� ������������� (���� ����)
      * \return ������� ���������
      */
    Program::RPNElement makeCommand(const Program::RPNElement &e, string repr = "") {
        switch (e.type) {
            case LEX_INT:
                return Program::RPNElement(Executor::VARIABLE, 0);
        }
    }

    //! ��������� ������� �������, ���� ��� �������� ������, ���������� ��� ����������, � ���������,
    //! ������� ��� ���� ��������������� ��� � ��������� � � ������� ���� �����������
    void addName() {
        if (m_l == LEX_TERM)
    }

    //! �������� ��������� ���������-�������� � ��������� �� ������� ����� � ���������� true,
    //! ���� �������� �� ������� ����� ���� ���������� � ���������
    /*! ����� ������� ����� ������������ true:
      * 1) (���� �������� o1 ���������������, ���� ����-���������������) ��������� o1 ������ ����
      *    ����� ���������� ���������, ������������ �� ������� �����
      * 2) (���� �������� o1 �����-��������������) ��������� o1 ������ ���������� ���������,
            ������������ �� ������� �����
      */
    bool needToPop(int op);

    //! ���������, �������� �� ������� ���� com ���������� ���������
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
