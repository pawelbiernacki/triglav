#include "triglav.h"
using namespace triglav;

void command_precalculate::execute(agent & a) const
{
    a.precalculate();    
}
