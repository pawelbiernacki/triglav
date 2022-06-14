#include "triglav.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

using namespace triglav;
//#define TRIGLAV_DEBUG

#ifdef TRIGLAV_DEBUG
#define DEBUG(X) std::cout << __FILE__ << " " << __LINE__ << ":" << X << "\n"
#else
#define DEBUG(X)
#endif

std::ostream & operator<<(std::ostream & s, const triglav::agent::my_iterator_for_variable_instances::list_of_possible_values_and_some_other_stuff & l)
{
    l.report(s);
    return s;
}


void agent::my_iterator_for_variable_instances::list_of_possible_values_and_some_other_stuff::report(std::ostream & s) const
{
    s << "list_of_possible_values_and_some_other_stuff:\nlist: ";
    for (auto & t: my_list)
    {
        s << t << " ";
    }
    s << "\n";
    s << "my_list_of_unusual_values:\n";
    for (auto & t: my_list_of_unusual_values)
    {
        s << t << " ";
    }
    s << "\n";
}

void agent::my_iterator_for_variable_instances::initialize_list_item_if_it_has_usual_values(std::list<std::shared_ptr<variable_instance>>::iterator i, std::map<std::string, list_of_possible_values_and_some_other_stuff>::iterator it)
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
                
                
    DEBUG("for variable instance " << (*i)->get_name() << " we got " << (*it).second);    
}


void agent::my_iterator_for_variable_instances::initialize_list_item_if_it_has_no_usual_values(std::list<std::shared_ptr<variable_instance>>::iterator i, std::map<std::string, list_of_possible_values_and_some_other_stuff>::iterator it)
{
                // there is no "usually" clause, all values are usual
                for (auto j((*i)->get_vector_of_value_instances().begin()); j!=(*i)->get_vector_of_value_instances().end(); j++)
                {            
                    (*it).second.get_list().push_back((*j)->get_name());
                    (*it).second.add_unusual_value((*j)->get_name());
                }
                (*it).second.set_has_exactly_one_usual_value((*it).second.get_list().size()==1);    // it should be never true
                
    DEBUG("for variable instance " << (*i)->get_name() << " we got " << (*it).second);                    
}

