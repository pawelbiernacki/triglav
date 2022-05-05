#include "triglav.h"
using namespace triglav;

void type::report(std::ostream & s) const
{
    s << "type " << name << "={";
    iterate_with_first(*this, s, [&s](auto i) {
        i->report(s);
    });
    s << "};";
}
