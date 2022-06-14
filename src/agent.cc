#include "triglav.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sys/wait.h>
#include <unistd.h>
#include <thread>

using namespace triglav;
//#define TRIGLAV_DEBUG

#ifdef TRIGLAV_DEBUG
#define DEBUG(X) std::cout << __FILE__ << " " << __LINE__ << ":" << X << "\n"
#else
#define DEBUG(X)
#endif


extern "C" void triglav_open_file(void * scanner, const char * f);
extern "C" void triglav_close_file(void * scanner);
extern "C" void triglavlex_init(void **);
extern "C" void triglavlex_destroy(void *);


agent::agent(): 
        debug{false}, 
        amount_of_actions{0}, 
        depth{0}, 
        amount_of_processors{1}, 
        max_amount_of_unusual_cases{1}, 
        amount_of_case_files{0},
        case_files_prefix{""},
        case_files_extension{"txt"},
        case_files_path{"."}
        {}


void agent::calculate_the_consider_ranks()
{
    rank = 0;
    for (auto & i: list_of_commands)
    {
        if (i->get_is_consider_command())
        {
            i->set_rank(rank++);
        }
    }
}

int agent::get_variable_instance_rank(const std::string & n) const
{
    auto i=std::find_if(map_variable_instances_to_consider_rank.begin(), map_variable_instances_to_consider_rank.end(),
                                                            [&n](auto & x){ return x.first == n; });
    if (i != map_variable_instances_to_consider_rank.end())
    {
        return i->second;
    }
    return rank;
}


void agent::clear_validation_items()
{
    vector_of_validation_items.clear();
}
    
void agent::add_validation_item(const std::string & v, int processor_id)
{
    vector_of_validation_items.push_back(std::pair(v,processor_id));    
}


void agent::add_consider_variable_instance(const std::string & n, int rank)
{
    auto [it, success]=map_variable_instances_to_consider_rank.insert(std::pair(n, rank));
    if (!success)
    {
        throw std::runtime_error("unable to consider the variable instance twice");
    }
}


void agent::add_variable_instance(std::shared_ptr<variable_instance> && v)
{
    auto x = std::find_if(list_of_variable_instances.begin(), list_of_variable_instances.end(),[&v](auto y) {
        return y->get_name()==v->get_name();
    });

    if (x == list_of_variable_instances.end())
    {
        std::shared_ptr<variable_instance> new_pointer = v;
        std::string name{new_pointer->get_name()};
        list_of_variable_instances.push_back(std::move(v));
        auto [it, success] = map_name_to_variable_instance.insert(std::pair<std::string,std::shared_ptr<variable_instance>>(name, std::move(new_pointer)));
        if (!success)
        {
            std::runtime_error("failed to insert variable instance");
        }
    }
}


void agent_reading_input_files::get_input(input & i)
{
    std::cout << "triglav: enter input file name>";
    std::string file_name;
    std::getline(std::cin, file_name);

    std::ifstream f(file_name);

    if (f.good())
    {
        while (f)
        {
            std::getline(f, i.get_line());

            if (f.good())
                i.parse_line();
        }

        f.close();

        if (!i.get_all_questions_have_been_answered())
        {
            std::cerr << "you did not answer the question " << i.get_question() << ".\n";
            exit(0);
        }

    }
    else
    {
        std::cerr << "failed to open file " << file_name << "\n";
        exit(0);
    }
}

void agent::get_input(input & i)
{
    while (!i.get_all_questions_have_been_answered())
    {
        std::cout << "triglav: " << i.get_question() << "?>";
        std::getline(std::cin, i.get_line());
        i.parse_line();
    }

    std::cout << "triglav: I have no more questions\n";
}

