#include "triglav.h"
#include <algorithm>
using namespace triglav;

belief::belief(agent & a)
{
    for (auto i(a.get_list_of_variable_instances().begin()); i!=a.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::HIDDEN)
        {
            vector_of_projections.push_back(std::make_shared<projection>(**i));
        }
    }
}

belief::belief(const belief & b)
{
    for (auto i(b.get_vector_of_projections().begin()); i!=b.get_vector_of_projections().end(); i++)
    {
        std::shared_ptr<projection> p = std::make_shared<projection>(**i);
        vector_of_projections.push_back(p);
    }
}

void belief::update_set_of_possible_values(set_of_possible_values & target, variable_instance & i)
{
    auto x=std::find_if(vector_of_projections.begin(), vector_of_projections.end(),[&i](auto p) {
        return &p->get_variable_instance() == &i;
    });

    if (x != vector_of_projections.end())
    {
        (*x)->update_set_of_possible_values(target);
    }
}

void belief::set_possible_values(set_of_possible_values & st, variable_instance & i)
{
    auto x=std::find_if(vector_of_projections.begin(), vector_of_projections.end(),[&i](auto p) {
        return &p->get_variable_instance() == &i;
    });

    if (x != vector_of_projections.end())
    {
        (*x)->set_possible_values(st);
    }
}


void belief::aggregate(const belief & b)
{
    //std::cout << "aggregate belief\n";
    auto i(vector_of_projections.begin());
    auto j(b.get_vector_of_projections().begin());

    for (; (i!=vector_of_projections.end()) && (j!=b.get_vector_of_projections().end()); i++,j++)
    {
        if ((*i)->get_variable_instance().get_name() == (*j)->get_variable_instance().get_name()) // should be always true
        {
            (*i)->aggregate(**j);
        }
    }
}



void belief::initialize_for_aggregation()
{
    for (auto i(vector_of_projections.begin()); i!=vector_of_projections.end(); i++)
    {
        (*i)->initialize_for_aggregation();
    }
}

std::string belief::get_value(variable_instance & i) const
{
    auto x=std::find_if(vector_of_projections.begin(), vector_of_projections.end(),[&i](auto p) {
        return &p->get_variable_instance() == &i;
    });

    if (x != vector_of_projections.end())
    {
        if ((*x)->get_amount_of_possible_states() == 1)
        {
            return (*x)->get_value();
        }
        else
        {
            return input::unknown_marker;
        }
    }

    return input::unknown_marker;
}

bool belief::get_is_known(variable_instance & i) const
{
    // identify the projection
    auto x=std::find_if(vector_of_projections.begin(), vector_of_projections.end(),[&i](auto p) {
        return &p->get_variable_instance() == &i;
    });

    if (x != vector_of_projections.end())
    {
        return (*x)->get_amount_of_possible_states() == 1;
    }

    return false;
}

void belief::report(std::ostream & s) const
{
    s << "belief - amount of projections " << vector_of_projections.size() << "\n";
    iterate(*this,[&s](auto i) {
        i->report(s);
    });
}

void belief::all_but_one_state_is_impossible(variable_instance & i, const std::string & v)
{
    // identify the projection
    auto x=std::find_if(vector_of_projections.begin(), vector_of_projections.end(),[&i](auto p) {
        return &p->get_variable_instance() == &i;
    });

    if (x != vector_of_projections.end())
    {
        (*x)->all_but_one_state_is_impossible(v);
    }
}


void belief::this_state_is_impossible(variable_instance & i, const std::string & v)
{
    auto x=std::find_if(vector_of_projections.begin(), vector_of_projections.end(),[&i](auto p) {
        return &p->get_variable_instance() == &i;
    });

    if (x != vector_of_projections.end())
    {
        (*x)->this_state_is_impossible(v);
    }
}

void belief::normalize()
{
    iterate(*this,[](auto i) {
        i->normalize();
    });
}


void belief::projection::initialize_for_aggregation()
{
    iterate(*this,[](auto i) {
        i->set_is_impossible();
    });
}


void belief::projection::update_set_of_possible_values(set_of_possible_values & target)
{
    for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
    {
        if (!target.get_contains((*i)->get_value_instance().get_name()))
        {
            target.push_back((*i)->get_value_instance().get_name());
        }
    }
}

