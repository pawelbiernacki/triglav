%{

struct class_with_scanner
{
	void * scanner;
	void * agent_ptr;
};

#define YYPARSE_PARAM data
#define YYLEX_PARAM ((struct class_with_scanner*)data)->scanner

#define YYERROR_VERBOSE 1


// this is a cool trick to pass the parser environment to the error message function
#define yyerror(MSG) yyexterror(data, MSG)


#include <stdio.h>

%}

%union {
char * value_string;
double value_float;
int value_int;
}

%token T_TYPE T_INPUT T_OUTPUT T_HIDDEN T_VARIABLE T_INITIAL T_TERMINAL T_VALUE T_ILLEGAL T_ASSERT_IS_TRUE T_ASSERT_IS_FALSE T_EQUAL_EQUAL T_IF T_EXPAND T_LOOP
%token T_REPORT T_ADD_PAYOFF T_THERE_IS_A_CHANCE T_USUALLY T_GENERATE_CASES T_EQUAL_GREATER T_SAVE_DATABANK T_OPEN_DATABANK
%token T_AND_AND T_OR_OR T_IMPOSSIBLE T_ESTIMATE_CASES T_CONSIDER T_PRECALCULATE

%token<value_string> T_IDENTIFIER T_STRING_LITERAL
%token<value_float> T_FLOAT_LITERAL
%token<value_int> T_INT_LITERAL

%define api.pure full


%start program


%%

program:

%%

int yymyparse(void * d)
{
	return yyparse(d);
}