void agent::populate_belief_for_consequence(belief & b, input & j, input * j2, output & o, belief & target)
{
    belief b_aggregated{*this};

    b_aggregated.initialize_for_aggregation();

    for (int k=0; k<amount_of_repetitions; k++)
    {
        for (auto p(list_of_rules.begin()); p!=list_of_rules.end(); p++)
        {
            (*p)->try_to_apply_as_a_static_rule(*this, b_aggregated, j, nullptr);
        }
    }
    std::cout << "belief before aggregation:\n";
    b_aggregated.report(std::cout);


    for (auto m(b.get_vector_of_projections().begin()); m!=b.get_vector_of_projections().end(); m++)
    {
        std::cout << "consider belief projection " << (*m)->get_variable_instance().get_name() << "\n";
        if ((*m)->get_amount_of_possible_states() > 1)
        {
            for (auto n((*m)->get_list_of_states().begin()); n!=(*m)->get_list_of_states().end(); n++)
            {
                if ((*n)->get_is_possible())
                {
                    std::cout << "assume belief state " << (*m)->get_variable_instance().get_name() << "=" << (*n)->get_value_instance().get_name() << "\n";

                    belief b_with_assumption{b};

                    b_with_assumption.all_but_one_state_is_impossible((*m)->get_variable_instance(), (*n)->get_value_instance().get_name());
                    b_with_assumption.normalize();

                    for (int k=0; k<amount_of_repetitions; k++)
                    {
                        for (auto p(list_of_rules.begin()); p!=list_of_rules.end(); p++)
                        {
                            (*p)->try_to_apply_as_a_static_rule(*this, b, j, nullptr);
                        }
                    }
                    for (auto k(list_of_rules.begin()); k!=list_of_rules.end(); k++)
                    {
                        (*k)->try_to_apply_as_a_dynamic_rule(*this, b, &b_with_assumption, j, j2, o);
                    }
                    b_aggregated.aggregate(b_with_assumption);
                }
            }
        }
    }

    b_aggregated.normalize();
    std::cout << "belief after aggregation and normalization:\n";
    b_aggregated.report(std::cout);
}

float agent::get_payoff_expected_value_for_consequences(belief & b, unsigned depth, input & j, output & o)
{
    // consider all possible input projections

    input j2{*this};
    float result = 0.0f;
    unsigned amount_of_branches = 0;
    auto start = std::chrono::system_clock::now();

    if (depth == 0)
    {
        return 0.0f;
    }

    j2.create_all_input_items();

    for (auto i(j2.get_vector_of_input_items().begin()); i!=j2.get_vector_of_input_items().end(); i++)
    {
        std::cout << "consider projection " << (*i)->get_variable_instance().get_name() << "\n";

        for (auto l((*i)->get_variable_instance().get_vector_of_value_instances().begin()); l!=(*i)->get_variable_instance().get_vector_of_value_instances().end(); l++)
        {
            std::cout << "consider input value " << (*l)->get_name() << "\n";
            (*i)->all_but_one_state_is_impossible((*l)->get_name());

            j2.reset_payoff();
            j2.reset_probability();

            belief b2{*this};

            populate_belief_for_consequence(b, j, &j2, o, b2);

            if (o.get_illegal()) // some assertion must have failed
            {
                o.set_legal();
                continue;
            }

            std::cout << "for " << (*i)->get_variable_instance().get_name() << "=" << (*l)->get_name() << " probability " << std::showpoint << j2.get_probability() << "\n";

            if (j2.get_probability() > 0.0f)
            {
                output decision{*this};
                time_in_seconds elapsed2;

                get_optimal_action(b2, depth-1, j2, decision, elapsed2);

                result += j2.get_probability()*(j2.get_payoff()+get_payoff_expected_value_for_consequences(b2, depth-1, j2, decision));
                amount_of_branches ++;
            }

            (*i)->reset();
        }
    }

    if (amount_of_branches == 0)
        return 0.0f;


    auto end = std::chrono::system_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "elapsed " << elapsed.count() << "s\n";

    return result/static_cast<float>(amount_of_branches);
}

