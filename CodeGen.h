/**
 * @file CodeGen.h
 * @brief Declaration of an LLVM code generator for the sample language.
 */
#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <iostream>
#include <map>
#include "AST.h"

using namespace std;

/// Generates an LLVM Module with a main method that evaluates the given
/// expression and then calls a runtime library function (output_integer
/// or output_double) to display its value.
namespace CodeGen {
  llvm::Module *Generate(const Expr *expr);
}

#endif // CODEGEN_H_