void agent::my_iterator_for_variable_instances::initialize_list_item(std::list<std::shared_ptr<variable_instance>>::iterator i, std::map<std::string, list_of_possible_values_and_some_other_stuff>::iterator it)
{
            if ((*i)->get_has_usual_values())
            {
                initialize_list_item_if_it_has_usual_values(i, it);
                
            }
            else
            {
                initialize_list_item_if_it_has_no_usual_values(i, it);
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


agent::my_iterator_for_variable_instances::my_iterator_for_variable_instances(agent & a, unsigned d): 
    my_iterator_for_estimating_variable_instances{a, d},
    amount_of_checked_variable_instances{0}
{
    map_variable_instances_to_list_of_possible_values.clear();
    
    auto index=0;
    
    for (auto i(my_agent.list_of_variable_instances.begin()); i!=my_agent.list_of_variable_instances.end(); i++)
    {
        if ((*i)->get_mode()==variable_mode::INPUT || (*i)->get_mode()==variable_mode::HIDDEN)
        {
            auto [it, success] = map_variable_instances_to_list_of_possible_values.insert(std::pair((*i)->get_name(), 
                list_of_possible_values_and_some_other_stuff((*i)->get_mode(), (*i)->get_has_usual_values(), index++)));
            if (!success)
            {
                throw std::runtime_error("failed to insert a variable instance");
            }

            initialize_list_item(i, it);
                                    
        }
    }
    calculate_amount_of_checked_variable_instances();
}


void agent::my_iterator_for_variable_instances::calculate_amount_of_checked_variable_instances()
{
    amount_of_checked_variable_instances = 0;
    for (unsigned r=0;r<vector_of_variable_instances_names.size();r++)
    {
        my_agent.assume_only_the_first_n_variable_instance_known(r+1, vector_of_variable_instances_names);
        if (my_agent.get_the_iterator_is_partially_valid(r+1,*this))
        {
            amount_of_checked_variable_instances = r+1;
            continue;
        }
        else
        {
            return;
        }
    }
}



void agent::my_iterator_for_variable_instances::partial_reinitialize(unsigned r)
{
    DEBUG("agent::my_iterator_for_variable_instances::partial_reinitialize " << r);
    
    if (r<vector_of_variable_instances_names.size())
    {
        auto & x=vector_of_variable_instances_names[r];                        
        auto i=map_variable_instances_to_values.find(x);
                                    
        unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
        allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
        has_exactly_one_usual_value = map_variable_instances_to_list_of_possible_values.at((*i).first).get_has_exactly_one_usual_value();            
        auto my_end = map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values);
                
        (*i).second = map_variable_instances_to_list_of_possible_values.at((*i).first).get_begin(allows_unusual_values);                                        
        
        DEBUG("assigned with " << *(*i).second);
        
    }
    
    DEBUG("agent::my_iterator_for_variable_instances::partial_reinitialize done");
    
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

bool agent::my_iterator_for_variable_instances::get_is_partially_not_end(unsigned n)
{
    for (unsigned j=0; j<n; j++)
    {
        auto & x = vector_of_variable_instances_names[j];
        auto i = map_variable_instances_to_values.find(x);
        unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
        bool allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
        if ((*i).second == map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values))
        {
            return false;
        }
    }
    return true;
}

bool agent::my_iterator_for_variable_instances::get_is_valid() const
{
    if (amount_of_checked_variable_instances <vector_of_variable_instances_names.size())
    {
        return false;
    }
    
    for (auto i(map_variable_instances_to_values.begin()); i!=map_variable_instances_to_values.end(); i++)
    {
        unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
        bool allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
        
        if ((*i).second == map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values)
            
            || (*i).second == map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(!allows_unusual_values)            
        )
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
            
            if ((*i).second != map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values) 
                && (*i).second != map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(!allows_unusual_values))
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


bool agent::my_iterator_for_variable_instances::get_is_assumed_ok(unsigned r)
{
    my_agent.assume_only_the_first_n_variable_instance_known(r+1, vector_of_variable_instances_names);
    if (my_agent.get_the_iterator_is_partially_valid(r+1, *this))
    {
        return true;
    }
    return false;
}


void agent::my_iterator_for_variable_instances::partial_report(std::ostream & s) const
{
    bool first = true;
        
    s << "PARTIAL REPORT [" << amount_of_variable_instances << "]";
    
    my_vector_of_indices.report(s);
    
    s << "{";

    
    unsigned number=0;
    for (auto & x: vector_of_variable_instances_names)
    {
        if (++number == amount_of_checked_variable_instances)
        {
            break;
        }
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


void agent::my_iterator_for_variable_instances::on_exactly_one_usual_value(std::map<std::string, std::list<std::string>::iterator>::iterator & i, unsigned & r)
{
    (*i).second = map_variable_instances_to_list_of_possible_values.at((*i).first).get_begin(false);
    DEBUG("for this variable instance (" << current_variable_instance_name << ") it must be " << *(*i).second);
            
    if (incremented)
    {
        if (get_is_assumed_ok(r))
        {
            DEBUG("the iterator is partially valid for " << (r+1));
            amount_of_checked_variable_instances = r+1;
            r++;
            DEBUG("incrementing r to " << r);
            partial_reinitialize(r);
        }
        else
        {
            incremented = false;
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
                amount_of_checked_variable_instances = r;
                DEBUG("decrementing r to " << r);
                partial_reinitialize(r+1);
            }            
        }
    }
    else
    {
        incremented = false;
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
            amount_of_checked_variable_instances = r;
            DEBUG("decrementing r to " << r);
            partial_reinitialize(r+1);
        }
    }
    
}



void agent::my_iterator_for_variable_instances::on_regular_iteration(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r)
{
    if (!incremented)
    {
        (*i).second++;
        incremented=true;
        DEBUG("it is not the end, incrementing at " << r << " the variable instance " << current_variable_instance_name);
        partial_reinitialize(r+1);
    }                
    
    if (incremented)
    {        
        if (get_is_assumed_ok(r))
        {        
            DEBUG("the iterator is partially valid for " << (r+1) << " variable instances");
            //partial_report(std::cout);
            amount_of_checked_variable_instances = r + 1;        
            r++;
            DEBUG("incrementing r to " << r);                        
        }
        else
        {
            incremented = false;
        }
    }
    else
    {
        incremented = false;
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
            amount_of_checked_variable_instances = r;
            DEBUG("decrementing r to " << r);
            partial_reinitialize(r+1);
        }
    }
}

