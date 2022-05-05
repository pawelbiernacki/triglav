#include "triglav.h"
using namespace triglav;

void statement_there_is_a_chance::execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    if (j2 != nullptr)
    {
        //std::cout << "statement_there_is_a_chance::execute\n";
        j2->there_is_a_chance(probability);
    }
    else
    {
        //std::cout << "j2 is nullptr\n";
    }
}

void statement_there_is_a_chance::report(std::ostream & s) const
{
    s << "there_is_a_chance(" << std::showpoint << probability << ");";
}
