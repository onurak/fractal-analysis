#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "Exceptions.h"
#include "G/GCollection.h"
#include "G/GVariant.h"

using namespace std;

namespace FL {

// ��������� ���������������� ���� ������
const int LEX_NONE = -1; // �����
// ��������� � �������������� ����������
const int LEX_LBR = -2; // (
const int LEX_RBR = -3; // )
const int LEX_EQ = -4; // ==
const int LEX_NEQ = -5; // !=
const int LEX_LESS = -6; // <
const int LEX_GRTR = -7; // >
const int LEX_LEQ = -8; // <=
const int LEX_GEQ = -9; // >=
const int LEX_AND = -10; // &&
const int LEX_OR = -11; // ||
const int LEX_NOT = -12; // !
const int LEX_PLUS = -13; // +
const int LEX_MINUS = -14; // -
const int LEX_MULT = -15; // *
const int LEX_DIV = -16; // /
const int LEX_ASGN = -17; // =
// �����
const int LEX_TERM = -100; // ��� �����
const int LEX_STR = -101; // ������ ��������
const int LEX_INT = -102; // ����� �����
// �����������
const int LEX_EOL = -200; // \n
const int LEX_CLN = -201; // :
const int LEX_SCLN = -202; // ;
const int LEX_LINE = -203; // |
const int LEX_LRT = -204; // [
const int LEX_RRT = -205; // ]
const int LEX_LFG = -206; // {
const int LEX_RFG = -207; // }

//! ����������� ����� ������� ��������� � �������������� ��������� �� ����
/*! ������� ��� ����, ����� ���� ������ ���������� ���������� �������� � ���� �������.
  * ������ ���������� �������� ������������� �� 0 �� N, � �� �� -2 �� -(N+2)
  */
inline int zop(int op) { return abs(op+2); }

/*! \addtogroup FLFiles
  */
/*@{*/

/*! \class AbstractParser
  * \brief ������� ����� ��� �������� ������. ������������� �������� ��� �������� ��� ���������
  *        (��� ������������ � ��������������� �����������)
  */
class AbstractParser
{    
public:
    AbstractParser() {
        m_lexsToRemember.push_back(LEX_TERM);
        m_lexsToRemember.push_back(LEX_INT);
    }

    ~AbstractParser() { close(); }
protected:
    //! �������� ��������
    enum SourceKind {
        FILE,   //!< ����
        STRING  //!< ������
    };

    /* ����� ��������� */
    //! ������� ����, ������� ������ ������
    /*! \param fileName ��� �����
      * \throws EFile
      */
    void open(string fileName) {
        m_file.open(fileName.c_str());
        if (!m_file.is_open())
            throw EFile(ECantOpenFile, fileName);
        m_ln = m_pos = 0;
        gc();
    }

    //! ������� ����. ���� ���� �� ������, �� ������ �� ����������
    void close() {
        if (m_file.is_open())
            m_file.close();
    }

    //! ������������� ����
    /*! ��������� ��������� ���������� � ��������� lexer() � syntax()
      */
    void analyze(string fileName) {
        m_source = FILE;
        open(fileName);
        m_sourceString = fileName;
        lexer();
        close();
        syntax();
    }

    //! ������������� ������
    /*! ��������� ��������� ���������� � ��������� lexer() � syntax()
      */
    void analyzeString(string s) {
        m_source = STRING;
        m_sourceString = s;
        m_stringPos = m_pos = m_ln = 0;
        lexer();
        syntax();
    }

    /***** ��������� ������������ ����������� *****/
    int m_c; //!< ������� ������
    int m_l; //!< ������� �������
    vector<int> m_lexsToRemember; //!< �������, ����� ������� ���� ���������
    string m_repr; //!< ��������� ������������� ������� �������
    int m_lpos; //!< ����� ������� �������
    int m_ln; //!< ��������� � ����� (����� ������)
    int m_pos; //!< ��������� � ����� (����� �������)
    int m_bufpos; //!< ��������� � ������
    int m_stringPos; //!< ��������� � ������ (���� �������� �������� �������)
    /*! ��������� �������.
      * m_lex[i].first - ��� �������
      * m_lex[i].second - ������ � ������� ��������� ������������� (��� �����, ����, ������ � �.�.)
      *                   ���� -1 ���� � ������������� �� ���������
      */
    vector< pair<int, int> > m_lex;
    vector<string> m_terms; //!< ������ � ���������� ��������������� �����, ������ � �.�.
    char m_buffer[1000]; //!< ����� ��� �������� �������� ���������� �������������

