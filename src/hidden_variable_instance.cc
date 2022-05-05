#include "triglav.h"
using namespace triglav;

void hidden_variable_instance::report(std::ostream & s) const
{
    report_with_internal(*this, s, "hidden variable instance");
}
