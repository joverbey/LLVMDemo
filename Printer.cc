#include <iostream>
#include <iomanip>
#include "Printer.h"
using namespace std;

namespace Printer {

class PrintVisitor: public Visitor {
 public:
  PrintVisitor(ostream &o) : out(o) { }
  virtual void Visit(const IntegerLiteral *node);
  virtual void Visit(const RealLiteral *node);
  virtual void Visit(const InputExpr *node);
  virtual void Visit(const CastExpr *node);
  virtual void Visit(const BinExpr *node);
  virtual void Visit(const IfExpr *node);

 private:
  ostream &out;
};

void Print(const Expr *node, ostream &out) {
  PrintVisitor v(out);
  node->Accept(&v);
}

void PrintVisitor::Visit(const IntegerLiteral *node) {
  out << node->Value();
}

void PrintVisitor::Visit(const RealLiteral *node) {
  out << fixed << node->Value();
}

void PrintVisitor::Visit(const InputExpr *node) {
  out << "input()";
}

void PrintVisitor::Visit(const CastExpr *node) {
  out << node->Type() << "(";
  node->ExprToCast()->Accept(this);
  out << ")";
}

void PrintVisitor::Visit(const BinExpr *node) {
  out << '(';
  node->Lhs()->Accept(this);
  out << ' ';
  switch (node->Op()) {
    case ADD:
      out << '+';
      break;
    case SUB:
      out << '-';
      break;
    case MUL:
      out << '*';
      break;
    case DIV:
      out << '/';
      break;
    case LT:
      out << '<';
      break;
    case LTE:
      out << "<=";
      break;
    case GT:
      out << '>';
      break;
    case GTE:
      out << ">=";
      break;
    case EQ:
      out << "==";
      break;
    case NEQ:
      out << "!=";
      break;
  }
  out << ' ';
  node->Rhs()->Accept(this);
  out << ')';
}

void PrintVisitor::Visit(const IfExpr *node) {
  out << "if ";
  node->CondExpr()->Accept(this);
  out << " then ";
  node->ThenExpr()->Accept(this);
  out << " else ";
  node->ElseExpr()->Accept(this);
}

}
