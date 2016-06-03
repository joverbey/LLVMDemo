/**
 * @file Parser.h
 * @brief Provides an interface to the lex/yacc-generated lexer/parser.
 */
#ifndef PARSER_H_
#define PARSER_H_

#include "AST.h"

namespace Parser {

/// Parses input provided on standard input, returning an abstract syntax tree.
const Expr *Parse();

}

#endif // PARSER_H_