void agent::get_optimal_action(belief & b, unsigned depth, input & j, output & decision, time_in_seconds & elapsed)
{
    bool decision_has_been_made=false;
    float e = std::numeric_limits<float>::lowest(), e0;

    auto start = std::chrono::system_clock::now();

    if (depth == 0)
    {
        auto end = std::chrono::system_clock::now();

        elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        return;
    }

    for (int k=0; k<amount_of_repetitions; k++)
    {
        for (auto i(list_of_rules.begin()); i!=list_of_rules.end(); i++)
        {
            (*i)->try_to_apply_as_a_static_rule(*this, b, j, nullptr);
        }
    }

    for (output o{*this}; !o.get_all_actions_have_been_processed(); ++o)
    {
        if (o.get_is_valid())
        {
            std::cout << "consider action ";
            o.report(std::cout);
            std::cout << "\n";


            for (auto i(list_of_rules.begin()); i!=list_of_rules.end() && !o.get_illegal(); i++)
            {
                (*i)->try_to_apply_as_a_dynamic_rule(*this, b, nullptr, j, nullptr, o);
            }

            if (!o.get_illegal())
            {
                std::cout << "it is legal\n";

                e0 = get_payoff_expected_value_for_consequences(b, depth, j, o);

                if (!decision_has_been_made || e0>e)
                {
                    decision = o;
                    decision_has_been_made = true;
                    e = e0;
                }

            }
            else
            {
                std::cout << "it is not legal\n";
            }
        }
    }

    std::cout << "optimal action ";
    decision.report(std::cout);
    std::cout << "\n";

    auto end = std::chrono::system_clock::now();

    elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
}


void agent::loop(unsigned depth)
{
    current_belief = std::make_shared<belief>(*this);

    current_belief->normalize();

    do
    {
        input i{*this};
        i.reset_payoff();   // not really needed, since payoff is initialized with 0.0 in the input's constructor
        get_input(i);

        current_belief->report(std::cout);

        output decision{*this};
        time_in_seconds elapsed;
        get_optimal_action(*current_belief, depth, i, decision, elapsed);

        std::cout << "get_optimal_action took " << elapsed.count() << "s\n";

        std::cout << "current situation payoff " << std::showpoint << i.get_payoff() << "\n";
    }
    while (true);
}

void agent::report_input_variables(std::ostream & s) const
{
    for (auto i(list_of_variable_instances.begin()); i!=list_of_variable_instances.end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::INPUT)
        {
            (*i)->report(s);
            s << "\n";
        }
    }
}

template <typename SOME_CLASS> void iterate_and_report(SOME_CLASS & x, std::ostream & s)
{
    for (auto i(x.begin()); i!=x.end(); i++)
    {
        (*i)->report(s);
        s << "\n";
    }
}

void agent::report(std::ostream & s) const
{
    iterate_and_report(list_of_types, s);
    iterate_and_report(list_of_variables, s);
    iterate_and_report(list_of_type_instances, s);
    iterate_and_report(list_of_variable_instances, s);
    iterate_and_report(list_of_rules, s);

    std::cout << "amount_of_actions=" << amount_of_actions << "\n";
}

void agent::expand(unsigned depth)
{
    for (auto i(list_of_types.begin()); i!=list_of_types.end(); i++)
    {
        std::shared_ptr<type> t{*i};
        list_of_type_instances.push_back(std::make_shared<type_instance>((*i)->get_name(), std::move(t)));
    }

    for (unsigned i=1; i<=depth; i++)
    {
        internal_expand_for_types(i);
    }

    internal_expand_for_values();
    internal_expand_for_variables();

    for (output o{*this}; !o.get_all_actions_have_been_processed(); ++o)
    {
        if (o.get_is_valid())
        {
            amount_of_actions++;
        }
    }
}


void agent::estimate_cases()
{    
    for (my_iterator_for_estimating_variable_instances m(*this, max_amount_of_unusual_cases); !m.get_finished(); ++m)
    {
        m.report(std::cout);
        std::cout << "\n";
    }    
}

