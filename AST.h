/**
 * @file AST.h
 * @brief Declaration of classes defining an abstract syntax tree for the sample language.
 */
#ifndef AST_H_
#define AST_H_

#include "Type.h"
#include <iostream>
using namespace std;

class Visitor;

/// A node in an abstract syntax tree.
class Expr {
 public:
  Expr() { }
  virtual ~Expr() { }
  virtual const class Type &Type() const = 0;
  virtual void Accept(Visitor *v) const = 0;
  friend ostream &operator<<(ostream &out, const Expr *node);
};

/// An integer literal, like 0 or 5.
class IntegerLiteral: public Expr {
 public:
  explicit IntegerLiteral(int val) : value(val) { }
  int Value() const { return value; }
  virtual const class Type &Type() const;
  virtual void Accept(Visitor *v) const;
 private:
  const int value;
};

/// A real (double precision floating-point) literal, like 3.5 or 1.0.
class RealLiteral: public Expr {
 public:
  explicit RealLiteral(double val) : value(val) { }
  double Value() const { return value; }
  virtual const class Type &Type() const;
  virtual void Accept(Visitor *v) const;
 private:
  const double value;
};

/// The input() expression, which prompts the user to enter an integer value.
class InputExpr: public Expr {
 public:
  InputExpr() { }
  ~InputExpr() { }
  virtual const class Type &Type() const;
  virtual void Accept(Visitor *v) const;
 private:
};

/// An integer(x) or real(n) expression, which converts a value from one type to another.
class CastExpr: public Expr {
 public:
  explicit CastExpr(const class Type &t, const Expr *e)
      : type(t), expr(e) { }
  const Expr *ExprToCast() const { return expr; }
  virtual const class Type &Type() const;
  virtual void Accept(Visitor *v) const;
 private:
  const class Type &type;
  const Expr *expr;
};

/// Binary operators: + - * / < <= > >= = !=
typedef enum {
  ADD, SUB, MUL, DIV,
  LT, LTE, GT, GTE, EQ, NEQ,
} BinOp;

/// A binary expression, like 3+4 or 2<5
class BinExpr: public Expr {
 public:
  explicit BinExpr(const Expr *l, BinOp o, const Expr *r)
      : lhs(l), op(o), rhs(r) { }
  const Expr *Lhs() const { return lhs; }
  BinOp Op() const { return op; }
  const Expr *Rhs() const { return rhs; }
  virtual const class Type &Type() const;
  virtual void Accept(Visitor *v) const;
 private:
  const Expr *lhs;
  const BinOp op;
  const Expr *rhs;
};

/// An if-else expression: if c then t else e
class IfExpr: public Expr {
 public:
  explicit IfExpr(const Expr *c, const Expr *t, const Expr *e)
      : condExpr(c), thenExpr(t), elseExpr(e) { }
  const Expr *CondExpr() const { return condExpr; }
  const Expr *ThenExpr() const { return thenExpr; }
  const Expr *ElseExpr() const { return elseExpr; }
  virtual const class Type &Type() const;
  virtual void Accept(Visitor *v) const;
 private:
  const Expr *condExpr;
  const Expr *thenExpr;
  const Expr *elseExpr;
};

/**
 * A Visitor, which can be used to traverse an abstract syntax tree.
 *
 * The Visit methods are responsible for traversing child nodes.
 */
class Visitor {
 public:
  virtual void Visit(const IntegerLiteral *node) = 0;
  virtual void Visit(const RealLiteral *node) = 0;
  virtual void Visit(const InputExpr *node) = 0;
  virtual void Visit(const CastExpr *node) = 0;
  virtual void Visit(const BinExpr *node) = 0;
  virtual void Visit(const IfExpr *node) = 0;
};

#endif // AST_H_
