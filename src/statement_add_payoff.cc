#include "triglav.h"
using namespace triglav;

void statement_add_payoff::execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    j.add_payoff(p);
}

void statement_add_payoff::report(std::ostream & s) const
{
    s << "add_payoff(" << std::showpoint << p << ");";
}
