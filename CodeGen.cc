#include "CodeGen.h"
#include <cassert>
#include <iostream>
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Verifier.h"
using namespace std;
using namespace llvm;

namespace CodeGen {

class CodeGen: public Visitor {
 public:
  explicit CodeGen(const Expr *expr);
  virtual ~CodeGen();
  virtual void Visit(const IntegerLiteral *node);
  virtual void Visit(const RealLiteral *node);
  virtual void Visit(const InputExpr *node);
  virtual void Visit(const CastExpr *node);
  virtual void Visit(const BinExpr *node);
  virtual void Visit(const IfExpr *node);
  llvm::Module *GetModule();

 private:
  llvm::Type *translateType(const ::Type &type);
  llvm::Value *translateBinOpReal(llvm::Value *lhs, BinOp op, llvm::Value *rhs);
  llvm::Value *translateBinOpInteger(llvm::Value *lhs, BinOp op, llvm::Value *rhs);
  void finalizeComputeFunction();
  llvm::Function *createMain();

  llvm::IRBuilder<> builder;
  llvm::Module *module; /// The LLVM module containing the translation
  llvm::Function *compute;  /// The LLVM function containing the translation -- called compute()
  llvm::Function *inputInteger; /// The LLVM declaration of the input_integer() runtime library function
  const Expr *root; /// Root of the expression tree being translated
  map<const Expr *, llvm::Value *> code; /// Maps an expression to its corresponding Value in the LLVM translation
};

CodeGen::CodeGen(const Expr *expr)
    : builder(llvm::getGlobalContext()) {
  module = new Module("translated", getGlobalContext());

  FunctionType *funcType = TypeBuilder<int(), false>::get(getGlobalContext());
  inputInteger = cast<Function>(module->getOrInsertFunction("input_integer", funcType));

  FunctionType *functionType = FunctionType::get(translateType(expr->Type()), false);
  compute = Function::Create(functionType, Function::ExternalLinkage, "compute", module);
  BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "entry", compute);
  builder.SetInsertPoint(bb);

  root = expr;
}

CodeGen::~CodeGen() {
}

llvm::Type *CodeGen::translateType(const ::Type &type) {
  if (type == ::Type::INTEGER) {
    return llvm::Type::getInt32Ty(getGlobalContext());
  } else if (type == ::Type::REAL) {
    return llvm::Type::getDoubleTy(getGlobalContext());
  } else {
    assert(false);
  }
}

void CodeGen::Visit(const IntegerLiteral *node) {
  code[node] = ConstantInt::get(getGlobalContext(), APInt(32, node->Value(), true));
}

void CodeGen::Visit(const RealLiteral *node) {
  code[node] = ConstantFP::get(getGlobalContext(), APFloat(node->Value()));
}

void CodeGen::Visit(const InputExpr *node) {
  code[node] = builder.CreateCall(inputInteger);
}

void CodeGen::Visit(const CastExpr *node) {
  node->ExprToCast()->Accept(this);

  const ::Type &origType = node->ExprToCast()->Type();
  const ::Type &newType = node->Type();

  if (origType == newType) {
    code[node] = code[node->ExprToCast()];
  } else if (origType == ::Type::INTEGER && newType == ::Type::REAL) {
    code[node] = builder.CreateSIToFP(code[node->ExprToCast()], llvm::Type::getDoubleTy(getGlobalContext()));
  } else if (origType == ::Type::REAL && newType == ::Type::INTEGER) {
    code[node] = builder.CreateFPToSI(code[node->ExprToCast()], llvm::Type::getInt32Ty(getGlobalContext()));
  } else {
    assert(false);
  }
}

void CodeGen::Visit(const BinExpr *node) {
  node->Lhs()->Accept(this);
  node->Rhs()->Accept(this);

  Value *lhs = code[node->Lhs()];
  Value *rhs = code[node->Rhs()];

  if (node->Lhs()->Type() == ::Type::REAL) {
    code[node] = translateBinOpReal(lhs, node->Op(), rhs);
  } else if (node->Lhs()->Type() == ::Type::INTEGER) {
    code[node] = translateBinOpInteger(lhs, node->Op(), rhs);
  } else {
    assert(false);
  }
}