void agent::precalculate()
{
    std::vector<pid_t> vector_of_pids;
    pid_t p;
    
    for (unsigned i = 1; i < amount_of_processors; i++)
    {
        p = fork();
        if (p == 0)
        {
            // child
            std::cout << "starting child process " << i << "\n";
            precalculate_for_processor(i);
            exit(EXIT_SUCCESS);
        }
        else
        if (p == -1)
        {
            std::cerr << "failed to start child process " << i << "\n";
            exit(EXIT_FAILURE);
        }
        else
        {
            vector_of_pids.push_back(p);
        }
    }
        
    precalculate_for_processor(0);
}



void agent::internal_expand_for_variables()
{
    for (auto i(list_of_variables.begin()); i!=list_of_variables.end(); i++)
    {
        auto &list = (*i)->get_name_list();


        if (list.get_contains_expandable_items())
        {
            for (my_iterator_for_variables l{*this, list}; !l.get_finished(); ++l)
            {
                if (l.get_is_valid())
                {
                    //l.report(std::cout);


                    std::stringstream s;
                    auto & list2(list.get_list_of_name_items());

                    bool ok=true;
                    for (auto m(list2.begin()); m!=list2.end(); m++)
                    {
                        if ((*m)->get_is_expandable())
                        {
                            if (l.get_current_name((*m)->get_placeholder_name()) == (*m)->get_placeholder_name())
                            {
                                ok=false;
                                break;
                            }
                            s << l.get_current_name((*m)->get_placeholder_name());
                        }
                        else
                        {
                            (*m)->report(s);
                        }
                    }

                    if (ok)
                    {
                        std::shared_ptr<variable> j=*i;


                        switch ((*i)->get_mode())
                        {
                        case variable_mode::HIDDEN:
                        {
                            std::shared_ptr<variable_instance> k=std::make_shared<hidden_variable_instance>(s.str(), std::move(j));

                            auto& l=(*i)->get_type();
                            if (l->get_is_type_name())
                            {
                                auto m=std::find_if(list_of_type_instances.begin(), list_of_type_instances.end(),[&l](auto & x) {
                                    return x->get_name()==l->get_type_name();
                                });

                                if (m != list_of_type_instances.end())
                                {
                                    k->set_type_instance(*m);
                                    for (auto n((*m)->get_list_of_value_instances().begin()); n!=(*m)->get_list_of_value_instances().end(); n++)
                                    {
                                        std::shared_ptr<value_instance> o=*n;
                                        k->add(std::move(o));
                                    }
                                }
                            }
                            add_variable_instance(std::move(k));
                        }
                        break;

                        case variable_mode::INPUT:
                        {
                            std::shared_ptr<variable_instance> k=std::make_shared<input_variable_instance>(s.str(), std::move(j));

                            auto& l=(*i)->get_type();
                            if (l->get_is_type_name())
                            {
                                auto m=std::find_if(list_of_type_instances.begin(), list_of_type_instances.end(),[&l](auto & x) {
                                    return x->get_name()==l->get_type_name();
                                });

                                if (m != list_of_type_instances.end())
                                {
                                    k->set_type_instance(*m);
                                    for (auto n((*m)->get_list_of_value_instances().begin()); n!=(*m)->get_list_of_value_instances().end(); n++)
                                    {
                                        std::shared_ptr<value_instance> o=*n;
                                        k->add(std::move(o));
                                    }
                                }
                            }
                            add_variable_instance(std::move(k));
                        }
                        break;

                        case variable_mode::OUTPUT:
                        {
                            std::shared_ptr<variable_instance> k=std::make_shared<output_variable_instance>(s.str(), std::move(j));

                            auto& l=(*i)->get_type();
                            if (l->get_is_type_name())
                            {
                                auto m=std::find_if(list_of_type_instances.begin(), list_of_type_instances.end(),[&l](auto & x) {
                                    return x->get_name()==l->get_type_name();
                                });

                                if (m != list_of_type_instances.end())
                                {
                                    k->set_type_instance(*m);
                                    for (auto n((*m)->get_list_of_value_instances().begin()); n!=(*m)->get_list_of_value_instances().end(); n++)
                                    {
                                        std::shared_ptr<value_instance> o=*n;
                                        k->add(std::move(o));
                                    }
                                }
                            }
                            add_variable_instance(std::move(k));
                        }
                        break;

                        }
                    }
                }
            }
        }
        else
        {
            std::stringstream s;
            auto & list2(list.get_list_of_name_items());
            for (auto m(list2.begin()); m!=list2.end(); m++)
            {
                (*m)->report(s);
            }

            std::shared_ptr<variable> j=*i;


            switch ((*i)->get_mode())
            {
            case variable_mode::HIDDEN:
            {
                std::shared_ptr<variable_instance> k=std::make_shared<hidden_variable_instance>(s.str(), std::move(j));

                auto& l=(*i)->get_type();
                if (l->get_is_type_name())
                {
                    auto m=std::find_if(list_of_type_instances.begin(), list_of_type_instances.end(),[&l](auto & x) {
                        return x->get_name()==l->get_type_name();
                    });

                    if (m != list_of_type_instances.end())
                    {
                        k->set_type_instance(*m);
                        for (auto n((*m)->get_list_of_value_instances().begin()); n!=(*m)->get_list_of_value_instances().end(); n++)
                        {
                            std::shared_ptr<value_instance> o=*n;
                            k->add(std::move(o));
                        }
                    }
                }
                add_variable_instance(std::move(k));
            }
            break;

            case variable_mode::INPUT:
            {
                std::shared_ptr<variable_instance> k=std::make_shared<input_variable_instance>(s.str(), std::move(j));

                auto& l=(*i)->get_type();
                if (l->get_is_type_name())
                {
                    auto m=std::find_if(list_of_type_instances.begin(), list_of_type_instances.end(),[&l](auto & x) {
                        return x->get_name()==l->get_type_name();
                    });

                    if (m != list_of_type_instances.end())
                    {
                        k->set_type_instance(*m);
                        for (auto n((*m)->get_list_of_value_instances().begin()); n!=(*m)->get_list_of_value_instances().end(); n++)
                        {
                            std::shared_ptr<value_instance> o=*n;
                            k->add(std::move(o));
                        }
                    }
                }
                add_variable_instance(std::move(k));
            }
            break;

            case variable_mode::OUTPUT:
            {
                std::shared_ptr<variable_instance> k=std::make_shared<output_variable_instance>(s.str(), std::move(j));

                auto& l=(*i)->get_type();
                if (l->get_is_type_name())
                {
                    auto m=std::find_if(list_of_type_instances.begin(), list_of_type_instances.end(),[&l](auto & x) {
                        return x->get_name()==l->get_type_name();
                    });

                    if (m != list_of_type_instances.end())
                    {
                        k->set_type_instance(*m);
                        for (auto n((*m)->get_list_of_value_instances().begin()); n!=(*m)->get_list_of_value_instances().end(); n++)
                        {
                            std::shared_ptr<value_instance> o=*n;
                            k->add(std::move(o));
                        }
                    }
                }
                add_variable_instance(std::move(k));
            }
            break;

            }
        }
    }
}



