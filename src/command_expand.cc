#include "triglav.h"
using namespace triglav;


void command_expand::execute(agent & a) const
{
    a.expand(depth);
    a.set_depth(depth);
}
