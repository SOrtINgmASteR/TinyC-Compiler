%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

FILE *g_token_out = NULL;

static void emit_token(const char *type, const char *lexeme) {
    FILE *out = g_token_out ? g_token_out : stdout;
    fprintf(out, "%-12s %s\n", type, lexeme);
}
%}

%option noyywrap
%option yylineno
%option noinput
%option nounput
%option nodefault

%x COMMENT

DIGIT       [0-9]
ID_START    [A-Za-z_]
ID_CONT     [A-Za-z0-9_]
INT_LIT     {DIGIT}+
FLOAT_LIT   ({DIGIT}+\.{DIGIT}+|{DIGIT}+\.|\.{DIGIT}+)([eE][+-]?{DIGIT}+)?
STRING_LIT  \"([^\\\n]|(\\.))*\"
CHAR_LIT    \'([^\\\n]|(\\.))\'

%%

"/*"                { BEGIN(COMMENT); }
<COMMENT>"*/"        { BEGIN(INITIAL); }
<COMMENT>\n          { /* keep line count */ }
<COMMENT>.           { /* consume comment */ }

"//".*              { /* ignore single-line comment */ }

[ \t\r\n]+          { /* ignore whitespace */ }

"print"             { emit_token("KEYWORD", yytext); return PRINT; }
"if"                { emit_token("KEYWORD", yytext); return IF; }
"else"              { emit_token("KEYWORD", yytext); return ELSE; }
"while"             { emit_token("KEYWORD", yytext); return WHILE; }
"return"            { emit_token("KEYWORD", yytext); return RETURN; }
"int"               { emit_token("KEYWORD", yytext); return INT; }
"float"             { emit_token("KEYWORD", yytext); return FLOAT; }
"char"              { emit_token("KEYWORD", yytext); return CHAR; }
"void"              { emit_token("KEYWORD", yytext); return VOID; }

"auto"              { emit_token("KEYWORD", yytext); return KEYWORD; }
"break"             { emit_token("KEYWORD", yytext); return KEYWORD; }
"case"              { emit_token("KEYWORD", yytext); return KEYWORD; }
"const"             { emit_token("KEYWORD", yytext); return KEYWORD; }
"continue"          { emit_token("KEYWORD", yytext); return KEYWORD; }
"default"           { emit_token("KEYWORD", yytext); return KEYWORD; }
"do"                { emit_token("KEYWORD", yytext); return KEYWORD; }
"double"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"enum"              { emit_token("KEYWORD", yytext); return KEYWORD; }
"extern"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"for"               { emit_token("KEYWORD", yytext); return KEYWORD; }
"goto"              { emit_token("KEYWORD", yytext); return KEYWORD; }
"long"              { emit_token("KEYWORD", yytext); return KEYWORD; }
"register"          { emit_token("KEYWORD", yytext); return KEYWORD; }
"short"             { emit_token("KEYWORD", yytext); return KEYWORD; }
"signed"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"sizeof"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"static"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"struct"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"switch"            { emit_token("KEYWORD", yytext); return KEYWORD; }
"typedef"           { emit_token("KEYWORD", yytext); return KEYWORD; }
"union"             { emit_token("KEYWORD", yytext); return KEYWORD; }
"unsigned"          { emit_token("KEYWORD", yytext); return KEYWORD; }
"volatile"          { emit_token("KEYWORD", yytext); return KEYWORD; }

{FLOAT_LIT}          { emit_token("FLOAT", yytext); yylval.fval = atof(yytext); return FLOAT_LIT; }
{INT_LIT}            { emit_token("INTEGER", yytext); yylval.ival = atoi(yytext); return INT_LIT; }
{STRING_LIT}         { emit_token("STRING", yytext); yylval.sval = strdup(yytext); return STRING_LIT; }
{CHAR_LIT}           { emit_token("CHAR", yytext); yylval.sval = strdup(yytext); return CHAR_LIT; }

{ID_START}{ID_CONT}* { emit_token("IDENTIFIER", yytext); yylval.sval = strdup(yytext); return IDENT; }

"=="                { emit_token("OPERATOR", yytext); return EQ; }
"!="                { emit_token("OPERATOR", yytext); return NE; }
"<="                { emit_token("OPERATOR", yytext); return LE; }
">="                { emit_token("OPERATOR", yytext); return GE; }
"&&"                { emit_token("OPERATOR", yytext); return AND; }
"||"                { emit_token("OPERATOR", yytext); return OR; }
"+"                 { emit_token("OPERATOR", yytext); return '+'; }
"-"                 { emit_token("OPERATOR", yytext); return '-'; }
"*"                 { emit_token("OPERATOR", yytext); return '*'; }
"/"                 { emit_token("OPERATOR", yytext); return '/'; }
"%"                 { emit_token("OPERATOR", yytext); return '%'; }
"="                 { emit_token("OPERATOR", yytext); return '='; }
"<"                 { emit_token("OPERATOR", yytext); return '<'; }
">"                 { emit_token("OPERATOR", yytext); return '>'; }
"!"                 { emit_token("OPERATOR", yytext); return '!'; }

";"                 { emit_token("SEPARATOR", yytext); return ';'; }
","                 { emit_token("SEPARATOR", yytext); return ','; }
"("                 { emit_token("SEPARATOR", yytext); return '('; }
")"                 { emit_token("SEPARATOR", yytext); return ')'; }
"{"                 { emit_token("SEPARATOR", yytext); return '{'; }
"}"                 { emit_token("SEPARATOR", yytext); return '}'; }
"["                 { emit_token("SEPARATOR", yytext); return '['; }
"]"                 { emit_token("SEPARATOR", yytext); return ']'; }

.                   { emit_token("UNKNOWN", yytext); return UNKNOWN; }

%%
