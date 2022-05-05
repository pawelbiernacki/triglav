#include "triglav.h"
#include <algorithm>
#include <sstream>
using namespace triglav;

void generic_name_list::report(std::ostream & s) const
{
    iterate(*this, [&s](auto i) {
        i->report(s);
    });
}

bool generic_name_list::get_contains_expandable_items() const
{
    bool result=false;
    iterate(*this, [&result](auto i) {
        if (i->get_is_expandable()) result=true;
    });
    return result;
}

void generic_name_list::update_iterator(my_iterator & target)
{
    iterate(*this, [&target](auto i) {
        target.update(*i);
    });
}


std::string generic_name_list::get_actual_name(my_iterator & source)
{
    std::stringstream s;
    bool ok=true;

    for (auto i(list_of_name_items.begin()); i!=list_of_name_items.end(); i++)
    {
        if ((*i)->get_is_expandable())
        {
            if (source.get_current_name((*i)->get_placeholder_name()) == (*i)->get_placeholder_name())
            {
                ok=false;
                break;
            }
            s << source.get_current_name((*i)->get_placeholder_name());
        }
        else
        {
            (*i)->report(s);
        }
    }

    if (ok)
    {
        std::string actual_variable_name = s.str();
        return actual_variable_name;
    }
    return input::unknown_marker;
}
