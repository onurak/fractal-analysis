#include "FileTPL.h"
#include "tools/spirit.h"

using namespace FL;

namespace PAT {

    struct Element {
        string name;
        int no;
    };

    FL::Patterns *m_patterns;
    FL::Pattern *m_pattern;
    Element m_elem("", -1);
    FL::POLIZ::Program *m_program;
    std::stack<FL::RPN::Program::Command> st;

    /*************** работа с программой **************/

    // добавить конкретную команду
    void add(int type, int concr)
    {
        m_program->add(type, concr);
    }

    // добавить в выходную программу константу
    void add(double value)
    {
        m_program->vars.push_back(value);
        add(Program::VARIABLE, m_program->vars.size() - 1);
    }


    /************* работа со стеком операций *************/

    // положить конкретное значение
    void push(int type, int concr)
    {
        st.push(FL::RPN::Program::Command(type, concr));
    }

    // положить функцию
    void push(std::string funcName)
    {
        Terms::Term term = Terms::TermFactory::termByName(funcName);
        if (term == NULL)
            throw GException("Can't find function", funcName);
        m_program->functions.push_back();
    }

    // вытолкнуть верхнее значение
    FL::RPN::Program::Command pop()
    {
        FL::RPN::Program::Command result = st.top();
        st.pop();
        return result;
    }

    // считать верхнее значение
    FL::RPN::Program::Command top()
    {
        return st.top();
    }

    /*************** actions *******************/

    void onTpl(std::string value)
    {
        m_Pattern = new FL::Pattern();
        m_Pattern->setName(value);
        m_program = new FL::RPN::Program();
    }

    void onTplIndexed()
    {
        m_Pattern->elements().add(
                new Pattern::Element(m_elem.name, Pattern::tePattern, m_elem.no)
        );
    }

    void onTplEnd()
    {
        m_tpls->add(m_Pattern);
    }

    void onIndexedName(std::string name)
    {
        m_elem.name = name;
    }

    void onIndexedNo(unsigned int no)
    {
        m_elem.no = int(no);
    }

    void onExprNum(double d)
    {
        add(d);
    }

    void onExprFunc(std::string name)
    {
        push(name);
    }
    
    void onExprFuncLBR(char c)
    {
        
    }
    
    void onExprFuncRBR(char c)
    {
        
    }

    /************** грамматика TPL-файла ******************/
    template <typename Iterator>
    struct Grammar : qi::grammar<Iterator, ascii::space_type>
    {
        Grammar() : Grammar::base_type(file)
        {
            using ascii::char_;
            using ascii::alpha;
            using ascii::alnum;
            using ascii::digit;
            using qi::double_;
            using qi::uint_;
            using qi::lit;
            using qi::lexeme;
            using ascii::space;

            name        %= alpha >> *alnum;
            indexed      = name[onIndexedName] >> '_' >> uint_[onIndexedNo];
            term         = 
                    name[onExprFunc] >> char_('(')[onExprFuncLBR] >> indexed >> char_(')')[onExprFuncRBR];
            math_op      = lit("<") | lit(">") | lit("==") | lit("!=") | lit("<=") | lit(">=");
            logic_op     = (lit("or") | "and") >> -lit("not");

            file         = *file_record;
            file_record  = bnf >> -('@' >> guard) >> char_(';')[onTplEnd];
            bnf          = name[onTpl] >> '=' >> +(indexed[onTplIndexed]);
            expr         = term >> math_op >> (term | double_[onExprNum])  |  '(' >> expr >> ')';
            guard        = expr >> *(logic_op >> (expr | '(' >> expr >> ')'));
        }

        qi::rule<Iterator, ascii::space_type>
            term, math_op, logic_op, file, file_record, bnf, expr, guard, indexed;
        qi::rule<Iterator, std::string(), ascii::space_type>
            name;
    };

    // парсер
    void parse(std::string fileName, FL::Patterns *Patterns)
    {
        // подготавливаемся
        if (Patterns == NULL)
            throw GException("TPL parser internal error: Invalid Pattern parameter");
        m_tpls = Patterns;
        std::ifstream file(fileName.c_str(), std::ios_base::in);
        if (!file)
            throw EFile(ECantOpenFile, fileName);

        std::string storage; // будем считывать сюда строки из файла

        std::copy(
                std::istream_iterator<char>(file),
                std::istream_iterator<char>(),
                std::back_inserter(storage));

        // готовим грамматику, парсим
        typedef PAT::Grammar<std::string::const_iterator> TPLGrammar;
        TPLGrammar g;            // грамматика

        std::string::const_iterator iter = storage.begin();
        std::string::const_iterator end = storage.end();
        bool r = phrase_parse(iter, end, g, ascii::space);

        file.close();
        // проверяем на ошибки
        if (!r || iter != end)
            throw EFile(EInvalidFileFormat, fileName);
    }

} // namespace


