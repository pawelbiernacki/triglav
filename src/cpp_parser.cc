#include "triglav.h"
#include "parser.tab.h"
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <cstring>
using namespace triglav;

#define PARSING_ERROR(i, x) *my_output_stream << "[" << __LINE__ << "] line: " << lineno() << " " << x << " expected at " << get_token_name(i) << "\n"

extern "C" int triglav_lex(void*,void*);

extern "C" int triglavget_lineno(void*);


const cpp_parser::token_and_name cpp_parser::array_of_tokens_and_names[] = {
    { T_IF, "T_IF" },
    { T_EQUAL_EQUAL, "T_EQUAL_EQUAL" },
    { T_EQUAL_GREATER, "T_EQUAL_GREATER" },
    { T_AND_AND, "T_AND_AND" },
    { T_OR_OR, "T_OR_OR" },
    { T_TYPE, "T_TYPE" },
    { T_INPUT, "T_INPUT" },
    { T_OUTPUT, "T_OUTPUT" },
    { T_HIDDEN, "T_HIDDEN" },
    { T_VARIABLE, "T_VARIABLE" },
    { T_INITIAL, "T_INITIAL" },
    { T_TERMINAL, "T_TERMINAL" },
    { T_VALUE, "T_VALUE" },
    { T_ILLEGAL, "T_ILLEGAL" },
    { T_ASSERT_IS_TRUE, "T_ASSERT_IS_TRUE" },
    { T_ASSERT_IS_FALSE, "T_ASSERT_IS_FALSE" },
    { T_EXPAND, "T_EXPAND" },
    { T_LOOP, "T_LOOP" },
    { T_REPORT, "T_REPORT" },
    { T_ADD_PAYOFF, "T_ADD_PAYOFF" },
    { T_THERE_IS_A_CHANCE, "T_THERE_IS_A_CHANCE" },
    { T_USUALLY, "T_USUALLY" },
    { T_GENERATE_CASES, "T_GENERATE_CASES" },
    { T_VALIDATE_CASES, "T_VALIDATE_CASES" },
    { T_SAVE_DATABANK, "T_SAVE_DATABANK" },
    { T_OPEN_DATABANK, "T_OPEN_DATABANK" },
    { T_IMPOSSIBLE, "T_IMPOSSIBLE" },
    { T_ESTIMATE_CASES, "T_ESTIMATE_CASES" },
    { T_IDENTIFIER, "T_IDENTIFIER" },
    { T_FLOAT_LITERAL, "T_FLOAT_LITERAL" },
    { T_INT_LITERAL, "T_INT_LITERAL" },
    { T_STRING_LITERAL, "T_STRING_LITERAL" },
    { 0, "EOF" }
};

int cpp_parser::lineno() {
    return triglavget_lineno(scanner);
}

const std::string cpp_parser::get_token_name(int t)
{
    for (int i=0; array_of_tokens_and_names[i].token; i++)
    {
        if (array_of_tokens_and_names[i].token == t)
            return array_of_tokens_and_names[i].name;
    }
    if (t == 0)
        return "EOF";
    std::stringstream s;
    s << (char)t;
    return s.str();
}


int cpp_parser::lex()
{
    int i = 0;

    if (unlex_done)
    {
        i = next_token;
        unlex_done = false;
    }
    else
    {
        i = triglav_lex(&yylval, scanner); // std::cout << "token " << get_token_name(i) << "\n";
    }
    return i;
}


void cpp_parser::unlex(int t)
{
    if (!unlex_done)
    {
        unlex_done = true;
        next_token = t;
    }
}

