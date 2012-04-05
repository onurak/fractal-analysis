#include "Pattern.h"
#include "Context.h"
#include "../Trees/Tree.h"
#include "standard/DescriptionCompilerEbnf.h"
#include "../compilers/AbstractCompiler.h"

using namespace FL::Patterns;
using namespace FL::Exceptions;

std::map< int, std::vector<Pattern*> > PatternsRegistry::m_registry;


namespace Internal
{
    using namespace FL::Compilers;

    class Compiler : public AbstractCompiler
    {
    public:
        static const int LEX_RAW = 1;
    public:
        Compiler(Description &d, Guard &g) : d(d), g(g)
        {
        }

        virtual void vgl()
        {
            m_l = LEX_RAW;
        }
    protected:
        virtual void S()
        {
            EParsing e;

            Standard::DescriptionCompilerEbnf descriptionCompiler;

            e = descriptionCompiler.compile(*m_input, &d);
            if (e.id() != FL::E_OK)
                throw e;

            e = g.compile(*m_input);
            if (e.id() != FL::E_OK)
                throw e;
        }
    protected:
        Description &d;
        Guard &g;
    };

} // namespace Internal

Pattern::Pattern(const PatternConstructor &pc)
{
    m_description = pc.createDescription();
    m_guard = pc.createGuard(*m_description);
}

Pattern::Pattern(const Pattern &p)
    : m_guard(p.m_guard), m_description(p.m_description)
{

}

Pattern::~Pattern()
{
    PatternsRegistry::unregisterPattern(id(), this);
}

const std::string& Pattern::name() const
{
    return m_description->name();
}

void Pattern::setName(const std::string& name)
{
    PatternsRegistry::unregisterPattern(id(), this);
    m_description->setName(name);
    PatternsRegistry::registerPattern(id(), this);
}

int Pattern::id() const
{
    return m_description->id();
}

Guard* Pattern::guard() const
{
    return m_guard;
}

Description* Pattern::description() const
{
    return m_description;
}

int Pattern::maxLength() const
{
    size_t result = 0;
    CISet::const_iterator i;
    forall(i, m_description->sequences())
    {
        if (i->size() > result)
            result = i->size();
    }

    return int(result);
}

EParsing Pattern::compile(const std::string &s)
{
    PatternsRegistry::unregisterPattern(id(), this);

    Internal::Compiler compiler(*m_description, *m_guard);
    if (!compiler.compile(s, cfPartialInput))
        return compiler.lastError();
    else
    {
        PatternsRegistry::registerPattern(id(), this);
        return EParsing(E_OK);
    }
}

EParsing Pattern::compile(Compilers::Input *input)
{
    PatternsRegistry::unregisterPattern(id(), this);

    Internal::Compiler compiler(*m_description, *m_guard);
    if (!compiler.compile(input, cfPartialInput))
        return compiler.lastError();
    else
    {
        PatternsRegistry::registerPattern(id(), this);
        return EParsing(E_OK);
    }
}

//CheckResult Pattern::check(Context &c, CheckInfo &info)
//{
//    // Prepare context
//    //c.clearLastParsed();

//    // Fill partial info on candidateNode
//    Trees::Node *candidate = c.candidateNode();
//    candidate->setId(m_description->id());
//    candidate->setBegin(-1);
//    candidate->setEnd(-1);

//    // Check description
//    if (!m_description->check(c, info))
//        return crInvalidDescription;

//    // For each applicable sequence check guard
//    std::vector<CISequence*>::iterator sequence =
//            info.applicableSequences.begin();
//    for (; sequence != info.applicableSequences.end(); )
//    {
//        c.buildLastParsed(**sequence);
//        // Check guard
//        if (!m_guard->check(c, info))
//        {
//            sequence = info.applicableSequences.erase(sequence);
//        }
//        else
//            ++sequence;
//    }
//    if (info.applicableSequences.size() == 0)
//        return crInvalidGuard;

//    return crOK;
//}


//! Check for left recursion in patterns set
bool PatternsSet::haveLeftRecursion() const
{
    ConstIterator itPattern;
    forall(itPattern, *this)
    {
        Pattern *p = *itPattern;
        CISet::const_iterator itSeq;
        forall(itSeq, p->description()->sequences())
        {
            if ((*itSeq)[0].id == p->id())
                return true;
        }
    }

    return false;
}

int PatternsSet::maxLength() const
{
    int result = 0;

    ConstIterator itPattern;
    forall(itPattern, *this)
    {
        int pLength = (*itPattern)->maxLength();
        if (pLength > result)
            result = pLength;
    }

    return result;
}


