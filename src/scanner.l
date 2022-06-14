%{

#define YYPARSE_PARAM
 
#include "parser.tab.h"
#define YY_DECL int triglav_lex( YYSTYPE * yylval_param, void * yyscanner )
#define YYLTYPE int

struct class_with_scanner
{
	void * scanner;
	void * agent;
};

%}

%option noyywrap 
%option reentrant
%option bison-bridge
%option yylineno
%option stack
%option prefix="triglav"


%x COMMENT


DIGIT    [0-9]
ID       [a-z][a-z0-9]*
BLANK           [ \t]
ANYTHING_BUT_EOL [^\n]
LETTER          [a-zA-Z_]

LETTER_OR_DIGIT ({DIGIT}|{LETTER})
IDENTIFIER		{LETTER}({LETTER_OR_DIGIT})*

%%


"#"                 yy_push_state( COMMENT, yyscanner);
     
<COMMENT>\n          yy_pop_state( yyscanner );
<COMMENT>[^\n]+      

";"						return ';';
","						return ',';
":"						return ':';
"{"						return '{';
"}"						return '}';
"("						return '(';
")"						return ')';
"."						return '.';
"-"                     return '-';
"?"						return '?';
"!"						return '!';
"["                     return '[';
"]"                     return ']';
"=="                    return T_EQUAL_EQUAL;
"=>"                    return T_EQUAL_GREATER;
"&&"                    return T_AND_AND;
"||"                    return T_OR_OR;
"="						return '=';
     
"if"                    { return T_IF; }
"type"				    { return T_TYPE; }
"input"                 { return T_INPUT; }
"output"                { return T_OUTPUT; }
"hidden"                { return T_HIDDEN; }
"variable"              { return T_VARIABLE; }
"initial"               { return T_INITIAL; }
"terminal"              { return T_TERMINAL; }
"value"                 { return T_VALUE; }
"illegal"               { return T_ILLEGAL; }
"expand"                { return T_EXPAND; }
"loop"                  { return T_LOOP; }
"report"                { return T_REPORT; }
"assert_is_true"        { return T_ASSERT_IS_TRUE; }
"assert_is_false"       { return T_ASSERT_IS_FALSE; }
"add_payoff"            { return T_ADD_PAYOFF; }
"there_is_a_chance"     { return T_THERE_IS_A_CHANCE; }
"usually"               { return T_USUALLY; }
"generate_cases"        { return T_GENERATE_CASES; }
"save_databank"         { return T_SAVE_DATABANK; }
"open_databank"         { return T_OPEN_DATABANK; }
"impossible"            { return T_IMPOSSIBLE; }
"estimate_cases"        { return T_ESTIMATE_CASES; }
"consider"              { return T_CONSIDER; }
"precalculate"          { return T_PRECALCULATE; }

{IDENTIFIER}  			{ yylval_param->value_string = strdup(yytext); return T_IDENTIFIER; }

-?{DIGIT}+\.{DIGIT}+e(-|\+){DIGIT}+	{ yylval_param->value_float = atof(yytext); return T_FLOAT_LITERAL; }

-?{DIGIT}+\.{DIGIT}+	{ yylval_param->value_float = atof(yytext); return T_FLOAT_LITERAL; }

-?{DIGIT}+				{ yylval_param->value_int = atoi(yytext); return T_INT_LITERAL; }

\"([^\\\"]|\\.)*\"		{ 

                        const char * p = yytext+1;                        
                        yylval_param->value_string = malloc(strlen(yytext+1)+1); 
                        char * t = yylval_param->value_string;
                        while (*p != '\0')
                        {
                            if (*p == '\"')
                            {
                                *t = '\0';
                                break;
                            }
                            else
                            {
                                *t++ = *p++;
                            }
                        }
                        
                        return T_STRING_LITERAL; 
                        }

.|\n

%%


void triglav_open_file(struct yyguts_t* yyg, const char * f)
{
	yyin = fopen( f, "r" );
	if (yyin ==  NULL)
	{
		fprintf(stderr, "unable to open file %s\n", f);
		exit(EXIT_FAILURE);
	}
}

void triglav_close_file(struct yyguts_t* yyg)
{
	fclose(yyin);
}


void yyexterror (struct class_with_scanner * data, const char * m)
{
	struct yyguts_t * yyg = data->scanner;
	printf("line %u: at %s, syntax error %s\n", yylineno, yytext, m);
	//printf("syntax error %s\n", m);
}
