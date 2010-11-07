#include "Guards.h"

using namespace FL;
using namespace FL::Terms;

bool Guard::Condition::check(ParseContext &context, GCollection<SequenceElement*> &subseq)
{
    double vLeft = calcFunc(context, left, subseq),   vRight;

    if (right.type() == G_VAR_PVOID) {
        Terms::TermCall *call;
        if (!right.asClass(&call))
            return false;
        vRight = calcFunc(context, *call, subseq);
    } else
        vRight = (double)right;

    switch (op) {
        case Less:            return vLeft <  vRight;
        case Greater:         return vLeft >  vRight;
        case Equals:          return vLeft == vRight;
        case NotEquals:       return vLeft != vRight;
        case LessOrEquals:    return vLeft <= vRight;
        case GreaterOrEquals: return vLeft >= vRight;
        default: return false;
    }
}

double Guard::Condition::calcFunc(ParseContext &context, Terms::TermCall &func,
                                  GCollection<SequenceElement*> &subseq)
{
    // ищем элемент, к которому нужно применить условие
    for (int i = 0; i < subseq.count(); i++)
        if (func.canApplyTo(*(subseq[i])))
            return func(context, context.prevPos + i);
    return 0;
}

bool Guard::check(ParseContext &context)
{
    // пробуем выделить подпоследовательность, соответствующую шаблону
    GCollection<SequenceElement*> subseq;
    if (!context.psPrevious->getSubsequence(context.prevPos, m_template->elements().count(), subseq))
        return false;    
    for (unsigned int i = 0; i < m_conditions.size(); i++)
        if (!m_conditions[i].check(context, subseq))
            return false;
    return true;
}
