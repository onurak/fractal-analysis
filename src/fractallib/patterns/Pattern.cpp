/** This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fractal Library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Pattern.h"
#include "Context.h"
#include "../trees/Tree.h"
#include "../compilers/AbstractCompiler.h"
#include "../TimeSeries.h"
#include <cmath>

using namespace FL;
using namespace FL::Patterns;
using namespace FL::Exceptions;

namespace Internal
{
    using namespace FL::Compilers;

    class Compiler : public AbstractCompiler
    {
    public:
        static const int LEX_RAW = 1;
    public:
        Compiler(Description &d, Guard &g) : d(d), g(g) {}
    protected:
        virtual void S()
        {
            EParsing e;

            e = d.compile(*m_input);
            if (e.id() != FL::E_OK) throw e;
            e = g.compile(*m_input);
            if (e.id() != FL::E_OK) throw e;
        }
    protected:
        Description &d;
        Guard &g;
    };

} // namespace Internal

Pattern::Pattern(const PatternConstructor *pc, bool autoDeleteConstructor)
{
    assert(pc != NULL);
    m_description = pc->createDescription();
    m_guard = pc->createGuard(*m_description);
    if (autoDeleteConstructor)
        delete pc;
}

Pattern::Pattern(const std::string &name, const PatternConstructor *pc, bool autoDeleteConstructor)
{
    assert(pc != NULL);
    setName(name);
    m_description = pc->createDescription();
    m_guard = pc->createGuard(*m_description);
    if (autoDeleteConstructor)
        delete pc;
}

Pattern::Pattern(const PatternConstructor &pc)
{
    m_description = pc.createDescription();
    m_guard = pc.createGuard(*m_description);
}

Pattern::Pattern(const std::string &name, const PatternConstructor &pc)
{
    setName(name);
    m_description = pc.createDescription();
    m_guard = pc.createGuard(*m_description);
}

Pattern::Pattern(const Pattern &p)
    : m_guard(p.m_guard), m_description(p.m_description)
{

}

int Pattern::maxSequenceLength() const
{
    CISet &set = m_description->sequences();

    if (set.size() == 0)
        return 0;

    int result = (int)set.front().size();

    CISet::const_iterator seq;
    forall(seq, set)
        if (result < (int)seq->size())
            result = (int)seq->size();
    return result;
}

const std::string& Pattern::name() const
{
    return m_description->name();
}

void Pattern::setName(const std::string& name)
{
    m_description->setName(name);
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

EParsing Pattern::compile(const std::string &s)
{
    Internal::Compiler compiler(*m_description, *m_guard);
    if (!compiler.compile(s, cfPartialInput))
        return compiler.lastError();
    else
        return EParsing(E_OK);
}

EParsing Pattern::compile(Compilers::Input *input)
{
    Internal::Compiler compiler(*m_description, *m_guard);
    if (!compiler.compile(input, cfPartialInput))
        return compiler.lastError();
    else
        return EParsing(E_OK);
}

CheckResult Pattern::check(Context &c, CheckInfo &info, CheckOptions check)
{
    // Fill partial info on candidateNode
    if (!(check & coContinueChecking))
    {
        Trees::Node *candidate = c.candidateNode();
        candidate->setId(m_description->id());
        candidate->setBegin(-1);
        candidate->setEnd(-1);
    }

    // Check description
    if ( !(check & coNoDescriptionCheck) )
    {
        if (!m_description->check(c, info, check))
            return crInvalidDescription;
    }
    else if (check & coContinueChecking)
    {
        info.applicableSequences.clear();
        if (c.candidateNode()->origSequence())
        {
            CISequenceInfo tempInfo;
            tempInfo.sequence = c.candidateNode()->origSequence();
            info.applicableSequences.push_back(tempInfo);
        }
    }

    // For each applicable sequence check guard
    std::vector<CISequenceInfo>::iterator si = info.applicableSequences.begin();

    while ( si != info.applicableSequences.end() )
    {
        // Check guard in the just parsed sequence
        if (check & coContinueChecking)
        {
            std::list<Trees::Node*> newRoots;
            newRoots.assign(c.candidateNode()->children().begin(),
                            c.candidateNode()->children().end());
            Trees::Layer followNodes =
                    c.outputTree().getFollowingRoots(c.candidateNode());
            if (followNodes.size() > 0)
                newRoots.insert(newRoots.end(), followNodes.begin(), followNodes.end());
            c.setCustomRoots(newRoots);
        }
        c.buildLastParsed(*si->sequence);

        if (!m_guard->check(c, info))
            si = info.applicableSequences.erase(si);
        else
            ++si;
    }
    if (info.applicableSequences.size() == 0)
        return crInvalidGuard;

    return crOK;
}

bool Pattern::calcGuardLimits(
    FL::Trees::Node *node,
    FL::Trees::Tree &tree,
    FL::TimeSeries &ts,
    FL::ForecastItem &forecast)
{
    if (node == NULL ||
        node->status() != nsPossible ||
        node->origSequence() == NULL ||
        node->children().size() == node->origSequence()->size() ||
        node->origPattern() != this)
    {
        return false;
    }

    if (node->status() != nsPossible)
        return false;

    Context context;
    context.setParseTree(&tree);
    context.setOutputTree(&tree);
    context.setCandidateNode(node);
    context.setTimeSeries(&ts);
    context.setCustomRoots(node->children());
    context.buildLastParsed(*node->origSequence());

    CheckInfo info;
    CISequenceInfo si;
    si.sequence = node->origSequence();
    info.applicableSequences.push_back(si);

    double initialValue = ts.values(-1);

    forecast.minValue = 0;
    forecast.maxValue = 0;
    forecast.minDuration = 0;
    forecast.maxDuration = 0;

    double tsStep = fabs(ts.values(-1) - ts.values(-2)) / 10;
    int maxK = 20;

    // Try move up
    int k;
    for (k = 0; k < maxK; ++k)
    {
        bool isOk = m_guard->check(context, info);

        if (isOk)
        {
            ts.values(-1) += tsStep;
        }
        else
        {
            k--;
            break;
        }
    }
    ts.values(-1) = initialValue;
    forecast.maxValue = k * tsStep;

    // Try move down
    for (k = 0; k < maxK; ++k)
    {
        bool isOk = m_guard->check(context, info);

        if (isOk)
        {
            ts.values(-1) -= tsStep;
        }
        else
        {
            k--;
            break;
        }
    }

    ts.values(-1) = initialValue;
    forecast.minValue = -k * tsStep;


    if (forecast.maxDuration == 0)
        forecast.maxDuration = 1;
    return true;
}



int PatternsSet::maxSequenceLength() const
{
    if (size() == 0)
        return 0;
    int result = front()->maxSequenceLength();

    ConstIterator pattern;
    forall(pattern, *this)
    {
        if ( (*pattern)->maxSequenceLength() > result )
            result = (*pattern)->maxSequenceLength();
    }

    return result;
}
