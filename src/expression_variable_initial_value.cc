#include "triglav.h"
#include <sstream>
#include <algorithm>
using namespace triglav;


void expression_variable_initial_value::report(std::ostream & s) const
{
    s << "initial value ";
    my_variable_name->report(s);
}

void expression_variable_initial_value::update_iterator(my_iterator & target)
{
    my_variable_name->update_iterator(target);
}

void expression_variable_initial_value::set_possible_values(set_of_possible_values & st, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2)
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
            b.set_possible_values(st, *(*x).second);
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable\n";
        {
        }
        break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable\n";
            //std::cout << "RETURN " << (o!=nullptr?"true": "false") << "\n";
            break;
        }
    }
}


void expression_variable_initial_value::update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
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
            b.update_set_of_possible_values(target, *(*x).second);
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable\n";
        {
            std::string v = j.get_variable_instance_value((*x).second->get_name());
            if (v!=input::unknown_marker && !target.get_contains(v))
            {
                target.push_back(v);
            }
        }
        break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable\n";
            //std::cout << "RETURN " << (o!=nullptr?"true": "false") << "\n";
            if (o != nullptr)
            {
                auto r = o->get_variable_instance_value((*x).second->get_name());
                if (!target.get_contains(r))
                {
                    target.push_back(r);
                }
            }
            break;
        }
    }
}

bool expression_variable_initial_value::get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const
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

            //std::cout << "it is a hidden variable\n";
            return false;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable\n";
            return false;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable\n";
            return true;
        }
    }
    return false;
}


bool expression_variable_initial_value::get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o)
{
    // either an input variable or a known hidden variable (according to the current belief)
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
            return b.get_is_known(*(*x).second);
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable\n";
            return j.get_variable_instance_value((*x).second->get_name())!=input::unknown_marker;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable\n";
            //std::cout << "RETURN " << (o!=nullptr?"true": "false") << "\n";
            return o != nullptr;
        }
    }

    return false;
}


void expression_variable_initial_value::explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{

    std::string actual_variable_name = my_variable_name->get_actual_name(source);

    auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto i) {
        return i->get_name() == actual_variable_name;
    });

    std::cout << actual_variable_name;


    if (x != a.get_list_of_variable_instances().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x)->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "its value equals " << b.get_value(**x) << "\n";

            std::cout << "=" << b.get_value(**x) << "\n";
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable\n";
            std::cout << "=" << j.get_variable_instance_value((*x)->get_name()) << "\n";
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable\n";
            if (o)
            {
                auto r = o->get_variable_instance_value((*x)->get_name());

                //std::cout << "output variable value " << r << "\n";
                std::cout << "=" << r << "\n";
            }
            else
            {
                std::cout << "=" << input::unknown_marker << "\n";
            }
            break;
        }
    }
    else
    {
        std::cout << "not found\n";
    }
}


std::string expression_variable_initial_value::get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);

    auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i) {
        return i->get_name() == actual_variable_name;
    });

    if (x != a.get_list_of_variable_instances().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x)->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "its value equals " << b.get_value(**x) << "\n";

            return b.get_value(**x);
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable\n";
            return j.get_variable_instance_value((*x)->get_name());

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable\n";
            if (o)
            {
                auto r = o->get_variable_instance_value((*x)->get_name());

                //std::cout << "output variable value " << r << "\n";
                return r;
            }
            break;
        }
    }

    return input::unknown_marker;
}

void expression_variable_initial_value::assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);

    //std::cout << "actual variable name " << actual_variable_name << "\n";

    auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i) {
        return i->get_name() == actual_variable_name;
    });

    if (x != a.get_list_of_variable_instances().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x)->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
            b.this_state_is_impossible(**x, v);
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable - cannot be changed\n";
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }

}

void expression_variable_initial_value::assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v)
{
    std::string actual_variable_name = my_variable_name->get_actual_name(source);

    //std::cout << "actual variable name " << actual_variable_name << "\n";

    auto x = std::find_if(a.get_list_of_variable_instances().begin(), a.get_list_of_variable_instances().end(), [&actual_variable_name](auto & i) {
        return i->get_name() == actual_variable_name;
    });

    if (x != a.get_list_of_variable_instances().end())
    {
        //std::cout << "identified variable " << actual_variable_name << "\n";
        switch ((*x)->get_mode())
        {
        case variable_mode::HIDDEN:
        {
            //std::cout << "it is a hidden variable\n";
            //std::cout << "eliminate all but " << v << "\n";
            b.all_but_one_state_is_impossible(**x, v);
        }
        break;

        case variable_mode::INPUT:
            //std::cout << "it is an input variable - cannot be changed\n";
            break;

        case variable_mode::OUTPUT:
            //std::cout << "it is an output variable - up to decision\n";
            break;
        }
    }

}
