#include "triglav.h"
using namespace triglav;

void set_of_possible_values::report(std::ostream & s) const
{
    bool first = true;
    s << "{";
    for (auto i: *this)
    {
        if (!first) s << ",";
        s << i;
        first = false;
    }
    s << "}";
}
