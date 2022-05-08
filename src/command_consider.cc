#include "triglav.h"
using namespace triglav;

void command_consider::execute(agent & a) const
{
    a.add_consider_variable_instance(variable_name, my_rank);
}
