/**
 * @file Parser.h
 * @brief Provides an interface to the lex/yacc-generated lexer/parser.
 */
#ifndef PARSER_H_
#define PARSER_H_

#include "AST.h"
#include <iostream>
using namespace std;

extern const Expr *parse_result;
extern const Expr *yyparse();

namespace Parser {

/// Parses input provided on standard input, returning an abstract syntax tree.
const Expr *Parse() {
  if (yyparse()) {
    return NULL;
  }

  return parse_result;
}

}

#endif // PARSER_H_
