#include "triglav.h"
#include <algorithm>
using namespace triglav;


void my_iterator::update(generic_name_item & source)
{
    if (source.get_is_expandable())
    {
        const std::string & placeholder_name = source.get_placeholder_name();
        auto& te=source.get_type_expression();

        if (map_placeholders_to_values.find(placeholder_name) == map_placeholders_to_values.end())
        {
            amount_of_placeholders++;

            //std::cout << "updated amount_of_placeholders : " << amount_of_placeholders << "\n";

            auto [it, success] = map_placeholders_to_list_of_possible_values.insert(std::pair(placeholder_name, std::list<std::string>()));
            if (!success)
            {
                throw std::runtime_error("failed to insert a placeholder");
            }

            auto x = std::find_if(my_agent.get_list_of_type_instances().begin(), my_agent.get_list_of_type_instances().end(),
            [&te](auto & ti) {
                return ti->get_name() == te->get_type_name();
            });

            if (x != my_agent.get_list_of_type_instances().end())
            {
                bool found = false;
                for (auto j((*x)->get_list_of_value_instances().begin()); j!=(*x)->get_list_of_value_instances().end(); j++)
                {
                    found = true;
                    map_placeholders_to_list_of_possible_values.at(placeholder_name).push_back((*j)->get_name());
                }

                if (found)
                {
                    auto [it, success] = map_placeholders_to_values.insert(std::pair(placeholder_name, map_placeholders_to_list_of_possible_values.at(placeholder_name).begin()));

                    if (!success)
                    {
                        throw std::runtime_error("failed to insert a list of possible values");
                    }
                }
            }
        }
    }
}

std::string my_iterator::get_current_name(std::string placeholder)
{
    if (map_placeholders_to_values.find(placeholder) != map_placeholders_to_values.end())
    {
        return *map_placeholders_to_values.at(placeholder);
    }
    return placeholder;
}

void my_iterator::report(std::ostream & s) const
{
    bool first = true;
    s << "agent's iterator: [" << amount_of_placeholders << "] ";

    for (auto i(map_placeholders_to_values.begin()); i!=map_placeholders_to_values.end(); i++)
    {
        if (!first) s << ",";

        if ((*i).second != map_placeholders_to_list_of_possible_values.at((*i).first).end())
        {
            s << "map[" << (*i).first << "]=" << *(*i).second;
        }
        else
        {
            s << "map[" << (*i).first << "]=" << "<END>";
        }

        first = false;
        /*
        for (auto j(map_placeholders_to_list_of_possible_values.at((*i).first).begin()); j!=map_placeholders_to_list_of_possible_values.at((*i).first).end(); j++)
        {
            s << (*j) << " ";
        }
        */
    }
    s << "\n";
}

bool my_iterator::get_is_valid() const
{
    if (amount_of_placeholders == 0)
    {
        return true;
    }

    for (auto i(map_placeholders_to_values.begin()); i!=map_placeholders_to_values.end(); i++)
    {
        if ((*i).second == map_placeholders_to_list_of_possible_values.at((*i).first).end())
        {
            return false;
        }
    }
    return true;
}


bool my_iterator::get_finished() const
{
    if (amount_of_placeholders == 0)
    {
        return processed;
    }


    for (auto i(map_placeholders_to_values.begin()); i!=map_placeholders_to_values.end(); i++)
    {
        if ((*i).second != map_placeholders_to_list_of_possible_values.at((*i).first).end())
        {
            return false;
        }
    }

    return true;
}

my_iterator& my_iterator::operator++()
{
    if (amount_of_placeholders == 0)
    {
        processed = true;
    }
    else
    {
        for (auto i(map_placeholders_to_values.begin()); i!=map_placeholders_to_values.end(); i++)
        {
            if ((*i).second != map_placeholders_to_list_of_possible_values.at((*i).first).end())
            {
                (*i).second++;
                return *this;
            }
            else
            {
                if (!get_finished())
                {
                    (*i).second = map_placeholders_to_list_of_possible_values.at((*i).first).begin();
                    continue;
                }
            }
        }
    }
    return *this;
}

