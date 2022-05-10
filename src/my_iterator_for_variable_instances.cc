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


agent::my_iterator_for_variable_instances::my_iterator_for_variable_instances(agent & a, unsigned d): 
    my_iterator_for_estimating_variable_instances{a, d},
    amount_of_checked_variable_instances{0}
{
    map_variable_instances_to_list_of_possible_values.clear();
    
    for (auto i(my_agent.list_of_variable_instances.begin()); i!=my_agent.list_of_variable_instances.end(); i++)
    {
        if ((*i)->get_mode()==variable_mode::INPUT || (*i)->get_mode()==variable_mode::HIDDEN)
        {
            auto [it, success] = map_variable_instances_to_list_of_possible_values.insert(std::pair((*i)->get_name(), 
                list_of_possible_values_and_some_other_stuff((*i)->get_mode(), (*i)->get_has_usual_values(), amount_of_variable_instances)));
            if (!success)
            {
                throw std::runtime_error("failed to insert a variable instance");
            }
            
                                    
            if ((*i)->get_has_usual_values())
            {
                for (auto j((*i)->get_list_of_usual_values().begin()); j!=(*i)->get_list_of_usual_values().end(); j++)
                {
                    (*it).second.get_list().push_back(*j);
                }
                (*it).second.set_has_exactly_one_usual_value((*it).second.get_list().size()==1);
                
                for (auto j((*i)->get_vector_of_value_instances().begin()); j!=(*i)->get_vector_of_value_instances().end(); j++)
                {
                    auto a = std::find_if((*i)->get_list_of_usual_values().begin(), (*i)->get_list_of_usual_values().end(), [&j](auto & k){ return k == (*j)->get_name(); });
                    if (a == (*i)->get_list_of_usual_values().end())
                    {
                        (*it).second.add_unusual_value((*j)->get_name());
                    }
                }
            }
            else
            {
                // there is no "usually" clause, all values are usual
                for (auto j((*i)->get_vector_of_value_instances().begin()); j!=(*i)->get_vector_of_value_instances().end(); j++)
                {            
                    (*it).second.get_list().push_back((*j)->get_name());
                    (*it).second.add_unusual_value((*j)->get_name());
                }
                (*it).second.set_has_exactly_one_usual_value((*it).second.get_list().size()==1);    // it should be never true
            }
                                                        
            auto [it2, success2] = map_variable_instances_to_values.insert(std::pair((*i)->get_name(), map_variable_instances_to_list_of_possible_values.at((*i)->get_name()).get_begin(my_vector_of_indices.get_allows_unusual_values_at(amount_of_variable_instances-1))));
            
            if (!success2)
            {
                throw std::runtime_error("failed to insert a variable instance iterator");
            }
            
            auto [it3, success3] = map_variable_instances_to_end_iterator.insert(std::pair((*i)->get_name(), map_variable_instances_to_list_of_possible_values.at((*i)->get_name()).get_end(my_vector_of_indices.get_allows_unusual_values_at(amount_of_variable_instances-1))));

            if (!success3)
            {
                throw std::runtime_error("failed to insert a variable instance end iterator");
            }
            
        }
    }            
}


void agent::my_iterator_for_variable_instances::reinitialize()
{
    unsigned index = 0;
    for (auto i(my_agent.list_of_variable_instances.begin()); i!=my_agent.list_of_variable_instances.end(); i++)
    {
        if ((*i)->get_mode()==variable_mode::INPUT || (*i)->get_mode()==variable_mode::HIDDEN)
        {
            index++;
            map_variable_instances_to_values.at((*i)->get_name()) = map_variable_instances_to_list_of_possible_values.at((*i)->get_name()).get_begin(my_vector_of_indices.get_allows_unusual_values_at(index-1));
        }
    }    
    amount_of_checked_variable_instances = 0;
}



bool agent::my_iterator_for_variable_instances::get_is_valid() const
{
    for (auto i(map_variable_instances_to_values.begin()); i!=map_variable_instances_to_values.end(); i++)
    {
        unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
        bool allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
        
        if ((*i).second == map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values))
        {
            return false;
        }
    }
    
    return true;
}



void agent::my_iterator_for_variable_instances::report(std::ostream & s) const
{
    bool first = true;
        
    s << "[" << amount_of_variable_instances << "]";
    
    my_vector_of_indices.report(s);
    
    s << "{";

    for (auto & x: vector_of_variable_instances_names)
    {
        auto i = map_variable_instances_to_values.find(x);
        unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
        bool has_usual_values = map_variable_instances_to_list_of_possible_values.at((*i).first).get_has_usual_values();
        bool allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
        
        if (allows_unusual_values || !has_usual_values)
        {
            if (!first) s << ",";
            
            if ((*i).second != map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values))
            {
                s << (*i).first << "=>" << *(*i).second;
            }
            else
            {
                s << (*i).first << "=>" << "END";
            }
            first = false;
        }
    }
    s << "}\n";    
}