    //! ������ ����� ������ ��������� ������� ������������ �������
    virtual void doLexer() = 0;

    //! ��������� �������� ������������ �������
    void lexer() {
        m_ln = m_pos = m_bufpos = 0;
        m_buffer[0] = 0;
        while (!eof())
            doLexer();
    }

    //! �������� ��������� ������ �� �����, ���� -1 ���� ��������� �����
    int gc() {
        // ������ �� �����?
        if (m_source == FILE)
        {
            m_c = m_file.get();
            if (m_file.eof())
                m_c = -1;

        // ������ �� ������?
        } else {
            if (m_stringPos < int(m_sourceString.length())) {
                m_c = m_sourceString[m_stringPos++];
            } else
                m_c = -1;
        }
        // ������� ���������
        if (m_c == '\n') {
            m_pos = 0; m_ln++;
        } else
            m_pos++;
        return m_c;
    }

    //! �������� ������� ������ � �����
    void addc() {
        m_buffer[m_bufpos++] = char(m_c);
        if (m_bufpos >= 999)
            throw EFile(EInvalidFileFormat, atPos(m_ln, m_pos));
    }

    //! �������� �����
    void clearBuf() {
        m_bufpos = 0;
        m_buffer[0] = '\0';
    }

    //! �������� ��� �������. ���� ������� �������� ������, �� ��� ��� ����������� �� �������� ������
    //! � ��������� � ���� ����, � � ����� ��������������� ��������� ���������� ��������������� ������
    void putLex(int type) {
        pair<int, int> p;
        p.first = type;

        if (needToRemember(type)) {
            m_buffer[m_bufpos] = '\0';
            m_terms.push_back(m_buffer);
            clearBuf();
            p.second = m_terms.size() - 1;
        } else
            p.second = -1;
        m_lex.push_back(p);
    }

    //! ��������� �� ����� �����. ���������� check('\n')
    bool eol() { return m_c == '\n'; }

    //! ��������� �� ����� �����. ���������� check(-1)
    bool eof() { return m_c == -1; }

    //! ���������, �������� �� ������� ������ ��� ��, ��� � ��������, � ���� �� ��������, ��
    //! ������������ ���������� EFile
    /*! \param symbol ������ ��� ��������� (��� -1 ��� �������� �� ����� �����)
      * \throws EFile
      */
    void check(int symbol) {
        if (m_c != symbol)
            throw EFile(EInvalidFileFormat, atPos(m_ln, m_pos));
    }

    //! ���������� �������, ���������� ��������� ����� � ����
    void lexToFile(string fileName) {
        ofstream f;
        f.open(fileName.c_str());
        for (int i = 0; i < int(m_lex.size()); i++) {
            f << m_lex[i].first;
            if (m_lex[i].first == LEX_TERM)
                f << '(' << m_lex[i].second << ')';
            f << "   ";
            if (i == 10)
                f << '\n';
        }
        f.close();
    }

    /* ��������� ��������������� ����������� */
    //! ������ ����� ������ ��������� ����������� ��������������/������������� ����������
    virtual void doSyntax() = 0;

    void syntax() {
        m_lpos = -1;
        gl();
        doSyntax();
    }

    //! ����������� ��������� �������, ��������� � m_repr �� ��������� ������������ (���� ����)
    //! \return ������������� ����� �������, ���� -1 ���� ������ ��� ������
    int gl() {
        if (m_lpos >= int(m_lex.size()))
            return m_l = -1;
        if (++m_lpos == int(m_lex.size())) {
            m_repr = "";
            return m_l = -1;
        }
        m_l = m_lex[m_lpos].first;
        if (m_lex[m_lpos].second != -1)
            m_repr = m_terms[m_lex[m_lpos].second];
        else
            m_repr = "";
        return m_l;
    }

    //! ��������, �������� �� ������� ������� ��������� � ���������, � ���� ���, ���������� ����������
    //! \throw EParse
    void lcheck(int type) {
        if (m_l != type)
            throw EParse(m_sourceString, EInvalidFileFormat, -1, -1,
                         string("while lcheck with ") + (char*)GVariant(type));
    }

protected:
    ifstream m_file;
    string m_sourceString;
    SourceKind m_source;
private:
    bool needToRemember(int type) {
        vector<int>::iterator p;
        for (p = m_lexsToRemember.begin(); p != m_lexsToRemember.end(); p++)
            if (*p == type) return true;
        return false;
    }
};

/*@}*/
} // namespace

#endif // FILEPARSER_H
