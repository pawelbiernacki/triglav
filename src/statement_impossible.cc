#include "triglav.h"
using namespace triglav;

void statement_impossible::execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::cout << "executing impossible\n";
}

void statement_impossible::report(std::ostream & s) const
{
    s << "impossible;";
}
