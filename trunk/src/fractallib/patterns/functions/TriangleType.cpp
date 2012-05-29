#include "TriangleType.h"
#include <cmath>

using namespace FL::Patterns::Functions;

TriangleType::TriangleType()
{
    m_name = "TriangleType";
}

int sgn(double d)
{
    return d < 0 ? -1 : (d > 0 ? 1 : 0);
}

const GVariant& TriangleType::operator()(Patterns::Context& context, FunctionArgs& args)
{
    context.unused();
    if (args.size() != 8)
        throw FL::Exceptions::EArguments(m_name, 8, args.size());

    /* Find line params (k,b) from first two nodes: y(x) = k*x + b.
       Solve system for (k,5b):
          { y1 = k*x1 + b,
          { y2 = k*x2 + b
       Solution:
            k = (y1 - y2) / (x1 - x2)
            b = y2 - k*x2

       Find (k1, b1) and (k2, b2) for upper and lower line.
     */
    double x1 = double(*args[0]);
    double y1 = double(*args[1]);
    double x2 = double(*args[2]);
    double y2 = double(*args[3]);
    double x3 = double(*args[4]);
    double y3 = double(*args[5]);
    double x4 = double(*args[6]);
    double y4 = double(*args[7]);

    // Find intersection point
    double v = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
    if (fabs(v - 0.0) < 0.01)
        return m_result = 0;

    double xi = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4)) / v;
    //double yi = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4)) / v;

    if (xi > x1 || xi > x2 || xi > x3 || xi > x4)
        return m_result = 1;

    return m_result = 2;


//    double k1 = (y1 - y2) / (x1 - x2);
//    double b1 = y2 - k1*x2;

//    double k2 = (y4 - y3) / (x4 - x3);
//    double b2 = y4 - k2*x4;

//    // Ensure (k1, b1) is for upper line and
//    // (k2, b2) is for lower line.
//    if (b1 < b2)
//    {
//        std::swap(b1, b2);
//        std::swap(k1, k2);
//    }

//    // Determinte type of triangle
//    if (k1 == k2)
//        return m_result = 0;

//    if (k1 < 0.0 && k2 > 0.0)
//        return m_result = 1;

//    return m_result = 2;
}

