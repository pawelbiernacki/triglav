#include "triglav.h"
using namespace triglav;

void statement_assert_is_true::execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    //std::cout << "executing assert is true\n";
    bool failed;
    my_expression->assert_is_true(a, b, b2, source, j, j2, failed);
    if (failed && o!=nullptr) o->set_illegal();
}

void statement_assert_is_true::report(std::ostream & s) const
{
    s << "assert_is_true(";
    my_expression->report(s);
    s << ");";
}
