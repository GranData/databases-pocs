#include "SimpleIRBuilder.hpp"

using namespace llvm;

namespace simpledb {

SimpleIRBuilder::SimpleIRBuilder(LLVMContext &C) : IRBuilder<>(C) {}

AllocaInst *SimpleIRBuilder::Alloca(Type *Ty, Value *ArraySize, const Twine &Name) {
  return this->CreateAlloca(Ty, ArraySize, Name);
}

LoadInst *SimpleIRBuilder::Load(Value *Ptr, const char *Name) {
  return this->CreateLoad(Ptr, Name);
}

LoadInst *SimpleIRBuilder::Load(Value *Ptr, const Twine &Name) {
  return this->CreateLoad(Ptr, Name);
}

LoadInst *SimpleIRBuilder::Load(Value *Ptr, bool isVolatile, const Twine &Name) {
  return this->CreateLoad(Ptr, isVolatile, Name);
}

StoreInst *SimpleIRBuilder::Store(Value *Val, Value *Ptr, bool isVolatile) {
  return this->CreateStore(Val, Ptr, isVolatile);
}

}