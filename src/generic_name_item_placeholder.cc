#include "triglav.h"
using namespace triglav;

void generic_name_item_placeholder::report(std::ostream & s) const
{
    s << "(" << placeholder_name << ":";
    my_type_expression->report(s);
    s << ")";
}
