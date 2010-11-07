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

// некоторые предопределенные типы лексем
const int LEX_NONE = -1; // конец
// операторы в математических выражениях
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
// имена
const int LEX_TERM = -100; // имя терма
const int LEX_STR = -101; // строка символов
const int LEX_INT = -102; // целое число
// разделители
const int LEX_EOL = -200; // \n
const int LEX_CLN = -201; // :
const int LEX_SCLN = -202; // ;
const int LEX_LINE = -203; // |
const int LEX_LRT = -204; // [
const int LEX_RRT = -205; // ]
const int LEX_LFG = -206; // {
const int LEX_RFG = -207; // }

//! Отсчитывает номер лексемы оператора в математическом выражении от нуля
/*! Сделано для того, чтобы было удобно записывать приоритеты операций в виде массива.
  * Номера операторов начинают отсчитываться от 0 до N, а не от -2 до -(N+2)
  */
inline int zop(int op) { return abs(op+2); }

/*! \addtogroup FLFiles
  */
/*@{*/

/*! \class AbstractParser
  * \brief Базовый класс для парсеров файлов. Предоставляет полезный для парсинга БНФ интерфейс
  *        (для лексического и синтаксического анализатора)
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
    //! Источник символов
    enum SourceKind {
        FILE,   //!< Файл
        STRING  //!< Строка
    };

    /* общий интерфейс */
    //! Открыть файл, считать первый символ
    /*! \param fileName Имя файла
      * \throws EFile
      */
    void open(string fileName) {
        m_file.open(fileName.c_str());
        if (!m_file.is_open())
            throw EFile(ECantOpenFile, fileName);
        m_ln = m_pos = 0;
        gc();
    }

    //! Закрыть файл. Если файл не открыт, то ничего не произойдет
    void close() {
        if (m_file.is_open())
            m_file.close();
    }

    //! Анализаровать файл
    /*! Выполняет некоторую подготовку и запускает lexer() и syntax()
      */
    void analyze(string fileName) {
        m_source = FILE;
        open(fileName);
        m_sourceString = fileName;
        lexer();
        close();
        syntax();
    }

    //! Анализировать строку
    /*! Выполняет некоторую подготовку и запускает lexer() и syntax()
      */
    void analyzeString(string s) {
        m_source = STRING;
        m_sourceString = s;
        m_stringPos = m_pos = m_ln = 0;
        lexer();
        syntax();
    }

    /***** ИНТЕРФЕЙС ЛЕКСИЧЕСКОГО АНАЛИЗАТОРА *****/
    int m_c; //!< текущий символ
    int m_l; //!< текущая лексема
    vector<int> m_lexsToRemember; //!< лексемы, имена которых надо сохранить
    string m_repr; //!< текстовой представление текущей лексемы
    int m_lpos; //!< номер текущей лексемы
    int m_ln; //!< положение в файле (номер строки)
    int m_pos; //!< положение в файле (номер столбца)
    int m_bufpos; //!< положение в буфере
    int m_stringPos; //!< положение в строке (если источник является строкой)
    /*! считанные лексемы.
      * m_lex[i].first - тип лексемы
      * m_lex[i].second - индекс в массиве текстовых представлений (для чисел, имен, термов и т.д.)
      *                   либо -1 если в представлении не нуждается
      */
    vector< pair<int, int> > m_lex;
    vector<string> m_terms; //!< Массив с текстовыми представлениями чисел, лексем и т.д.
    char m_buffer[1000]; //!< Буфер для хранения текущего текстового представления

    //! Данный метод должен содержать автомат лексического анализа
    virtual void doLexer() = 0;

    //! Запускает алгоритм лексического анализа
    void lexer() {
        m_ln = m_pos = m_bufpos = 0;
        m_buffer[0] = 0;
        while (!eof())
            doLexer();
    }

    //! Вытащить очередной символ из файла, либо -1 если достигнут конец
    int gc() {
        // читаем из файла?
        if (m_source == FILE)
        {
            m_c = m_file.get();
            if (m_file.eof())
                m_c = -1;

        // читаем из строки?
        } else {
            if (m_stringPos < int(m_sourceString.length())) {
                m_c = m_sourceString[m_stringPos++];
            } else
                m_c = -1;
        }
        // подсчет положения
        if (m_c == '\n') {
            m_pos = 0; m_ln++;
        } else
            m_pos++;
        return m_c;
    }

    //! Добавить текущий символ в буфер
    void addc() {
        m_buffer[m_bufpos++] = char(m_c);
        if (m_bufpos >= 999)
            throw EFile(EInvalidFileFormat, atPos(m_ln, m_pos));
    }

    //! Сбросить буфер
    void clearBuf() {
        m_bufpos = 0;
        m_buffer[0] = '\0';
    }

    //! Положить тип лексемы. Если лексема является термом, то его имя считывается из текущего буфера
    //! и заносится в стек имен, а в стеке транслированной программы появляется соответствующая ссылка
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

    //! Достигнут ли конец линии. Аналогично check('\n')
    bool eol() { return m_c == '\n'; }

    //! Достигнут ли конец файла. Аналогично check(-1)
    bool eof() { return m_c == -1; }

    //! Проверяет, является ли текущий символ тем же, что и параметр, и если не является, то
    //! выкидывается исключение EFile
    /*! \param symbol Символ для сравнения (или -1 для проверки на конец файла)
      * \throws EFile
      */
    void check(int symbol) {
        if (m_c != symbol)
            throw EFile(EInvalidFileFormat, atPos(m_ln, m_pos));
    }

    //! Отладочная функция, сохранаяет структуру файла в файл
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

    /* интерфейс синтаксического анализатора */
    //! Данный метод должен содержать рекурсивный синтаксический/семантический анализатор
    virtual void doSyntax() = 0;

    void syntax() {
        m_lpos = -1;
        gl();
        doSyntax();
    }

    //! Вытаскивает следующую лексему, загружает в m_repr ее текстовое предсавление (если есть)
    //! \return Идентификатор новой лексемы, либо -1 если больше нет лексем
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

    //! Проеряет, является ли текущая лексема указанной в параметре, и если нет, выкидывает исключение
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
