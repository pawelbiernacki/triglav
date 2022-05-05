#include "triglav.h"
using namespace triglav;


void command_generate_cases::execute(agent & a) const
{
    a.generate_cases(max_amount_of_unusual_cases, amount_of_output_files);
}
