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


const cases_parser::token_and_name cases_parser::array_of_tokens_and_names[] = {
    { T_EQUAL_GREATER, "T_EQUAL_GREATER" },
    { T_IDENTIFIER, "T_IDENTIFIER" },
    { T_FLOAT_LITERAL, "T_FLOAT_LITERAL" },
    { T_INT_LITERAL, "T_INT_LITERAL" },
    { T_STRING_LITERAL, "T_STRING_LITERAL" },
    { 0, "EOF" }
};

const std::string cases_parser::get_token_name(int t)
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

int cases_parser::parse()
{
    int i;
    do
    {
        i = lex();
        if (i == 0)
            return 0;
        
        if (i != '[')
        {
            PARSING_ERROR(i, "[");
            return -1;
        }
        i = lex();
        if (i != T_INT_LITERAL)
        {
            PARSING_ERROR(i, "<int>");
            return -1;
        }
        i = lex();
        if (i != ']')
        {
            PARSING_ERROR(i, "]");
            return -1;
        }        

        i = lex();
        if (i != '[')
        {
            PARSING_ERROR(i, "[");
            return -1;
        }
        i = lex();
        if (i != T_INT_LITERAL)
        {
            PARSING_ERROR(i, "<int>");
            return -1;
        }
        i = lex();
        if (i != ']')
        {
            PARSING_ERROR(i, "]");
            return -1;
        }
        
        i = lex();
        if (i != '{')
        {
            PARSING_ERROR(i, "{");
            return -1;
        }
        
        i = lex();
        if (i == '}')
        {
            continue;
        }
        do
        {
            if (i != T_IDENTIFIER)
            {
                PARSING_ERROR(i, "<identifier>");
                return -1;
            }
            std::string variable_instance_name(yylval.value_string);
            free(yylval.value_string);
            i = lex();
            if (i != T_EQUAL_GREATER)
            {
                PARSING_ERROR(i, "=>");
                return -1;
            }
            i = lex();
            if (i != T_IDENTIFIER)
            {
                PARSING_ERROR(i, "<identifier>");
                return -1;
            }
            std::string value_instance_name(yylval.value_string);
            free(yylval.value_string);
            
            std::cout << variable_instance_name << "=>" << value_instance_name << "\n";
                                
            i = lex();
            if (i == ',')
            {
                i = lex();
                continue;
            }
            else
            if (i == '}')
            {
                break;
            }
        }
        while (true);
    }
    while (i);
    
    return 0;
}

int cases_parser::lineno() {
    return triglavget_lineno(scanner);
}

int cases_parser::lex()
{
    int i = triglav_lex(&yylval, scanner); //  std::cout << "token " << get_token_name(i) << "\n";
    return i;
}
