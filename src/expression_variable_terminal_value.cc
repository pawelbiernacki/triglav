#include "triglav.h"
#include <sstream>
#include <algorithm>
using namespace triglav;


void expression_variable_terminal_value::report(std::ostream & s) const
{
    s << "terminal value ";
    my_variable_name->report(s);
}

void expression_variable_terminal_value::update_iterator(my_iterator & target)
{
    my_variable_name->update_iterator(target);
}

void expression_variable_terminal_value::set_possible_values(set_of_possible_values & st, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);
    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);
    //auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i){ return i->get_name() == actual_variable_name; });

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
        }
        break;

        case variable_mode::INPUT:
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }
}


void expression_variable_terminal_value::update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);
    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);
    //auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i){ return i->get_name() == actual_variable_name; });

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
            b2->update_set_of_possible_values(target, *(*x).second);
        }
        break;

        case variable_mode::INPUT:
        {
            if (j2 != nullptr)
            {
                std::string r = j2->get_variable_instance_value((*x).second->get_name());
                if (!target.get_contains(r))
                {
                    target.push_back(r);
                }
            }
            else
            {
                //std::cout << actual_variable_name << " is an input variable, but no terminal input has been provided\n";
            }
        }
        break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }
}


bool expression_variable_terminal_value::get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);
    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);
    //auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i){ return i->get_name() == actual_variable_name; });

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
        }
        return false;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable " << (*x).second->get_name() << "\n";
            return true;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            return false;
        }
    }
    return false;
}

void expression_variable_terminal_value::explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);
    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);


    std::cout << actual_variable_name << " ";

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
            std::cout << "is terminal hidden variable\n";
        }
        break;

        case variable_mode::INPUT:
            if (j2 != nullptr)
                std::cout << "=" << j2->get_variable_instance_value((*x).second->get_name()) << "\n";
            else
                std::cout << "no input provided\n";
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            std::cout << "is terminal output variable\n";
            break;
        }
    }
    else
    {
        std::cout << "is unknown\n";
    }
}

std::string expression_variable_terminal_value::get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);
    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);
    //auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i){ return i->get_name() == actual_variable_name; });

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
        }
        break;

        case variable_mode::INPUT:
            if (j2 != nullptr)
                return j2->get_variable_instance_value((*x).second->get_name());
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }
    return input::unknown_marker;
}


void expression_variable_terminal_value::assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);

    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);
    //auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i){ return i->get_name() == actual_variable_name; });

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";

            if (b2)
            {
                b2->this_state_is_impossible(*(*x).second, v);
            }
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable, set it to " << v << "\n";
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }
}


void expression_variable_terminal_value::assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v)
{
    //std::cout << "expression_variable_terminal_value::assert_is_equal\n";

    std::string actual_variable_name = my_variable_name->get_actual_name(source);

    auto x = a.get_map_name_to_variable_instance().find(actual_variable_name);
    //auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i){ return i->get_name() == actual_variable_name; });

    if (x != a.get_map_name_to_variable_instance().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x).second->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";

            if (b2)
            {
                b2->all_but_one_state_is_impossible(*(*x).second, v);
            }
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable, set it to " << v << "\n";

            if (j2)
            {
                j2->all_but_one_state_is_impossible(*(*x).second, v);
            }
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }
}


bool expression_variable_terminal_value::get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const
{
    return false;
}
