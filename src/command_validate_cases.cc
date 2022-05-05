#include "triglav.h"
using namespace triglav;


void command_validate_cases::execute(agent & a) const
{
    a.validate_cases();
}
