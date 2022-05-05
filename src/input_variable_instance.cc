#include "triglav.h"
using namespace triglav;

void input_variable_instance::report(std::ostream & s) const
{
    report_with_internal(*this, s, "input variable instance");
}