void agent::internal_expand_for_types(unsigned i)
{
    for (auto j(list_of_type_instances.begin()); j!=list_of_type_instances.end(); j++)
    {
        //std::cout << "internal expand for " << i << " type instance " << (*j)->get_name() << "\n";

        if (i == 1)
        {
            auto & list((*j)->get_type().lock()->get_list_of_generic_names());
            for (auto k(list.begin()); k!=list.end(); k++)
            {
                if (!(*k)->get_contains_expandable_items())
                {
                    std::stringstream s;
                    auto & list2((*k)->get_list_of_name_items());
                    bool first=true;
                    for (auto l(list2.begin()); l!=list2.end(); l++)
                    {
                        //if (!first) s << "_";
                        (*l)->report(s);
                        first = false;
                    }
                    (*j)->add(std::make_shared<value_instance>(s.str(), 1));
                }
            }
        }
        else
        {
            auto & list((*j)->get_type().lock()->get_list_of_generic_names());
            for (auto k(list.begin()); k!=list.end(); k++)
            {
                if ((*k)->get_contains_expandable_items())
                {
                    /*
                    std::cout << "the item ";
                    (*k)->report(std::cout);
                    std::cout << " contains expandable items\n";
                    */

                    for (my_iterator_for_types l{*this, *k, i}; !l.get_finished(); ++l)
                    {
                        if (l.get_is_valid())
                        {
                            //l.report(std::cout);

                            std::stringstream s;
                            auto & list2((*k)->get_list_of_name_items());
                            bool first=true;
                            bool ok=true;
                            for (auto m(list2.begin()); m!=list2.end(); m++)
                            {
                                //if (!first) s << "";
                                if ((*m)->get_is_expandable())
                                {
                                    if (l.get_current_name((*m)->get_placeholder_name()) == (*m)->get_placeholder_name())
                                    {
                                        ok=false;
                                        break;
                                    }
                                    s << l.get_current_name((*m)->get_placeholder_name());
                                }
                                else
                                {
                                    (*m)->report(s);
                                }

                                first = false;
                            }

                            if (ok)
                            {
                                (*j)->add(std::make_shared<value_instance>(s.str(), i));
                            }
                        }
                    }
                }
            }
        }
    }
}


