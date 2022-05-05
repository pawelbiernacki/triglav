#include "triglav.h"
using namespace triglav;

void variable::internal_report(std::ostream & s) const
{

    my_name.report(s);
    s << ":";
    my_type->report(s);
    if (has_usual_values)
    {
        s << " usually {";
        bool first = true;
        for (auto i(list_of_usual_values.begin()); i!=list_of_usual_values.end(); i++)
        {
            if (!first) s << ",";
            s << *i;
            first = false;
        }
        s << "}";
    }
}
