#include "triglav.h"
using namespace triglav;

void output_variable_instance::report(std::ostream & s) const
{
    report_with_internal(*this, s, "output variable instance");
}
