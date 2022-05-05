#include "triglav.h"
using namespace triglav;


void command_generate_cases::execute(agent & a) const
{
    a.generate_cases();
}