void agent::my_iterator_for_variable_instances::on_end_of_regular_iteration(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r)
{                
    DEBUG("it is the end, setting to begin at " << r << " the variable instance " << current_variable_instance_name);
    (*i).second = map_variable_instances_to_list_of_possible_values.at((*i).first).get_begin(allows_unusual_values);                                    
    partial_reinitialize(r+1);
                
    
    if (incremented)
    {
        if (get_is_assumed_ok(r))
        {                
            DEBUG("the iterator is partially valid for " << (r+1));
            //partial_report(std::cout);
            amount_of_checked_variable_instances = r + 1;        
            r++;
            DEBUG("incrementing r to " << r);
        }
        else
        {
            incremented = false;
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
                amount_of_checked_variable_instances = r;
                DEBUG("decrementing r to " << r);
                partial_reinitialize(r+1);
            }        
        }
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
            amount_of_checked_variable_instances = r;
            DEBUG("decrementing r to " << r);
            partial_reinitialize(r+1);
        }        
    }
}


agent::my_iterator_for_variable_instances& agent::my_iterator_for_variable_instances::operator++()
{
    DEBUG("agent::my_iterator_for_variable_instances::operator++");
    
    
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

        unsigned first_r;
        
        if (amount_of_checked_variable_instances == 0)
        {
            first_r = 0;
        }
        else
        {
            first_r = amount_of_checked_variable_instances-1;
        }
                
        DEBUG("incrementing the iterator, begin from " << amount_of_checked_variable_instances);
        
        incremented = false;
        
        for (unsigned r=first_r; (r<vector_of_variable_instances_names.size()) && !processed;)            
        {
            current_variable_instance_name=vector_of_variable_instances_names[r];            
                        
            auto i=map_variable_instances_to_values.find(current_variable_instance_name);
                                    
            unsigned index = map_variable_instances_to_list_of_possible_values.at((*i).first).get_index();        
            allows_unusual_values = my_vector_of_indices.get_allows_unusual_values_at(index);
            has_exactly_one_usual_value = map_variable_instances_to_list_of_possible_values.at((*i).first).get_has_exactly_one_usual_value();            
            auto my_end = map_variable_instances_to_list_of_possible_values.at((*i).first).get_end(allows_unusual_values);
                        
            DEBUG("try to increment " << current_variable_instance_name << ", number " << r << " " << (allows_unusual_values ? "allows unusual values" : "does not allow unusual values") << " " << (has_exactly_one_usual_value ? "has exactly one usual value" : "has more usual values") << " " << (incremented ? "incremented" : "not incremented"));
            
            //partial_report(std::cout);
                                    
            if (!allows_unusual_values && has_exactly_one_usual_value)
            {
                on_exactly_one_usual_value(i, r);                
            }
            else
            if ((*i).second != my_end)
            {
                on_regular_iteration(i, r);
            }
            else
            {   // if (*i).second == my_end
                on_end_of_regular_iteration(i, r);
            }
        }
    }
    return *this;
}

bool agent::my_iterator_for_variable_instances::get_finished() const
{
    return processed;
}