int cpp_parser::parse_generic_name(std::shared_ptr<generic_name_list> & target)
{
    int i;
    std::shared_ptr<generic_name_list> result = std::make_shared<generic_name_list>();

    do
    {
        i = lex();
        if (i == T_IDENTIFIER)
        {
            result->add(std::make_shared<generic_name_item_token>(std::string(yylval.value_string)));
            free(yylval.value_string);
        }
        else if (i == '(')
        {
            i = lex();
            if (i != T_IDENTIFIER)
            {
                PARSING_ERROR(i, "<identifier>");
                return -1;
            }

            std::string placeholder_name = std::string(yylval.value_string);

            free(yylval.value_string);

            i = lex();
            if (i != ':')
            {
                PARSING_ERROR(i, ":");
                return -1;
            }

            i = lex();
            if (i != T_IDENTIFIER) // type name
            {
                PARSING_ERROR(i, "<identifier>");
                return -1;
            }

            std::string type_name = std::string(yylval.value_string);
            free(yylval.value_string);

            i = lex();
            if (i != ')')
            {
                PARSING_ERROR(i, ")");
                return -1;
            }

            std::shared_ptr<generic_name_item_placeholder> r = std::make_shared<generic_name_item_placeholder>(placeholder_name,
                    std::make_shared<triglav::type_expression_type_name>(type_name));

            result->add(std::move(r));
        }
        else if (i == ',')
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else if (i == '}')
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else if (i == ':')
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else if (i == T_EQUAL_EQUAL)
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else if (i == T_AND_AND)
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else if (i == T_OR_OR)
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else if (i == ')')
        {
            unlex(i);
            target = std::move(result);
            return 0;
        }
        else
        {
            PARSING_ERROR(i, "<identifier> or (");
            return -1;
        }
    }
    while (true);


    return 0;
}

int cpp_parser::parse_type()
{
    int i;
    i = lex();


    if (i != T_TYPE)
    {
        PARSING_ERROR(i, "type");
        return -1;
    }

    do
    {
        i = lex();
        if (i != T_IDENTIFIER)
        {
            PARSING_ERROR(i, "<identifier>");
            return -1;
        }
        std::shared_ptr<type> result = std::make_shared<type>(std::string(yylval.value_string));
        free(yylval.value_string);

        i = lex();
        if (i != '=')
        {
            PARSING_ERROR(i, "=");
            return -1;
        }

        i = lex();
        if (i != '{')
        {
            PARSING_ERROR(i, "{");
            return -1;
        }

        do
        {
            std::shared_ptr<generic_name_list> r;
            if (parse_generic_name(r))
            {
                return -1;
            }
            result->add(std::move(r));

            i = lex();
            if (i == ',') continue;
            if (i == '}') break;
            PARSING_ERROR(i, ", or }");
            return -1;
        }
        while (true);

        my_agent.add_type(std::move(result));

        i = lex();
        if (i == ';')
        {
            return 0;
        }
        else if (i == ',')
        {
            continue;
        }
        else
        {
            PARSING_ERROR(i, "; or ,");
            return -1;
        }
    }
    while (true);

    return 0;
}

int cpp_parser::parse_variable()
{
    int i;
    variable_mode m;
    i = lex();

    if (i == T_INPUT || i == T_OUTPUT || i == T_HIDDEN)
    {
        switch (i)
        {
        case T_INPUT:
            m = variable_mode::INPUT;
            break;
        case T_OUTPUT:
            m = variable_mode::OUTPUT;
            break;
        case T_HIDDEN:
            m = variable_mode::HIDDEN;
            break;
        }
    }
    else
    {
        PARSING_ERROR(i, "input or output or hidden");
        return -1;
    }

    i = lex();
    if (i != T_VARIABLE)
    {
        PARSING_ERROR(i, "variable");
        return -1;
    }

    do
    {
        std::shared_ptr<generic_name_list> gn;

        if (parse_generic_name(gn))
        {
            return -1;
        }
        i = lex();
        if (i != ':')
        {
            PARSING_ERROR(i, ":");
            return -1;
        }

        i = lex();
        if (i != T_IDENTIFIER)
        {
            PARSING_ERROR(i, "<identifier>");
            return -1;
        }
        std::string type_name(yylval.value_string);
        free(yylval.value_string);

        std::shared_ptr<variable> v;

        switch (m)
        {
        case variable_mode::INPUT:
            v = std::make_shared<input_variable>(std::make_shared<triglav::type_expression_type_name>(type_name));
            break;
        case variable_mode::OUTPUT:
            v = std::make_shared<output_variable>(std::make_shared<triglav::type_expression_type_name>(type_name));
            break;
        case variable_mode::HIDDEN:
            v = std::make_shared<hidden_variable>(std::make_shared<triglav::type_expression_type_name>(type_name));
            break;
        }

        for (auto i(gn->get_list_of_name_items().begin()); i!=gn->get_list_of_name_items().end(); i++)
        {
            v->add_name_item(std::move(*i));
        }


        i = lex();
        if (i == T_USUALLY)
        {
            i=lex();
            if (i != '{')
            {
                PARSING_ERROR(i, "{");
                return -1;
            }

            while (true)
            {
                i = lex();
                if (i != T_IDENTIFIER)
                {
                    PARSING_ERROR(i, "<identifier>");
                    return -1;
                }
                std::string usually_string(yylval.value_string);
                free(yylval.value_string);
                v->add_usual_value(usually_string);
                i = lex();
                if (i == '}')
                {
                    i = lex();
                    break;
                }
                if (i == ',')
                    continue;
                PARSING_ERROR(i, "} or ,");
                return -1;
            }
        }

        my_agent.add_variable(std::move(v));

        if (i == ',') continue;
        if (i == ';') break;
        PARSING_ERROR(i, ", or ;");
        return -1;
    }
    while (true);
    return 0;
}


