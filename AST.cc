#include "AST.h"
#include "Printer.h"
#include <cassert>

ostream &operator<<(ostream &out, const Expr *node) {
  Printer::Print(node, out);
  return out;
}

const class Type &IntegerLiteral::Type() const {
  return Type::INTEGER;
}

void IntegerLiteral::Accept(Visitor *v) const {
  v->Visit(this);
}

const class Type &RealLiteral::Type() const {
  return Type::REAL;
}

void RealLiteral::Accept(Visitor *v) const {
  v->Visit(this);
}

const class Type &InputExpr::Type() const {
  return Type::INTEGER;
}

void InputExpr::Accept(Visitor *v) const {
  v->Visit(this);
}

const class Type &CastExpr::Type() const {
  return this->type;
}

void CastExpr::Accept(Visitor *v) const {
  v->Visit(this);
}

const class Type &BinExpr::Type() const {
  if (this->lhs->Type() != this->rhs->Type()
      || this->lhs->Type().IsError()
      || this->rhs->Type().IsError()) {
    return Type::ERROR;
  }

  switch (this->op) {
    case ADD:
    case SUB:
    case MUL:
    case DIV:
      return this->lhs->Type();
    case LT:
    case LTE:
    case GT:
    case GTE:
    case EQ:
    case NEQ:
      return Type::INTEGER;
  }

  assert(false);
  return Type::ERROR;
}

void BinExpr::Accept(Visitor *v) const {
  v->Visit(this);
}

const class Type &IfExpr::Type() const {
  if (this->condExpr->Type() == Type::INTEGER
      && this->thenExpr->Type() == this->elseExpr->Type())
    return this->thenExpr->Type();

  return Type::ERROR;
}

void IfExpr::Accept(Visitor *v) const {
  v->Visit(this);
}
