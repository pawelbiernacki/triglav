#include "triglav.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sys/wait.h>
#include <unistd.h>

using namespace triglav;
#define TRIGLAV_DEBUG

#ifdef TRIGLAV_DEBUG
#define DEBUG(X) std::cout << __FILE__ << " " << __LINE__ << ":" << X << "\n"
#else
#define DEBUG(X)
#endif


void agent::my_single_range_iterator_for_variable_instances::init_for_single_usual_value(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag)
{
    (*i).second = map_variable_instances_to_list_of_possible_values.at((*i).first).get_begin(false);
            
    DEBUG("for " << variable_name << " it must be " << *(*i).second);
    
    my_agent.assume_only_the_first_n_variable_instance_known(r+1, vector_of_variable_instances_names);
    
    if (my_agent.get_the_iterator_is_partially_valid(r+1, *this))
    {
        DEBUG("the iterator is partially valid for " << (r+1));
        if (r+1==vector_of_variable_instances_names.size())
        {
            amount_of_checked_variable_instances = vector_of_variable_instances_names.size();
            DEBUG("assign amount_of_checked_variable_instances to " << vector_of_variable_instances_names.size());                                                
            continue_flag = false;
        }
        else
        {
            DEBUG("we increment r");
            r++;
            continue_flag = true;
        }
    }
    else
    {
        if (r == 0)
        {
            amount_of_checked_variable_instances=0;
            ++my_vector_of_indices;
            reinitialize();
            if (my_vector_of_indices.get_finished())
            {                    
                DEBUG("assign amount_of_checked_variable_instances to vector_of_variable_instances_names.size()");
                amount_of_checked_variable_instances = vector_of_variable_instances_names.size();
                    
                processed = true;
                continue_flag = false;
            }
            else
            {
                continue_flag = true;
            }
        }
        else
        {
            DEBUG("we decrement r");
            r--;
            continue_flag = true;
        }
    }
    
    amount_of_checked_variable_instances=r;
    
    continue_flag = true;
}


void agent::my_single_range_iterator_for_variable_instances::init_for_regular_value(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag)
{
    DEBUG("for " << variable_name << " it isn't finished yet, assume " << (r+1) << " are known");
    
    my_agent.assume_only_the_first_n_variable_instance_known(r+1, vector_of_variable_instances_names);
    
    if (my_agent.get_the_iterator_is_partially_valid(r+1, *this))
    {
        DEBUG("the iterator is partially valid for " << (r+1));
        if (r+1==vector_of_variable_instances_names.size())
        {
            amount_of_checked_variable_instances = vector_of_variable_instances_names.size();
            DEBUG("assign amount_of_checked_variable_instances to " << vector_of_variable_instances_names.size());                                                
            continue_flag = false;
        }
        else
        {
            DEBUG("we increment r");
            r++;
            continue_flag = true;
        }
    }
    else
    {
        DEBUG("the iterator is not partially valid for " << (r+1));
        (*i).second++;
        continue_flag = true;
    }
 
}

void agent::my_single_range_iterator_for_variable_instances::init_for_end(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag)
{
            
    DEBUG("for " << variable_name << " it is finished");
    if (r == 0)
    {
        amount_of_checked_variable_instances=0;
        ++my_vector_of_indices;
        reinitialize();
        if (my_vector_of_indices.get_finished())
        {                    
            DEBUG("assign amount_of_checked_variable_instances to vector_of_variable_instances_names.size()");
                    amount_of_checked_variable_instances = vector_of_variable_instances_names.size();
                    
            processed = true;
            continue_flag = false;
        }
    }
    else
    {
        r--;
        continue_flag = true;
    }
}


void agent::my_single_range_iterator_for_variable_instances::
init_for_given_amount_of_checked_variable_instances(unsigned & r, bool allows_unusual_values, bool has_exactly_one_usual_value, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, std::list<std::string>::iterator my_end, bool & continue_flag
)
{
        if (!allows_unusual_values && has_exactly_one_usual_value)
        {
            init_for_single_usual_value(r, i, variable_name, continue_flag);
        }
        else
        if ((*i).second != my_end)
        {        
            init_for_regular_value(r, i, variable_name, continue_flag);
        }
        else
        {
            init_for_end(r, i, variable_name, continue_flag);
        }
}


agent::my_single_range_iterator_for_variable_instances::my_single_range_iterator_for_variable_instances(const my_iterator_for_estimating_variable_instances & j, const std::string & range):
    my_iterator_for_variable_instances{j.get_agent(), j.get_depth()},
    my_range{range}
{
    DEBUG("my_single_range_iterator_for_variable_instances::my_single_range_iterator_for_variable_instances");
    
    bool continue_flag = false;
    
    for (unsigned r=amount_of_checked_variable_instances; r<vector_of_variable_instances_names.size();)            
    {
        DEBUG("checking for " << r << " (maximum is " << (vector_of_variable_instances_names.size()-1) << ")");
        auto & x=vector_of_variable_instances_names[r];
        auto i=map_variable_instances_to_values.find(x);
                                    
        unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
        bool allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
        bool has_exactly_one_usual_value = map_variable_instances_to_list_of_possible_values.at((*i).first).get_has_exactly_one_usual_value();            
        auto my_end = map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values);
            
        continue_flag = false;
        
        init_for_given_amount_of_checked_variable_instances(r, allows_unusual_values,
            has_exactly_one_usual_value, i, x, my_end, continue_flag);
        
        if (continue_flag)
            continue;
        else
            break;
    }
    
}

void agent::my_single_range_iterator_for_variable_instances::report(std::ostream & s) const
{
    my_iterator_for_variable_instances::report(s);    
}



agent::my_single_range_iterator_for_variable_instances& agent::my_single_range_iterator_for_variable_instances::operator++()
{
    if (get_is_matching(my_range))
    {
        this->my_iterator_for_variable_instances::operator++();        
    }
    else
    {
        this->my_iterator_for_estimating_variable_instances::operator++();
        reinitialize();
    }
    return *this;
}