int cpp_parser::parse_expression_variable_initial_value(std::shared_ptr<expression> & target)
{
    int i;

    //std::cout << "parse_expression_variable_initial_value\n";

    i = lex();
    if (i != T_INITIAL)
    {
        PARSING_ERROR(i, "initial");
        return -1;
    }
    i = lex();
    if (i != T_VALUE)
    {
        PARSING_ERROR(i, "value");
        return -1;
    }
    std::shared_ptr<generic_name_list> e;
    if (parse_generic_name(e))
    {
        return -1;
    }
    target = std::make_shared<expression_variable_initial_value>(std::move(e));
    return 0;
}

int cpp_parser::parse_expression_variable_terminal_value(std::shared_ptr<expression> & target)
{
    int i;
    i = lex();
    if (i != T_TERMINAL)
    {
        PARSING_ERROR(i, "terminal");
        return -1;
    }
    i = lex();
    if (i != T_VALUE)
    {
        PARSING_ERROR(i, "value");
        return -1;
    }
    std::shared_ptr<generic_name_list> e;
    if (parse_generic_name(e))
    {
        return -1;
    }

    target = std::make_shared<expression_variable_terminal_value>(std::move(e));

    return 0;
}

int cpp_parser::parse_expression_value(std::shared_ptr<expression> & target)
{
    std::shared_ptr<generic_name_list> e;
    if (parse_generic_name(e))
    {
        return -1;
    }

    target=std::make_shared<expression_value>(std::move(e));

    return 0;
}

int cpp_parser::parse_expression(std::shared_ptr<expression> & target)
{
    int i;

    //std::cout << "parse_expression\n";

    i = lex();

    //std::cout << "parse_expression got " << get_token_name(i) << "\n";

    if (i == T_INITIAL)
    {
        unlex(i);
        std::shared_ptr<expression> e;
        if (parse_expression_variable_initial_value(e))
        {
            return -1;
        }
        target = std::move(e);
    }
    else if (i == T_TERMINAL)
    {
        unlex(i);
        std::shared_ptr<expression> e;
        if (parse_expression_variable_terminal_value(e))
        {
            return -1;
        }
        target = std::move(e);
    }
    else
    {
        unlex(i);
        if (parse_expression_value(target))
        {
            return -1;
        }
    }
    return 0;
}