std::string agent::my_iterator_for_variable_instances::get_value(const std::string & n) const 
{ 
    if (map_variable_instances_to_values.find(n) == map_variable_instances_to_values.end())
    {
        std::stringstream s;
        s << "there is no value for this variable instance: " << n;
        throw std::runtime_error(s.str());
    }
    //DEBUG("get_value for " << n);
    if (map_variable_instances_to_values.at(n) == map_variable_instances_to_end_iterator.at(n))
    {
        return "END";
    }
        
    return *map_variable_instances_to_values.at(n);         
}

void agent::my_iterator_for_variable_instances::on_exactly_one_usual_value(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r, bool & continue_flag)
{
    (*i).second = map_variable_instances_to_list_of_possible_values.at((*i).first).get_begin(false);
    DEBUG("for this variable instance it must be " << *(*i).second);
    if (incremented)
    {
        r++;
        DEBUG("incrementing r to " << r);
    }
    else
    {
        if (r == 0)
        {
            amount_of_checked_variable_instances=0;
            ++my_vector_of_indices;
            reinitialize();
            
            DEBUG("incrementing my_vector_of_indices");
                        
            if (my_vector_of_indices.get_finished())
            {
                processed = true;                            
            }
        }
        else
        {
            r--;
            DEBUG("decrementing r to " << r);
        }
        continue_flag = true;
    }    
}


void agent::my_iterator_for_variable_instances::on_regular_iteration(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r, bool & continue_flag)
{
    if (!incremented)
    {
        DEBUG("it is not the end, incrementing at " << r);
        (*i).second++;
        incremented=true;
    }
                
    my_agent.assume_only_the_first_n_variable_instance_known(r+1, vector_of_variable_instances_names);
    if (my_agent.get_the_iterator_is_partially_valid(r+1, *this))
    {
        //DEBUG("the iterator is partially valid for " << (r+1));
        if (r+1==vector_of_variable_instances_names.size())
        {
            amount_of_checked_variable_instances = vector_of_variable_instances_names.size();
        }
        r++;
        DEBUG("incrementing r to " << r);
        continue_flag = true;                        
    }
    else
    {
        incremented = false;
    }    
}

void agent::my_iterator_for_variable_instances::on_end_of_regular_iteration(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r, bool & continue_flag)
{                
    DEBUG("it is the end");
    if (r+1 != vector_of_variable_instances_names.size())
    {
        (*i).second = map_variable_instances_to_list_of_possible_values.at((*i).first).get_begin(allows_unusual_values);                    
                    
        incremented=false;
        r--;        
        
        DEBUG("decrementing r to " << r);
        continue_flag = true;
    }
    else
    {
        r--;
        DEBUG("decrementing r to " << r);
        continue_flag = true;
    }    
}


agent::my_iterator_for_variable_instances& agent::my_iterator_for_variable_instances::operator++()
{
    if (amount_of_variable_instances == 0)
    {
        processed = true;
    }
    else
    {        
        if (amount_of_checked_variable_instances>vector_of_variable_instances_names.size())
        {
            throw std::runtime_error("internal error - amount_of_checked_variable_instances exceeds vector_of_variable_instances_names.size()");
        }
        
        if (amount_of_checked_variable_instances == 0)
        {
            report(std::cout);
            throw std::runtime_error("internal error - amount of checked variable instances equals 0");
        }
        
        DEBUG("incrementing the iterator, begin from " << amount_of_checked_variable_instances);
        
        incremented = false;
                
        for (unsigned r=amount_of_checked_variable_instances-1; (r<vector_of_variable_instances_names.size()) && !processed;)            
        {
            auto & x=vector_of_variable_instances_names[r];
            auto i=map_variable_instances_to_values.find(x);
                                    
            unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
            allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
            has_exactly_one_usual_value = map_variable_instances_to_list_of_possible_values.at((*i).first).get_has_exactly_one_usual_value();            
            auto my_end = map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values);
                        
            DEBUG("try to increment " << x << ", number " << r << " " << (allows_unusual_values ? "allows unusual values" : "does not allow unusual values") 
                << " " << (has_exactly_one_usual_value ? "has exactly one usual value" : "has more usual values") << " " << (incremented ? "incremented" : "not incremented"));
            
            bool continue_flag = false;            
            if (!allows_unusual_values && has_exactly_one_usual_value)
            {
                on_exactly_one_usual_value(i, r, continue_flag);                
            }
            else
            if ((*i).second != my_end)
            {
                on_regular_iteration(i, r, continue_flag);
            }
            else
            {   // if (*i).second == my_end
                on_end_of_regular_iteration(i, r, continue_flag);
            }
            if (continue_flag)
                continue;
        }
    }
    return *this;
}

bool agent::my_iterator_for_variable_instances::get_finished() const
{
    return processed;
}
