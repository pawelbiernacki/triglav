#include "triglav.h"
using namespace triglav;


void input_variable::report(std::ostream & s) const
{
    report_with_internal(*this, s, "input variable");
}
