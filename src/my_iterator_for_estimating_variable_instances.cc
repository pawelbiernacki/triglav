#include "triglav.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sys/wait.h>
#include <unistd.h>

using namespace triglav;
//#define TRIGLAV_DEBUG

#ifdef TRIGLAV_DEBUG
#define DEBUG(X) std::cout << __FILE__ << " " << __LINE__ << ":" << X << "\n"
#else
#define DEBUG(X)
#endif



agent::my_iterator_for_estimating_variable_instances& agent::my_iterator_for_estimating_variable_instances::operator++()
{
    if (amount_of_variable_instances == 0)
    {
        processed = true;
    }
    else
    {
        ++my_vector_of_indices;
                        
        if (my_vector_of_indices.get_finished())
        {
            processed = true;
        }
    }
    
    return *this;
}


void agent::my_iterator_for_estimating_variable_instances::report(std::ostream & s) const
{        
    s << "[" << amount_of_variable_instances << "]";
    
    my_vector_of_indices.report(s);
}

agent::my_iterator_for_estimating_variable_instances::vector_of_indices_and_some_other_stuff::vector_of_indices_and_some_other_stuff(unsigned d): 
    amount_of_variable_instances{0}, depth{d}, finished{false}
{
    if (depth > 0)
    {
        vector_of_indices_can_be_unusual.resize(depth);
        for (int i = 0; i < depth; i++)
        {
            vector_of_indices_can_be_unusual[i] = 0;
        }
    }
}


void agent::my_iterator_for_estimating_variable_instances::vector_of_indices_and_some_other_stuff::report(std::ostream & s) const
{
    bool first = true;
    
    s << "[";
    
    for (int i=0; i<depth; i++)
    {
        if (!first)
            s << ",";
        s << vector_of_indices_can_be_unusual[i];
        first = false;
    }
    
    s << "]";
}

agent::my_iterator_for_estimating_variable_instances::vector_of_indices_and_some_other_stuff&  agent::my_iterator_for_estimating_variable_instances::vector_of_indices_and_some_other_stuff::operator++()
{
    for (int i=0; i<vector_of_indices_can_be_unusual.size(); i++)
    {
        if (vector_of_indices_can_be_unusual[i] < amount_of_variable_instances)
        {
            vector_of_indices_can_be_unusual[i]++;            
            break;
        }
        else
        {
            vector_of_indices_can_be_unusual[i] = 0;
            if (i<vector_of_indices_can_be_unusual.size()-1)
            {
                continue;
            }
            else
            {
                finished = true;
            }
        }
    }
    
    DEBUG("after incrementation the vector of indices is ");
    //report(std::cout);
    
    
    return *this;
}

bool agent::my_iterator_for_estimating_variable_instances::vector_of_indices_and_some_other_stuff::get_finished() const
{
    return finished;
}


bool agent::my_iterator_for_estimating_variable_instances::get_finished() const
{
    return my_vector_of_indices.get_finished();
}


bool agent::my_iterator_for_estimating_variable_instances::vector_of_indices_and_some_other_stuff::get_allows_unusual_values_at(unsigned index) const
{
    for (int i=0; i<vector_of_indices_can_be_unusual.size(); i++)
    {
        if (vector_of_indices_can_be_unusual[i] == index)
            return true;
    }
    return false;
}


agent::my_iterator_for_estimating_variable_instances::my_iterator_for_estimating_variable_instances(agent & a, unsigned d):
    my_agent{a}, depth{d}, processed{false}, amount_of_variable_instances{0}, my_vector_of_indices{d}
{
    vector_of_variable_instances_names.clear();
    for (auto i(my_agent.list_of_variable_instances.begin()); i!=my_agent.list_of_variable_instances.end(); i++)
    {
        if ((*i)->get_mode()==variable_mode::INPUT || (*i)->get_mode()==variable_mode::HIDDEN)
        {
            amount_of_variable_instances++;
            vector_of_variable_instances_names.push_back((*i)->get_name());
        }
    }
    
    std::sort(vector_of_variable_instances_names.begin(), vector_of_variable_instances_names.end(), [&a](auto x, auto y){ return a.get_variable_instance_rank(x)<a.get_variable_instance_rank(y); });
        
    my_vector_of_indices.set_amount_of_variable_instances(vector_of_variable_instances_names.size());
}


bool agent::my_iterator_for_estimating_variable_instances::get_is_matching(const std::string & vr) const
{
    std::stringstream s;
    report(s);    
    return s.str()==vr;
}