void agent::internal_expand_for_values()
{
    for (auto j(list_of_type_instances.begin()); j!=list_of_type_instances.end(); j++)
    {
        for (auto k((*j)->get_list_of_value_instances().begin()); k!=(*j)->get_list_of_value_instances().end(); k++)
        {
            std::string name = (*k)->get_name();
            std::shared_ptr<value_instance> new_pointer=*k;
            list_of_value_instances.push_back(*k);
            auto [it, success] = map_name_to_value_instance.insert(std::pair<std::string, std::shared_ptr<value_instance>>(name, std::move(new_pointer)));
            if (!success)
            {
                throw std::runtime_error("failed to insert value instance");
            }
        }
    }
}


int agent::parse_error_to_stderr(class_with_scanner * data)
{
    cpp_parser p(data->scanner,*this, &std::cerr);
    return p.parse();
}

void agent::execute()
{
    for (auto i(list_of_commands.begin()); i!=list_of_commands.end(); i++)
    {
        (*i)->execute(*this);
    }
}



void agent::generate_cases()
{    
    std::vector<pid_t> vector_of_pids;
    
    
    for (unsigned i = 1; i < amount_of_processors; i++)
    {
        pid_t p = fork();
        
        if (p == 0)
        {
            generate_cases_for_processor(i);
            exit(0);
        }
        else
        if (p < 0)
        {
            std::cerr << "failed to fork\n";
        }
        else
        {
            vector_of_pids.push_back(p);
        }                
    }
    std::cout << "the processor 0 is starting the operation\n";    
    generate_cases_for_processor(0);        
}


void agent::precalculate_for_processor(int processor_id)
{
    std::vector<std::string> vector_of_case_file_names;
    
    std::sort(vector_of_precalculated_files.begin(), vector_of_precalculated_files.end(), [](auto &a, auto & b){ return a->get_depth()<b->get_depth(); });
    
    std::cout << "precalculate for processor " << processor_id << "\n";
    
    for (auto & a: vector_of_precalculated_files)
    {
        if (a->get_done())
        {
            std::cout << "processor " << processor_id << ", depth " << a->get_depth() << " has already been done\n";
        }
        else
        {
            if (a->get_depth()==1)
            {
                std::cout << "processor " << processor_id << " precalculating for depth=1\n";
                
                for (int n=0; n<amount_of_case_files; n++)
                {
                    std::stringstream case_file_name_stream;
                    case_file_name_stream << case_files_path << "/" << case_files_prefix << processor_id << "_" << n << "." << case_files_extension;
                    
                    vector_of_case_file_names.push_back(case_file_name_stream.str());                    
                }
                
                for (auto & case_file_name: vector_of_case_file_names)
                {
                    class_with_scanner c;
                    c.agent_ptr = this;

                    triglavlex_init ( &c.scanner );
                    
                    std::cout << "processor " << processor_id << " precalculating case file " << case_file_name << "\n";
                    
                    triglav_open_file(c.scanner, case_file_name.c_str());

                    cases_parser p(c.scanner,*this, &std::cerr);
                    int i = p.parse();

                    if (i != 0)
                        std::cout << "error" << "\n";

                    triglav_close_file(c.scanner);
                    triglavlex_destroy(c.scanner);
                }                
            }
            else
            {
                std::cout << "processor " << processor_id << " precalculating for depth=" << depth << "\n";
            }
        }
    }
}