int cpp_parser::parse_logical_expression_equality(std::shared_ptr<logical_expression> & target)
{
    int i;
    std::shared_ptr<expression> left, right;

    //std::cout << "parse_logical_expression_equality\n";

    if (parse_expression(left))
    {
        return -1;
    }

    //std::cout << "parse_logical_expression_equality - parsed left expression\n";

    i = lex();
    if (i != T_EQUAL_EQUAL)
    {
        PARSING_ERROR(i, "==");
        return -1;
    }
    if (parse_expression(right))
    {
        return -1;
    }

    //std::cout << "parse_logical_expression_equality - parsed right expression\n";

    target = std::make_shared<logical_expression_equality>(std::move(left), std::move(right));
    return 0;
}


int cpp_parser::parse_logical_expression_not(std::shared_ptr<logical_expression> & target)
{
    int i;
    i = lex();
    if (i == '!')
    {
        std::shared_ptr<logical_expression> t;
        if (parse_logical_expression_not(t))
        {
            return -1;
        }
        target = std::make_shared<logical_expression_not>(std::move(t));
        return 0;
    }
    else
    {
        unlex(i);
        std::shared_ptr<logical_expression> t;
        if (parse_logical_expression_equality(t))
        {
            return -1;
        }
        target = std::move(t);
        return 0;
    }

    return 0;
}

int cpp_parser::parse_logical_expression_and(std::shared_ptr<logical_expression> & target)
{
    int i;
    std::shared_ptr<logical_expression> left;

    if (parse_logical_expression_not(left))
    {
        return -1;
    }

    i = lex();
    if (i == T_AND_AND)
    {
        std::shared_ptr<logical_expression> right;
        if (parse_logical_expression_and(right))
        {
            return -1;
        }
        target = std::make_shared<logical_expression_and>(std::move(left), std::move(right));
    }
    else
    {
        target = std::move(left);
        unlex(i);
    }

    return 0;
}

int cpp_parser::parse_logical_expression(std::shared_ptr<logical_expression> & target)
{
    int i;
    std::shared_ptr<logical_expression> left;

    if (parse_logical_expression_and(left))
    {
        return -1;
    }

    i = lex();
    if (i == T_OR_OR)
    {
        std::shared_ptr<logical_expression> right;
        if (parse_logical_expression(right))
        {
            return -1;
        }
        target = std::make_shared<logical_expression_or>(std::move(left), std::move(right));
    }
    else
    {
        target = std::move(left);
        unlex(i);
    }

    return 0;
}

int cpp_parser::parse_statement(std::shared_ptr<statement> & target)
{
    int i;

    i = lex();
    if (i == T_ASSERT_IS_TRUE)
    {
        i = lex();
        if (i != '(')
        {
            PARSING_ERROR(i, "(");
            return -1;
        }
        std::shared_ptr<logical_expression> t;
        if (parse_logical_expression(t))
        {
            return -1;
        }
        i = lex();
        if (i != ')')
        {
            PARSING_ERROR(i, ")");
            return -1;
        }
        i = lex();
        if (i != ';')
        {
            PARSING_ERROR(i, ";");
            return -1;
        }
        target = std::make_shared<statement_assert_is_true>(std::move(t));
    }
    else if (i == T_ASSERT_IS_FALSE)
    {
        i = lex();
        if (i != '(')
        {
            PARSING_ERROR(i, "(");
            return -1;
        }
        std::shared_ptr<logical_expression> t;
        if (parse_logical_expression(t))
        {
            return -1;
        }
        i = lex();
        if (i != ')')
        {
            PARSING_ERROR(i, ")");
            return -1;
        }
        i = lex();
        if (i != ';')
        {
            PARSING_ERROR(i, ";");
            return -1;
        }
        target = std::make_shared<statement_assert_is_false>(std::move(t));
    }
    else if (i == T_ILLEGAL)
    {
        i = lex();
        if (i != ';')
        {
            PARSING_ERROR(i, ";");
            return -1;
        }
        target = std::make_shared<statement_illegal>();
    }
    else if (i == T_ADD_PAYOFF)
    {
        i = lex();
        if (i != '(')
        {
            PARSING_ERROR(i, "(");
            return -1;
        }
        i = lex();
        if (i != T_FLOAT_LITERAL)
        {
            PARSING_ERROR(i, "<float>");
            return -1;
        }
        float p = yylval.value_float;
        i = lex();
        if (i != ')')
        {
            PARSING_ERROR(i, ")");
            return -1;
        }
        i = lex();
        if (i != ';')
        {
            PARSING_ERROR(i, ";");
            return -1;
        }
        target = std::make_shared<statement_add_payoff>(p);
    }
    else if (i == T_THERE_IS_A_CHANCE)
    {
        i = lex();
        if (i != '(')
        {
            PARSING_ERROR(i, "(");
            return -1;
        }
        i = lex();
        if (i != T_FLOAT_LITERAL)
        {
            PARSING_ERROR(i, "<float>");
            return -1;
        }
        float probability = yylval.value_float;
        i = lex();
        if (i != ')')
        {
            PARSING_ERROR(i, ")");
            return -1;
        }
        i = lex();
        if (i != ';')
        {
            PARSING_ERROR(i, ";");
            return -1;
        }
        target = std::make_shared<statement_there_is_a_chance>(probability);
    }
    else if (i == T_IMPOSSIBLE)
    {
        i = lex();
        if (i != ';')
        {
            PARSING_ERROR(i, ";");
            return -1;
        }
        target = std::make_shared<statement_impossible>();
    }
    else
    {
        PARSING_ERROR(i, "<statement>");
        return -1;
    }

    return 0;
}


