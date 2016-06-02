#include "Parser.h"
#include "CodeGen.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
  const Expr *expr = Parser::Parse();
  if (!expr) return 1;

  cout << "Input File:" << endl << expr << endl;

  if (expr->Type() == Type::ERROR) {
    cerr << "ERROR: Program is not well-typed." << endl;
    delete expr;
    return 1;
  }

  CodeGen::Generate(expr)->dump();
  cout << "Done! LLVM assembly written to standard error." << endl;

  delete expr;
  return 0;
}
