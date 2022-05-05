#include "triglav.h"
#include <sstream>
#include <algorithm>
using namespace triglav;


const std::string input::unknown_marker="<unknown>";


input::input(agent & a): my_agent{a}, payoff{0.0f}, probability{0.0f}
{
}


void input::there_is_a_chance(float p)
{
    /*
    std::cout << "there_is_a_chance(" << std::showpoint << p << ")\n";
    e.report(std::cout);
    std::cout << ", ";
    source.report(std::cout);
    std::cout << "\n";
    */
    set_probability(p);
}

void input::report(std::ostream & s) const
{
    for (auto i(vector_of_input_items.begin()); i!=vector_of_input_items.end(); i++)
    {
        (*i)->report(s);
    }
}

void input::create_all_input_items()
{
    for (auto i(my_agent.get_list_of_variable_instances().begin()); i!=my_agent.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::INPUT)
        {
            vector_of_input_items.push_back(std::make_shared<input_item>(**i, nullptr));
        }
    }
}

void input::all_but_one_state_is_impossible(variable_instance & i, const std::string & v)
{
    auto x = std::find_if(vector_of_input_items.begin(), vector_of_input_items.end(), [&v](auto y) {
        return y->get_variable_instance().get_name()==v;
    });

    if (x != vector_of_input_items.end())
    {
        (*x)->all_but_one_state_is_impossible(v);
    }
}

std::string input::get_variable_instance_value(const std::string i) const
{
    auto x = std::find_if(vector_of_input_items.begin(), vector_of_input_items.end(), [&i](auto y) {
        return y->get_variable_instance().get_name()==i;
    });

    if (x != vector_of_input_items.end() && (*x)->get_is_defined())
    {
        return (*x)->get_value_instance().get_name();
    }


    return unknown_marker;
}

void input::parse_line()
{
    std::stringstream s(line), s1, s2;
    char c;

    while (s.get(c))
    {
        if (c == '=')
        {
            break;
        }
        else
        {
            s1 << c;
        }
    }

    while (s.get(c))
    {
        s2 << c;
    }

    std::string variable_name = s1.str(), value_name = s2.str();

    //std::cout << "got variable " << s1.str() << " and value " << s2.str() << "\n";

    for (auto i(my_agent.get_list_of_variable_instances().begin()); i!=my_agent.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_name() == variable_name && !get_question_has_been_answered((*i)->get_name()))
        {
            auto x = std::find_if((*i)->get_vector_of_value_instances().begin(), (*i)->get_vector_of_value_instances().end(),[&value_name](auto y) {
                return y->get_name()==value_name;
            });

            if (x != (*i)->get_vector_of_value_instances().end())
            {
                vector_of_input_items.push_back(std::make_shared<input_item>(**i, &(**x)));

                std::cout << "triglav: " << (*i)->get_name() << "==" << (*x)->get_name() << "\n";
            }
            else
            {
                vector_of_input_items.push_back(std::make_shared<input_item>(**i, &(*(*i)->get_vector_of_value_instances()[0])));
            }
            return;
        }
    }
}

bool input::get_all_questions_have_been_answered()
{
    for (auto i(my_agent.get_list_of_variable_instances().begin()); i!=my_agent.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::INPUT && !get_question_has_been_answered((*i)->get_name()))
        {
            question = (*i)->get_name();
            return false;
        }
    }
    return true;
}

bool input::get_question_has_been_answered(const std::string & q) const
{
    for (auto i(vector_of_input_items.begin()); i!=vector_of_input_items.end(); i++)
    {
        if ((*i)->get_variable_instance().get_name() == q)
        {
            return true;
        }
    }
    return false;
}

void input::input_item::all_but_one_state_is_impossible(const std::string & v)
{
    std::cout << "this input item equals " << v << "\n";

    auto x = std::find_if(my_variable_instance.get_vector_of_value_instances().begin(), my_variable_instance.get_vector_of_value_instances().end(),
    [&v](auto i) {
        return i->get_name() == v;
    });

    if (x != my_variable_instance.get_vector_of_value_instances().end())
    {
        std::cout << "setting to " << v << "\n";
        my_value_instance = &(**x);
    }

}


void input::input_item::reset()
{
    my_value_instance = nullptr;
}

void input::input_item::report(std::ostream & s) const
{
    if (my_value_instance)
    {
        s << my_variable_instance.get_name() << "==" << my_value_instance->get_name() << "\n";
    }
}