int cpp_parser::parse_rule()
{
    int i;
    unsigned line_number = lineno();

    i = lex();
    if (i != T_IF)
    {
        PARSING_ERROR(i, "if");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    std::shared_ptr<logical_expression> condition;
    if (parse_logical_expression(condition))
    {
        return -1;
    }
    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != '{')
    {
        PARSING_ERROR(i, "{");
        return -1;
    }
    std::shared_ptr<statement> s;
    if (parse_statement(s))
    {
        return -1;
    }
    i = lex();
    if (i != '}')
    {
        PARSING_ERROR(i, "}");
        return -1;
    }

    my_agent.add_rule(std::make_shared<rule>(std::move(condition), std::move(s), line_number, next_rule_comment));

    next_rule_comment = "";

    return 0;
}

int cpp_parser::parse_loop()
{
    int i;

    i = lex();
    if (i != T_LOOP)
    {
        PARSING_ERROR(i, "loop");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    i = lex();
    if (i != T_INT_LITERAL)
    {
        PARSING_ERROR(i, "<integer>");
        return -1;
    }

    my_agent.add_command(std::make_shared<command_loop>(yylval.value_int));

    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}


int cpp_parser::parse_generate_cases()
{
    int i;

    i = lex();
    if (i != T_GENERATE_CASES)
    {
        PARSING_ERROR(i, "generate_cases");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    i = lex();
    if (i != T_INT_LITERAL)
    {
        PARSING_ERROR(i, "<integer>");
        return -1;
    }
    
    int max_amount_of_unusual_cases = yylval.value_int;


    i = lex();
    if (i != ',')
    {
        PARSING_ERROR(i, ",");
        return -1;
    }    

    i = lex();
    if (i != T_INT_LITERAL)
    {
        PARSING_ERROR(i, "<integer>");
        return -1;
    }
    
    my_agent.add_command(std::make_shared<command_generate_cases>(max_amount_of_unusual_cases, yylval.value_int));
    
    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}

int cpp_parser::parse_open_databank()
{
    int i;
    i = lex();
    if (i != T_OPEN_DATABANK)
    {
        PARSING_ERROR(i, "open_databank");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    i = lex();
    if (i != T_STRING_LITERAL)
    {
        PARSING_ERROR(i, "<string>");
        return -1;
    }
    
    std::string databank_location = std::string(yylval.value_string);
    free(yylval.value_string);

    my_agent.add_command(std::make_shared<command_open_databank>(databank_location));    
    
    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    
    return 0;
}


int cpp_parser::parse_save_databank()
{
    int i;
    i = lex();
    if (i != T_SAVE_DATABANK)
    {
        PARSING_ERROR(i, "save_databank");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    i = lex();
    if (i != T_STRING_LITERAL)
    {
        PARSING_ERROR(i, "<string>");
        return -1;
    }

    
    std::string databank_location = std::string(yylval.value_string);
    free(yylval.value_string);

    i = lex();
    if (i != ',')
    {
        PARSING_ERROR(i, ",");
        return -1;
    }
    i = lex();
    if (i != T_INT_LITERAL)
    {
        PARSING_ERROR(i, "<int>");
        return -1;
    }
    
    my_agent.add_command(std::make_shared<command_save_databank>(databank_location, yylval.value_int));    
    
    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}


int cpp_parser::parse_validate_cases()
{
    int i;
    i = lex();
    if (i != T_VALIDATE_CASES)
    {
        PARSING_ERROR(i, "validate_cases");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    my_agent.add_command(std::make_shared<command_validate_cases>());
    
    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}


int cpp_parser::parse_estimate_cases()
{
    int i;
    i = lex();
    if (i != T_ESTIMATE_CASES)
    {
        PARSING_ERROR(i, "estimate_cases");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    i = lex();
    if (i != T_INT_LITERAL)
    {
        PARSING_ERROR(i, "<integer>");
        return -1;
    }

    my_agent.add_command(std::make_shared<command_estimate_cases>(yylval.value_int));
    
    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}




int cpp_parser::parse_expand()
{
    int i;

    i = lex();
    if (i != T_EXPAND)
    {
        PARSING_ERROR(i, "expand");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }
    i = lex();
    if (i != T_INT_LITERAL)
    {
        PARSING_ERROR(i, "<integer>");
        return -1;
    }

    my_agent.add_command(std::make_shared<command_expand>(yylval.value_int));

    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}


int cpp_parser::parse_report()
{
    int i;

    i = lex();
    if (i != T_REPORT)
    {
        PARSING_ERROR(i, "report");
        return -1;
    }
    i = lex();
    if (i != '(')
    {
        PARSING_ERROR(i, "(");
        return -1;
    }

    my_agent.add_command(std::make_shared<command_report>());

    i = lex();
    if (i != ')')
    {
        PARSING_ERROR(i, ")");
        return -1;
    }
    i = lex();
    if (i != ';')
    {
        PARSING_ERROR(i, ";");
        return -1;
    }
    return 0;
}

int cpp_parser::parse()
{
    int i;
    do
    {
        i = lex();
        //std::cout << "got " << get_token_name(i) << "\n";

        if (i == 0)
            break;

        switch (i)
        {
        case T_TYPE:
            unlex(i);
            if (parse_type())
                return -1;
            break;
        case T_INPUT:
        case T_OUTPUT:
        case T_HIDDEN:
            unlex(i);
            if (parse_variable())
                return -1;
            break;

        case T_IF:
            unlex(i);
            if (parse_rule())
                return -1;
            break;
        case T_EXPAND:
            unlex(i);
            if (parse_expand())
                return -1;
            break;
        case T_GENERATE_CASES:
            unlex(i);
            if (parse_generate_cases())
                return -1;
            break;
        case T_VALIDATE_CASES:
            unlex(i);
            if (parse_validate_cases())
                return -1;
            break;
        case T_ESTIMATE_CASES:
            unlex(i);
            if (parse_estimate_cases())
                return -1;
            break;
        case T_SAVE_DATABANK:
            unlex(i);
            if (parse_save_databank())
                return -1;
            break;
        case T_OPEN_DATABANK:
            unlex(i);
            if (parse_open_databank())
                return -1;
            break;
        case T_LOOP:
            unlex(i);
            if (parse_loop())
                return -1;
            break;
        case T_REPORT:
            unlex(i);
            if (parse_report())
                return -1;
            break;

        case T_STRING_LITERAL:
            next_rule_comment = std::string(yylval.value_string);
            free(yylval.value_string);
            i = lex();
            if (i!=';')
            {
                PARSING_ERROR(i, ";");
                return -1;
            }
            break;

        default:
            PARSING_ERROR(i, "type or input or output or hidden or if");
            return -1;
        }
    }
    while (true);

    return 0;
}
