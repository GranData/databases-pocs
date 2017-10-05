#ifndef SIMPLEDB_LLVMEXECUTOR_HPP
#define SIMPLEDB_LLVMEXECUTOR_HPP

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/Verifier.h>
#include "SimpleIRBuilder.hpp"

namespace simpledb {

class LLVMExecutor {
 public:
  LLVMExecutor();

  llvm::LLVMContext &getGlobalContext();
  llvm::IRBuilder<> getBuilder();
  SimpleIRBuilder getSimpleIRBuilder();
  std::unique_ptr<llvm::Module> getModule();
  std::unique_ptr<llvm::ExecutionEngine> compileModule(std::unique_ptr<llvm::Module> module,
                                                       bool debug = false);

 private:
  llvm::LLVMContext &globalContext;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<SimpleIRBuilder> customBuilder;
};

}

#endif //SIMPLEDB_LLVMEXECUTOR_HPP