#include "triglav.h"
using namespace triglav;

void variable_instance::internal_report(std::ostream & s) const
{
    s << "{";
    iterate_with_first(*this, s, [&s](auto i) {
        s << i->get_name();
    });
    s << "}";

    if (has_usual_values && my_variable)
    {
        bool first = true;
        s << " usually {";
        for (auto i(my_variable->get_list_of_usual_values().begin()); i!=my_variable->get_list_of_usual_values().end(); i++)
        {
            if (!first) s << ",";
            s << *i;
            first = false;
        }
        s << "}";
    }

    s << ";";
}
