#include "triglav.h"
#include <sstream>
#include <algorithm>
using namespace triglav;

void expression_value::report(std::ostream & s) const
{
    my_value_name->report(s);
}

void expression_value::update_iterator(my_iterator & target)
{
    my_value_name->update_iterator(target);
}

void expression_value::set_possible_values(set_of_possible_values & s, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2)
{
    // empty
}

void expression_value::update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_value_name = my_value_name->get_actual_name(source);
    if (!target.get_contains(actual_value_name))
    {
        target.push_back(actual_value_name);
    }
}

bool expression_value::get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const
{
    return false;
}

bool expression_value::get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_value_name = my_value_name->get_actual_name(source);

    //std::cout << "identified value " << actual_value_name << "\n";
    auto x = a.get_map_name_to_value_instance().find(actual_value_name);

    //auto x = std::find_if(a.get_list_of_value_instances().begin(), a.get_list_of_value_instances().end(), [&actual_value_name](auto & i){ return i->get_name() == actual_value_name; });

    return x != a.get_map_name_to_value_instance().end();
}


std::string expression_value::get_evaluate_given_assumption(agent & a, my_iterator & source)
{
    std::string actual_value_name = my_value_name->get_actual_name(source);

    //std::cout << "identified value " << actual_value_name << "\n";
    auto x = a.get_map_name_to_value_instance().find(actual_value_name);

    //auto x = std::find_if(a.get_list_of_value_instances().begin(), a.get_list_of_value_instances().end(), [&actual_value_name](auto & i){ return i->get_name() == actual_value_name; });
    return (*x).second->get_name();
}

bool expression_value::get_can_be_evaluated_given_assumption(agent & a, my_iterator & source)
{
    std::string actual_value_name = my_value_name->get_actual_name(source);

    //std::cout << "identified value " << actual_value_name << "\n";
    auto x = a.get_map_name_to_value_instance().find(actual_value_name);

    //auto x = std::find_if(a.get_list_of_value_instances().begin(), a.get_list_of_value_instances().end(), [&actual_value_name](auto & i){ return i->get_name() == actual_value_name; });

    return x != a.get_map_name_to_value_instance().end();
}


void expression_value::explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    report(std::cout);
    std::cout << "=";


    std::string actual_value_name = my_value_name->get_actual_name(source);

    //std::cout << "identified value " << actual_value_name << "\n";
    auto x = a.get_map_name_to_value_instance().find(actual_value_name);

    //auto x = std::find_if(a.get_list_of_value_instances().begin(), a.get_list_of_value_instances().end(), [&actual_value_name](auto & i){ return i->get_name() == actual_value_name; });

    std::cout << "=" << (*x).second->get_name() << "\n";
}

std::string expression_value::get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_value_name = my_value_name->get_actual_name(source);

    //std::cout << "identified value " << actual_value_name << "\n";
    auto x = a.get_map_name_to_value_instance().find(actual_value_name);

    //auto x = std::find_if(a.get_list_of_value_instances().begin(), a.get_list_of_value_instances().end(), [&actual_value_name](auto & i){ return i->get_name() == actual_value_name; });

    return (*x).second->get_name();
}

void expression_value::assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v)
{
}

void expression_value::assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v)
{
}

