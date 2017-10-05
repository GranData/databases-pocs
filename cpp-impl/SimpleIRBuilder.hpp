#ifndef SIMPLEDB_SIMPLEIRBUILDER_HPP
#define SIMPLEDB_SIMPLEIRBUILDER_HPP

#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRBuilder.h>

using namespace llvm;

namespace simpledb {

class SimpleIRBuilder : public llvm::IRBuilder<> {
 public:
  explicit SimpleIRBuilder(LLVMContext &C);

  // Create renames
  AllocaInst *Alloca(Type *Ty, Value *ArraySize = nullptr, const Twine &Name = "");
  LoadInst *Load(Value *Ptr, const char *Name);
  LoadInst *Load(Value *Ptr, const Twine &Name = "");
  LoadInst *Load(Value *Ptr, bool isVolatile, const Twine &Name = "");
  StoreInst *Store(Value *Val, Value *Ptr, bool isVolatile = false);
};

}

#endif //SIMPLEDB_SIMPLEIRBUILDER_HPP
