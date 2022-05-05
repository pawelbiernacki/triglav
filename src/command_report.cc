#include "triglav.h"
using namespace triglav;


void command_report::execute(agent & a) const
{
    report(a, std::cout);
}
