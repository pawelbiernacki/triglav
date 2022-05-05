#include "triglav.h"
using namespace triglav;


bool logical_expression_and::get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o)
{
    bool l = left->get_can_be_evaluated(a, b, source, j, j2, o);
    bool r = right->get_can_be_evaluated(a, b, source, j, j2, o);

    //std::cout << "logical_expression_and::get_can_be_evaluated " << l << "&&" << r << "\n";

    return l && r;
}

