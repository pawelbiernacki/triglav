#include "triglav.h"
#include <algorithm>
using namespace triglav;

void type_instance::report(std::ostream & s) const
{
    bool first = true;
    s << "type instance " << name << ":{";
    for (auto i(list_of_value_instances.begin()); i!=list_of_value_instances.end(); i++)
    {
        if (!first) s << ",";
        s << (*i)->get_name();
        first = false;
    }
    s << "};";
}


void type_instance::add(std::shared_ptr<value_instance> && i)
{
    auto x = std::find_if(list_of_value_instances.begin(), list_of_value_instances.end(), [&i](auto y) {
        return y->get_name()==i->get_name();
    });

    if (x == list_of_value_instances.end())
    {
        list_of_value_instances.push_back(std::move(i));
    }
}
