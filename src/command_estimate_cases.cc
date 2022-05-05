#include "triglav.h"
using namespace triglav;

void command_estimate_cases::execute(agent & a) const
{
    a.estimate_cases();
}
