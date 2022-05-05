#include "triglav.h"
using namespace triglav;

void statement_assert_is_false::execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    bool failed;
    my_expression->assert_is_false(a, b, b2, source, j, j2, failed);
    if (failed) o->set_illegal();
}

void statement_assert_is_false::report(std::ostream & s) const
{
    s << "assert_is_false(";
    my_expression->report(s);
    s << ");";
}
