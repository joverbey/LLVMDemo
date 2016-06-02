/**
 * @file Type.h
 * @brief Declaration of the Type class and type constants.
 */
#ifndef TYPE_H_
#define TYPE_H_

#include <iostream> /// TEMP
#include <string>
using namespace std;

/**
 * The type of an expression: integer, real, or ill-typed.
 *
 * Types can be compared using == and !=.
 */
class Type {
 public:
  const static Type INTEGER; /// Integer type
  const static Type REAL;    /// Real (floating-point) type
  const static Type ERROR;   /// Denotes an ill-typed expression

  bool IsError() const {
    return *this == Type::ERROR;
  }

  friend ostream &operator<<(ostream &out, const Type &type) {
    return out << type.name;
  }

  bool operator==(const Type &that) const {
    return this->name == that.name; // Nominal comparison for simplicity
  }

  bool operator!=(const Type &that) const {
    return !(*this == that);
  }

 private:
  explicit Type(const char *n) : name(n) { }
  virtual ~Type() { }

  const char *name;
};

#endif // TYPE_H_