Value *CodeGen::translateBinOpReal(Value *lhs, BinOp op, Value *rhs) {
  switch (op) {
    case ADD:
      return builder.CreateFAdd(lhs, rhs);
    case SUB:
      return builder.CreateFSub(lhs, rhs);
    case MUL:
      return builder.CreateFMul(lhs, rhs);
    case DIV:
      return builder.CreateFDiv(lhs, rhs);
    case LT:
      return builder.CreateFCmpOLT(lhs, rhs);
    case LTE:
      return builder.CreateFCmpOLE(lhs, rhs);
    case GT:
      return builder.CreateFCmpOGT(lhs, rhs);
    case GTE:
      return builder.CreateFCmpOGE(lhs, rhs);
    case EQ:
      return builder.CreateFCmpOEQ(lhs, rhs);
    case NEQ:
      return builder.CreateFCmpONE(lhs, rhs);
  }
  assert(false);
  return NULL;
}

Value *CodeGen::translateBinOpInteger(Value *lhs, BinOp op, Value *rhs) {
  switch (op) {
    case ADD:
      return builder.CreateAdd(lhs, rhs);
    case SUB:
      return builder.CreateSub(lhs, rhs);
    case MUL:
      return builder.CreateMul(lhs, rhs);
    case DIV:
      return builder.CreateSDiv(lhs, rhs);
    case LT:
      return builder.CreateICmpSLT(lhs, rhs);
    case LTE:
      return builder.CreateICmpSLE(lhs, rhs);
    case GT:
      return builder.CreateICmpSGT(lhs, rhs);
    case GTE:
      return builder.CreateICmpSGE(lhs, rhs);
    case EQ:
      return builder.CreateICmpEQ(lhs, rhs);
    case NEQ:
      return builder.CreateICmpNE(lhs, rhs);
  }
  assert(false);
  return NULL;
}

void CodeGen::Visit(const IfExpr *node) {
  node->CondExpr()->Accept(this);

  BasicBlock *bbTrue = BasicBlock::Create(getGlobalContext(), "then", compute);
  BasicBlock *bbFalse = BasicBlock::Create(getGlobalContext(), "else", compute);
  BasicBlock *bbEnd = BasicBlock::Create(getGlobalContext(), "endif", compute);

  builder.CreateCondBr(code[node->CondExpr()], bbTrue, bbFalse);

  builder.SetInsertPoint(bbTrue);
  node->ThenExpr()->Accept(this);
  bbTrue = builder.GetInsertBlock(); // In case of nested IfExpr
  builder.CreateBr(bbEnd);

  builder.SetInsertPoint(bbFalse);
  node->ElseExpr()->Accept(this);
  bbFalse = builder.GetInsertBlock(); // In case of nested IfExpr
  builder.CreateBr(bbEnd);

  builder.SetInsertPoint(bbEnd);
  PHINode *phi = builder.CreatePHI(translateType(node->Type()), 2);
  phi->addIncoming(code[node->ThenExpr()], bbTrue);
  phi->addIncoming(code[node->ElseExpr()], bbFalse);
  code[node] = phi;
}

llvm::Module *CodeGen::GetModule() {
  if (code.find(root) == code.end()) {
    root->Accept(this);
    finalizeComputeFunction();
    createMain();
  }

  return module;
}

void CodeGen::finalizeComputeFunction() {
  builder.CreateRet(code[root]);
  verifyFunction(*compute);
}

llvm::Function *CodeGen::createMain() {
  Function *output;
  if (compute->getReturnType()->isIntegerTy()) {
    FunctionType *outFuncType = TypeBuilder<void(int), false>::get(getGlobalContext());
    output = cast<Function>(module->getOrInsertFunction("output_integer", outFuncType));
  } else {
    assert(compute->getReturnType()->isDoubleTy());
    FunctionType *outFuncType = TypeBuilder<void(double), false>::get(getGlobalContext());
    output = cast<Function>(module->getOrInsertFunction("output_double", outFuncType));
  }

  FunctionType *funcType = TypeBuilder<int(int, char **), false>::get(getGlobalContext());
  Function *main = cast<Function>(module->getOrInsertFunction("main", funcType));
  BasicBlock *bb = BasicBlock::Create(getGlobalContext(), "entry", main);
  builder.SetInsertPoint(bb);

  CallInst *callCompute = builder.CreateCall(compute);

  builder.CreateCall(output, callCompute);

  builder.CreateRet(ConstantInt::getNullValue(main->getReturnType()));
  verifyFunction(*main);
  return main;
}

llvm::Module *Generate(const Expr *expr) {
  CodeGen cg(expr);
  return cg.GetModule();
}
}
