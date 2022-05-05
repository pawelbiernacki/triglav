#include "triglav.h"
using namespace triglav;

void command_loop::execute(agent & a) const
{
    a.loop(depth);
}
