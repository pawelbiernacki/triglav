#include "triglav.h"
#include <sstream>
using namespace triglav;


bool rule::try_to_apply_as_a_dynamic_rule(agent & a, belief & b, belief * b2, input & j, input * j2, output & o)
{
    bool result = false;
    std::stringstream s;

    a.set_debug(false);

    /*
    std::cout << "output equals ";
    o.report(std::cout);
    std::cout << "\n";
    */

    o.report(s);

    //if (line_number == 114 && s.str()=="I_do==doing_nothing\n") a.set_debug(true);

    if (a.get_debug())
    {
        std::cout << "trying to apply dynamic rule [" << line_number << "] ";
        report(std::cout);
        std::cout << "\n";
    }

    agent::my_iterator_for_rules i{a, *this};
    condition->update_iterator(i);
    implication->update_iterator(i);

    for (; !i.get_finished(); ++i)
    {
        if (i.get_is_valid())
        {
            if (o.get_illegal())
                return true;

            if (a.get_debug())
            {
                i.report(std::cout);
            }

            if (condition->get_contains_output_variables(a, b, i, j))
            {
                if (condition->get_can_be_evaluated(a, b, i, j, j2, &o))
                {

                    if (a.get_debug())
                        std::cout << "condition can be evaluated\n";

                    if (condition->get_evaluate(a, b, b2, i, j, j2, &o))
                    {
                        if (a.get_debug())
                        {
                            std::cout << "condition ";
                            condition->report(std::cout);
                            std::cout << " for ";
                            i.report(std::cout);
                            std::cout << " is true\n";
                        }

                        implication->execute(a, b, b2, i, j, j2, &o);

                        result = true;
                    }
                    else
                    {
                        if (a.get_debug())
                        {
                            std::cout << "condition ";
                            condition->report(std::cout);
                            condition->explain_evaluation(a, b, b2, i, j, j2, &o);
                            std::cout << " is false\n";
                        }
                    }

                }
                else
                {
                    if (a.get_debug())
                        std::cout << "condition cannot be evaluated\n";
                }
            }
        }
    }

    return result;
}

bool rule::try_to_apply_as_a_static_rule(agent & a, belief & b, input & j, input * j2)
{
    agent::my_iterator_for_rules i{a, *this};
    condition->update_iterator(i);
    implication->update_iterator(i);

    a.set_debug(false);
    if (a.get_debug())
    {
        std::cout << "trying to apply static rule [" << line_number << "] ";
        report(std::cout);
        std::cout << "\n";
    }

    for (; !i.get_finished(); ++i)
    {
        if (i.get_is_valid())
        {
            if (!condition->get_contains_output_variables(a, b, i, j))
            {
                if (a.get_debug()) i.report(std::cout);

                if (condition->get_can_be_evaluated(a, b, i, j, j2, nullptr))
                {
                    if (a.get_debug()) std::cout << "condition can be evaluated\n";
                    if (condition->get_evaluate(a, b, nullptr, i, j, nullptr, nullptr))
                    {
                        if (a.get_debug()) std::cout << "condition is true\n";

                        implication->execute(a, b, nullptr, i, j, j2, nullptr);
                    }
                    else
                    {
                        if (a.get_debug()) std::cout << "condition is false\n";
                    }
                }
                else
                {
                    if (a.get_debug()) std::cout << "condition cannot be evaluated\n";
                }
            }
        }
    }

    return false;
}


bool rule::get_is_dynamic(agent & a, belief & b, my_iterator & source, input & j) const
{
    return condition->get_contains_output_variables(a,b,source,j);
}
