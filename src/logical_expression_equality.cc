#include "triglav.h"
#include <iterator>
using namespace triglav;

void logical_expression_equality::assert_is_true(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed)
{
    assertion_failed = false;
    set_of_possible_values s1, s2, s3;
    auto it = s3.begin();

    left->update_set_of_possible_values(s1, a, b, b2, source, j, j2, nullptr);
    right->update_set_of_possible_values(s2, a, b, b2, source, j, j2, nullptr);

    s3.resize(s1.size()+s2.size());
    it = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), s3.begin());
    s3.resize(it-s3.begin());

    if (s3.empty())
    {
        assertion_failed = true;
        return;
    }

    /*
    std::cout << "intersection ";
    s1.report(std::cout);
    std::cout << " and ";
    s2.report(std::cout);
    std::cout << " equals ";
    s3.report(std::cout);
    std::cout << "\n";
    */

    left->set_possible_values(s3, a, b, b2, source, j, j2);
    right->set_possible_values(s3, a, b, b2, source, j, j2);
}

void logical_expression_equality::assert_is_false(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed)
{
    assertion_failed = false;

    if (left->get_can_be_evaluated(a, b, source, j, j2, nullptr) && !right->get_can_be_evaluated(a, b, source, j, j2, nullptr))
    {
        // right != left
        std::string v = left->get_evaluate(a, b, b2, source, j, j2, nullptr);

        /*
        std::cout << "left can be evaluated: ";
        left->report(std::cout);
        std::cout << "(equals " << v << ") and right cannot be evaluated: ";
        right->report(std::cout);
        std::cout << "\n";
        */
        right->assert_is_different(a, b, b2, source, j, j2, v);


    }
    else if (!left->get_can_be_evaluated(a, b, source, j, j2, nullptr) && right->get_can_be_evaluated(a, b, source, j, j2, nullptr))
    {
        // left != right
        std::string v = right->get_evaluate(a, b, b2, source, j, j2, nullptr);

        /*
        std::cout << "left cannot be evaluated: ";
        left->report(std::cout);
        std::cout << " and right can be evaluated: ";
        right->report(std::cout);
        std::cout << "(equals " << v << ")\n";
        */
        left->assert_is_different(a, b, b2, source, j, j2, v);
    }
    else if (left->get_can_be_evaluated(a, b, source, j, j2, nullptr) && right->get_can_be_evaluated(a, b, source, j, j2, nullptr))
    {
        std::string v1 = left->get_evaluate(a, b, b2, source, j, j2, nullptr);
        std::string v2 = left->get_evaluate(a, b, b2, source, j, j2, nullptr);

        if (v1 == v2)
        {
            std::cout << "both can be evaluated, but they are equal\n";
            assertion_failed = true;
        }
    }
}

bool logical_expression_equality::get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o)
{
    set_of_possible_values s1, s2, s3;
    auto it = s3.begin();

    left->update_set_of_possible_values(s1, a, b, nullptr, source, j, j2, o);
    right->update_set_of_possible_values(s2, a, b, nullptr, source, j, j2, o);

    s3.resize(s1.size()+s2.size());
    it = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), s3.begin());
    s3.resize(it-s3.begin());

    if (s3.empty())
    {
        return true;
    }

    bool l = left->get_can_be_evaluated(a, b, source, j, j2, o);
    bool r = right->get_can_be_evaluated(a, b, source, j, j2, o);

    /*
    left->report(std::cout);
    std::cout << " RESULT " << l << "\n";

    right->report(std::cout);
    std::cout << " RESULT " << r << "\n";

    std::cout << "logical_expression_equality::get_can_be_evaluated " << l << "&&" << r << "\n";
    */
    return l && r;
}

bool logical_expression_equality::get_can_be_evaluated_given_assumption(agent & a, my_iterator & source)
{
    bool l = left->get_can_be_evaluated_given_assumption(a, source);
    bool r = right->get_can_be_evaluated_given_assumption(a, source);
    return l && r;    
}



void logical_expression_equality::explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    set_of_possible_values s1, s2, s3;
    auto it = s3.begin();

    left->update_set_of_possible_values(s1, a, b, b2, source, j, j2, o);
    right->update_set_of_possible_values(s2, a, b, b2, source, j, j2, o);

    s3.resize(s1.size()+s2.size());
    it = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), s3.begin());
    s3.resize(it-s3.begin());

    if (s3.empty())
    {
        left->report(std::cout);
        std::cout << "==";
        right->report(std::cout);
        std::cout << " is false, because ";
        s1.report(std::cout);
        std::cout << " & ";
        s2.report(std::cout);
        std::cout << "={}\n";
        return;
    }
    auto l = left->get_evaluate(a, b, b2, source, j, j2, o);
    auto r = right->get_evaluate(a, b, b2, source, j, j2, o);

    left->report(std::cout);
    std::cout << "==";
    right->report(std::cout);
    std::cout << " is " << (l == r ? "true" : "false") << ", because " << l << "==" << r << "=" << (l == r ? "true" : "false") << "\n";
}

bool logical_expression_equality::get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o)
{
    set_of_possible_values s1, s2, s3;
    auto it = s3.begin();

    left->update_set_of_possible_values(s1, a, b, b2, source, j, j2, o);
    right->update_set_of_possible_values(s2, a, b, b2, source, j, j2, o);

    s3.resize(s1.size()+s2.size());
    it = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), s3.begin());
    s3.resize(it-s3.begin());

    if (s3.empty())
    {
        return false;
    }

    auto l = left->get_evaluate(a, b, b2, source, j, j2, o);
    auto r = right->get_evaluate(a, b, b2, source, j, j2, o);
    //std::cout << "CONTROL left=" << l << ", right=" << r << "\n";

    return l == r;
}