void agent::generate_cases_for_processor(int processor_id)
{    
    /*
    std::cout << "generating cases for:\n";
    for (auto & i: vector_of_validation_items)
    {
        std::cout << i << "\n";
    }
    */
    
    my_multifile = std::make_shared<my_output_multifile>(case_files_path, case_files_prefix, amount_of_case_files, case_files_extension, processor_id);    

    
    for (my_iterator_for_estimating_variable_instances m(*this, max_amount_of_unusual_cases); !m.get_finished(); ++m)
    {
        std::stringstream s;
        m.report(s);
        std::string name=s.str();
        
        if (std::find_if(vector_of_validation_items.begin(), vector_of_validation_items.end(), [&name, processor_id](auto &i){ return i.first==name && i.second==processor_id;})!=vector_of_validation_items.end())
        {
            /*
            std::cout << "consider validation range " << s.str() << " ";
            m.report(std::cout);
            std::cout << "\n";
            */
            
            for (my_single_range_iterator_for_variable_instances n{m, name}; !n.get_finished(); ++n)
            {                                
                if (n.get_is_valid())
                {                    
                    n.report(my_multifile->get_random_output_file_stream());
                    n.report(std::cout);                    
                }
            } 
        }
    }    
    
    my_multifile = nullptr;
}


int agent::parse(const char * filename)
{
    class_with_scanner c;
    c.agent_ptr = this;

    triglavlex_init ( &c.scanner );
    triglav_open_file(c.scanner, filename);

    int i = parse_error_to_stderr(&c);

    if (i != 0)
        std::cout << "error" << "\n";

    triglav_close_file(c.scanner);
    triglavlex_destroy (c.scanner);

    return i;
}


void agent::assume_only_the_first_n_variable_instance_known(unsigned n, const std::vector<std::string> & v)
{
    map_name_to_assumed_flag.clear();
    
    //DEBUG("assume " << n << " variables are known");
    for (unsigned i=0; i<n; i++)
    {
        auto [it, success] = map_name_to_assumed_flag.insert(std::pair(v[i], true));
        if (!success)
        {
            throw std::runtime_error("failed to assume some variable instances known");
        }
        //DEBUG("assume " << v[i] << " is known");                
    }
    map_name_to_assumed_value.clear();
}
    
std::string agent::get_assumed_value(const std::string & n) const
{
    return map_name_to_assumed_value.at(n);
}
    
    
    
    
bool agent::get_the_iterator_is_partially_valid(unsigned n, my_iterator_for_variable_instances & i)
{
    bool impossible = false;
        
    if (!i.get_is_partially_not_end(n))
    {
        return false;
    }
    
    for (auto x: map_name_to_assumed_flag)
    {
        auto [it, success]=map_name_to_assumed_value.insert(std::pair(x.first, i.get_value(x.first)));
        if (!success)
        {
            throw std::runtime_error("failed to assume a value");
        }
        DEBUG("got " << (*it).first << " with value " << (*it).second);
    }
    
    for (auto & j: list_of_rules)
    {
        if (j->try_to_apply_as_assumption_rule(*this, impossible))
        {
            if (impossible)
            {
                DEBUG("the rule " << j->get_comment() << " is violated!");
                return false;
            }
        }
    }    
    DEBUG("no rule is violated!");    
    
    return true;
}