void FilePAT::loadFromFile(std::string fileName, Patterns *tpl)
{
    if (fileName.empty() || tpl == NULL)
        return;
    PAT::parse(fileName, tpl);
}

/*
void FileTPL::doLexer()
{
    switch (m_c)
    {
        case ' ': case '\t':
            gc(); break;
        case '\n':
            putLex(LEX_EOL); gc();  break;
        case ';':
            putLex(LEX_SCLN); gc(); break;
        case '(':
            putLex(LEX_LBR); gc(); break;
        case ')':
            putLex(LEX_RBR); gc(); break;
        case '[':
            putLex(LEX_LRT); gc(); break;
        case ']':
            putLex(LEX_RRT); gc(); break;
        case '{':
            putLex(LEX_LFG); gc(); break;
        case '}':
            putLex(LEX_RFG); gc(); break;
        case '@':
            putLex(LEX_GUARD); gc(); break;
        case '<':
            gc();
            if (m_c == '=') {
                putLex(LEX_LEQ); gc(); break;
            }
            putLex(LEX_LESS);
            break;
        case '>':
            gc();
            if (m_c == '=') {
                putLex(LEX_GEQ); gc(); break;
            }
            putLex(LEX_GRTR);
            break;
        case '=':
            gc();
            if (m_c == '=') {
                putLex(LEX_EQ); gc();
            } else
                putLex(LEX_ASGN);
            break;
        case '!':
            gc();
            if (m_c == '=') {
                putLex(LEX_NEQ); gc(); break;
            }
            putLex(LEX_NOT);
            break;
        default:
            // имя
            if (isalpha(m_c))
            {
                // основная часть имени
                addc(); gc();
                while (isalpha(m_c) || isdigit(m_c)) {
                    addc(); gc();
                }
                // индексная часть (если это индексированное имя)
                if (m_c == '_')
                {
                    putLex(LEX_INDEXED_TERM);
                    gc();
                    if (!isdigit(m_c))
                        throw EParse(m_sourceString, EInvalidFileFormat, m_ln, m_pos, "after '_' can be only number");
                    while (isdigit(m_c)) {
                        addc(); gc();
                    }
                    putLex(LEX_INT);
                } else
                    putLex(LEX_TERM);
            } else
                throw EParse(m_sourceString, EInvalidFileFormat, m_ln, m_pos, "unexpected symbol");
            break;
    }
}

void FileTPL::file()
{
    while (m_l != -1) {
        try {
            m_curTpl = new Pattern();
            file_record();
            m_Patterns->add(m_curTpl);
        } catch (...) {
            delete m_curTpl;
            throw;
        }
    }
}

void FileTPL::file_record()
{
    bnf();
    if (m_l == LEX_GUARD) {
        m_guard = new Guard();
        guard();
    }
    if (m_l != -1 && m_l != LEX_EOL && m_l != LEX_SCLN)
        throw EParse(m_sourceString, EInvalidFileFormat, -1, -1, "unknown symbols after end of file record");
    gl();
}

void FileTPL::indexed()
{
    // имя элемента
    lcheck(LEX_INDEXED_TERM);
    string elName = m_repr;
    gl();
    // номер элемента
    lcheck(LEX_INT);
    int elNo = atoi(m_repr.c_str());
    // тип элемента (является траекторией если нет ни одного заглавного символа в имени)
    Pattern::ElementType elType = Pattern::teTrajectory;
    for (unsigned int i = 0; i < elName.length(); i++) {
        if (elName[i] >= 'A' && elName[i] <= 'Z') {
            elType = Pattern::tePattern;
            break;
        }
    }
    // добавляем
    m_curTpl->elements().add(new Pattern::Element(elName, elType, elNo));
    gl();
}

void FileTPL::bnf()
{
    lcheck(LEX_TERM);
    m_curTpl->setName(m_repr);
    gl();
    lcheck(LEX_ASGN);
    gl();
    indexed();
    while (m_l == LEX_INDEXED_TERM)
        indexed();
    while (m_l == LEX_LINE || m_l == LEX_LBR || m_l == LEX_LRT || m_l == LEX_LFG)
    {
        switch (m_l) {
            case LEX_LINE:
                bnf(); break;
            case LEX_LBR:
                bnf(); lcheck(LEX_RBR); gl(); break;
            case LEX_LRT:
                bnf(); lcheck(LEX_RRT); gl(); break;
            case LEX_LFG:
                bnf(); lcheck(LEX_RFG); gl(); break;
        }
    }
}

void FileTPL::guard()
{
    term();
    if (m_l == LEX_TERM)
    {

        if (m_repr == "or") {
            gl();
            if (m_l == LEX_TERM && m_repr == "not")
                ;
            else
                ;
            gl();
        } else if (m_repr == "and") {
            gl();
            if (m_l == LEX_TERM && m_repr == "not")
                ;
            else
                ;
            gl();
        }
        guard();
    }
}

void FileTPL::term()
{

}
*/
