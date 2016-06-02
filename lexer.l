%{
#include "AST.h"
#include "parser.yy.h"
#include <stdlib.h>
%}

%%

[ \t\r\n]*         { /* Ignore */ }

[0-9]+             { yylval.int_value = atoi(yytext); return T_INTEGER_LITERAL; }

[0-9]+\.[0-9]+     { yylval.real_value = atof(yytext); return T_REAL_LITERAL; }

"input"            { return T_INPUT; }
"integer"          { return T_INTEGER; }
"real"             { return T_REAL; }
"if"               { return T_IF; }
"then"             { return T_THEN; }
"else"             { return T_ELSE; }

\"[^\"\r\n]*\"     { yylval.string = new std::string(yytext+1, yyleng-2); return T_STRING; }

.                  { return yytext[0]; }

%%

int yywrap()
{
  return 1;
}
