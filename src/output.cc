#include "triglav.h"
#include <algorithm>
using namespace triglav;

output::output(agent & a): my_agent{a}, illegal{false}
{
    for (auto i(my_agent.get_list_of_variable_instances().begin()); i!= my_agent.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::OUTPUT)
        {
            vector_of_output_items.push_back(std::make_shared<output_item>(**i, (*i)->get_vector_of_value_instances().begin()));
        }
    }
}

output& output::operator=(output & source)
{
    vector_of_output_items.clear();
    illegal = source.illegal;

    for (auto i(source.vector_of_output_items.begin()); i!=source.vector_of_output_items.end(); i++)
    {
        vector_of_output_items.push_back(std::make_shared<output_item>((*i)->get_variable_instance(),(*i)->get_value_instance_iterator()));
    }
    return *this;
}


bool output::get_is_valid() const
{
    for (auto i(vector_of_output_items.begin()); i!=vector_of_output_items.end(); i++)
    {
        if ((*i)->get_value_instance_iterator() == (*i)->get_variable_instance().get_vector_of_value_instances().end())
        {
            return false;
        }
    }
    return true;
}

bool output::get_all_actions_have_been_processed() const
{
    for (auto i(vector_of_output_items.begin()); i!=vector_of_output_items.end(); i++)
    {
        if ((*i)->get_value_instance_iterator() != (*i)->get_variable_instance().get_vector_of_value_instances().end())
        {
            return false;
        }
    }
    return true;
}

output& output::operator++()
{
    illegal = false;

    for (auto i(vector_of_output_items.begin()); i!=vector_of_output_items.end(); i++)
    {
        if ((*i)->get_value_instance_iterator() != (*i)->get_variable_instance().get_vector_of_value_instances().end())
        {
            (*i)->get_value_instance_iterator()++;
            return *this;
        }
    }

    return *this;
}

void output::report(std::ostream & s) const
{
    bool first = true;
    for (auto i(vector_of_output_items.begin()); i!=vector_of_output_items.end(); i++)
    {
        if (!first) s << ",";
        (*i)->report(s);
        first = false;
    }
    s << "\n";
}

std::string output::get_variable_instance_value(const std::string i) const
{
    auto x = std::find_if(vector_of_output_items.begin(), vector_of_output_items.end(), [&i](auto y) {
        return y->get_variable_instance().get_name()==i;
    });

    if (x != vector_of_output_items.end())
    {
        return (*x)->get_value_instance().get_name();
    }

    return input::unknown_marker;
}


output::output_item::output_item(variable_instance & vi1, std::vector<std::shared_ptr<value_instance>>::iterator vi2):
    my_variable_instance{vi1}, my_value_instance_iterator{vi2}
{
}


void output::output_item::report(std::ostream & s) const
{
    s << my_variable_instance.get_name() << "==";
    if (my_value_instance_iterator != my_variable_instance.get_vector_of_value_instances().end())
    {
        s << (*my_value_instance_iterator)->get_name();
        return;
    }
    s << "<end>";
}
