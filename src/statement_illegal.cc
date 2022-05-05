#include "triglav.h"
using namespace triglav;

void statement_illegal::execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    //std::cout << "executing illegal\n";
    o->set_illegal();
}

void statement_illegal::report(std::ostream & s) const
{
    s << "illegal;";
}