void belief::projection::set_possible_values(set_of_possible_values & st)
{
    for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
    {
        if (st.get_contains((*i)->get_value_instance().get_name()))
        {
            (*i)->set_is_possible();
            //std::cout << "I THINK " << my_hidden_variable_instance.get_name() << " may be " << (*i)->get_value_instance().get_name() << "\n";
            (*i)->set_probability(1.0f);    // it will be normalized afterwards
        }
        else
        {
            (*i)->set_is_impossible();
            (*i)->set_probability(0.0f);
        }
    }
}


void belief::projection::aggregate(const projection & p)
{
    auto i(list_of_states.begin());
    auto j(p.get_list_of_states().begin());

    for (; (i!=list_of_states.end()) && (j!=p.get_list_of_states().end()); i++, j++)
    {
        if ((*i)->get_value_instance().get_name() == (*j)->get_value_instance().get_name()) // should be always true
        {
            (*i)->aggregate(**j);
        }
    }
}


bool belief::projection::get_is_not_certain() const
{
    return get_amount_of_possible_states()>1;
}

belief::projection::projection(const projection & p): my_hidden_variable_instance{p.my_hidden_variable_instance}
{
    for (auto i(p.list_of_states.begin()); i!=p.list_of_states.end(); i++)
    {
        list_of_states.push_back(std::make_shared<state>(**i));
    }
}

void belief::projection::normalize()
{
    float sum = 0.0f;
    iterate(*this, [&sum](auto i) {
        sum += i->get_probability();
    });

    if (sum > 0.0f)
    {
        iterate(*this, [&sum](auto i) {
            i->set_probability(i->get_probability()/sum);
        });
    }
    else
    {
        float amount_of_states = list_of_states.size();
        iterate(*this, [amount_of_states](auto i) {
            i->set_probability(1.0f/amount_of_states);
        });
    }
}


void belief::projection::all_but_one_state_is_impossible(const std::string & v)
{
    for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
    {
        if ((*i)->get_value_instance().get_name() == v)
        {
            (*i)->set_is_possible();
            //std::cout << "I AM SURE " << my_hidden_variable_instance.get_name() << " equals " << v << "\n";
            (*i)->set_probability(1.0f);
        }
        else
        {
            (*i)->set_is_impossible();
            (*i)->set_probability(0.0f);
        }
    }
}

void belief::projection::this_state_is_impossible(const std::string & v)
{
    for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
    {
        if ((*i)->get_value_instance().get_name() == v)
        {
            (*i)->set_is_impossible();
            //std::cout << "I AM SURE " << my_hidden_variable_instance.get_name() << " does not equal " << v << "\n";
        }
    }

    if (get_amount_of_possible_states() == 1)
    {
        //std::cout << "I AM SURE " << my_hidden_variable_instance.get_name() << " is !" << v << "\n";
    }
}


std::string belief::projection::get_value() const
{
    auto x=std::find_if(list_of_states.begin(), list_of_states.end(), [](auto s) {
        return s->get_is_possible();
    });

    if (x != list_of_states.end())
    {
        return (*x)->get_value_instance().get_name();
    }
    return input::unknown_marker;
}

void belief::projection::report(std::ostream & s) const
{
    if (get_amount_of_possible_states() == 1)
    {
        for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
        {
            if ((*i)->get_is_possible())
            {
                s << my_hidden_variable_instance.get_name() << "==" << (*i)->get_value_instance().get_name() << " is certain\n";
            }
        }
    }
    else
    {
        for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
        {
            if ((*i)->get_is_possible())
            {
                s << my_hidden_variable_instance.get_name() << "==" << (*i)->get_value_instance().get_name() << " (" <<  std::showpoint << (*i)->get_probability() << ") is possible\n";
            }
        }
    }
}

belief::projection::projection(variable_instance & i): my_hidden_variable_instance{i}
{
    for (auto j(my_hidden_variable_instance.get_vector_of_value_instances().begin()); j!=my_hidden_variable_instance.get_vector_of_value_instances().end(); j++)
    {
        list_of_states.push_back(std::make_shared<state>(**j));
    }
}

bool belief::projection::get_all_states_are_possible() const
{
    for (auto i(list_of_states.begin()); i!=list_of_states.end(); i++)
    {
        if (!(*i)->get_is_possible())
        {
            return false;
        }
    }
    return true;
}

unsigned belief::projection::get_amount_of_possible_states() const
{
    unsigned result = 0;
    iterate(*this, [&result](auto i) {
        if (i->get_is_possible()) {
            result++;
        }
    });
    return result;
}