bool agent::get_is_assumed(const std::string & n) const
{
    return map_name_to_assumed_flag.find(n)!=map_name_to_assumed_flag.end();
}




agent::my_iterator_for_rules::my_iterator_for_rules(agent & a, rule & r): my_iterator{a}, my_rule{r}
{
}


agent::my_iterator_for_variables::my_iterator_for_variables(agent & a, generic_name & n): my_iterator{a}, name_being_expanded{n}
{
    auto & list(name_being_expanded.get_list_of_name_items());
    for (auto i(list.begin()); i!=list.end(); i++)
    {
        if ((*i)->get_is_expandable())
        {
            auto & te = (*i)->get_type_expression();

            if (te->get_is_type_name())
            {
                //std::cout << "iterator gets all values of the type " << te->get_type_name() << "\n";

                auto [it, success] = map_placeholders_to_list_of_possible_values.insert(std::pair((*i)->get_placeholder_name(), std::list<std::string>()));

                if (!success)
                {
                    throw std::runtime_error("failed to insert a placeholder");
                }

                auto x = std::find_if(my_agent.list_of_type_instances.begin(), my_agent.list_of_type_instances.end(),
                [&te](auto & ti) {
                    return ti->get_name() == te->get_type_name();
                });

                if (x != my_agent.list_of_type_instances.end())
                {
                    bool found = false;
                    for (auto j((*x)->get_list_of_value_instances().begin()); j!=(*x)->get_list_of_value_instances().end(); j++)
                    {
                        found = true;
                        map_placeholders_to_list_of_possible_values.at((*i)->get_placeholder_name()).push_back((*j)->get_name());
                    }

                    if (found)
                    {
                        amount_of_placeholders++;
                        auto [it, success] = map_placeholders_to_values.insert(std::pair((*i)->get_placeholder_name(), map_placeholders_to_list_of_possible_values.at((*i)->get_placeholder_name()).begin()));
                        if (!success)
                        {
                            throw std::runtime_error("failed to insert a placeholder");
                        }
                    }
                }
            }
        }
    }
}

agent::my_iterator_for_types::my_iterator_for_types(agent & a, std::shared_ptr<generic_name> & n, unsigned l): my_iterator{a}, name_being_expanded{n}, level{l}
{
    auto & list(name_being_expanded->get_list_of_name_items());
    for (auto i(list.begin()); i!=list.end(); i++)
    {
        if ((*i)->get_is_expandable())
        {
            auto & te = (*i)->get_type_expression();

            if (te->get_is_type_name())
            {
                //std::cout << "iterator gets all values of the type " << te->get_type_name() << " of level " << (level-1) << "\n";

                auto [it, success] = map_placeholders_to_list_of_possible_values.insert(std::pair((*i)->get_placeholder_name(), std::list<std::string>()));

                if (!success)
                {
                    throw std::runtime_error("failed to insert a placeholder");
                }

                auto x = std::find_if(my_agent.list_of_type_instances.begin(), my_agent.list_of_type_instances.end(),
                [&te](auto & ti) {
                    return ti->get_name() == te->get_type_name();
                });

                if (x != my_agent.list_of_type_instances.end())
                {
                    bool found = false;
                    for (auto j((*x)->get_list_of_value_instances().begin()); j!=(*x)->get_list_of_value_instances().end(); j++)
                    {
                        if ((*j)->get_level() <= level-1)
                        {
                            found = true;
                            map_placeholders_to_list_of_possible_values.at((*i)->get_placeholder_name()).push_back((*j)->get_name());
                        }
                    }

                    if (found)
                    {
                        amount_of_placeholders++;
                        auto [it, success] = map_placeholders_to_values.insert(std::pair((*i)->get_placeholder_name(), map_placeholders_to_list_of_possible_values.at((*i)->get_placeholder_name()).begin()));
                        if (!success)
                        {
                            throw std::runtime_error("failed to insert a placeholder");
                        }
                    }
                }
            }
        }
    }
}

