#include "AST.h"
#include "Parser.h"

extern const Expr *parse_result;
extern const Expr *yyparse();

namespace Parser {

const Expr *Parse() {
  if (yyparse()) {
    return NULL;
  }
  return parse_result;
}

}
