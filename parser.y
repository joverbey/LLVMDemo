%{
#include <stdio.h>
#include <stdlib.h>
#include "AST.h"

extern char *yytext;
extern int yylex();

const Expr *parse_result;

void yyerror(const char *message);
%}

%start start

%union {
  int int_value;
  double real_value;
  const std::string *string;
  const Expr *expr;
}

%type <expr> expr

%token <int_value>  T_INTEGER_LITERAL
%token <real_value> T_REAL_LITERAL
%token <string>     T_STRING
%token              T_INPUT T_INTEGER T_REAL
%token              T_IF T_THEN T_ELSE
%token              T_LTE T_GTE T_EQ T_NEQ

%left '<' T_LTE '>' T_GTE T_EQ T_NEQ
%left '+' '-'
%left '*' '/'

%%

start:     expr                               { parse_result = $1; }

expr:      T_INTEGER_LITERAL                  { $$ = new IntegerLiteral($1); }
         | T_REAL_LITERAL                     { $$ = new RealLiteral($1); }
         | T_INPUT '(' ')'                    { $$ = new InputExpr(); }
         | T_INTEGER '(' expr ')'             { $$ = new CastExpr(Type::INTEGER, $3); }
         | T_REAL '(' expr ')'                { $$ = new CastExpr(Type::REAL, $3); }
         | expr '<'   expr                    { $$ = new BinExpr($1, LT, $3); }
         | expr T_LTE expr                    { $$ = new BinExpr($1, LTE, $3); }
         | expr '>'   expr                    { $$ = new BinExpr($1, GT, $3); }
         | expr T_GTE expr                    { $$ = new BinExpr($1, GTE, $3); }
         | expr T_EQ  expr                    { $$ = new BinExpr($1, EQ, $3); }
         | expr T_NEQ expr                    { $$ = new BinExpr($1, NEQ, $3); }
         | expr '+'   expr                    { $$ = new BinExpr($1, ADD, $3); }
         | expr '-'   expr                    { $$ = new BinExpr($1, SUB, $3); }
         | expr '*'   expr                    { $$ = new BinExpr($1, MUL, $3); }
         | expr '/'   expr                    { $$ = new BinExpr($1, DIV, $3); }
         | '(' expr ')'                       { $$ = $2; }
         | T_IF expr T_THEN expr T_ELSE expr  { $$ = new IfExpr($2, $4, $6); }
         //| T_LET name '=' expr T_IN expr  { $$ = new LetExpr($2, $4, $6); }
         ;

%%

void yyerror(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}
