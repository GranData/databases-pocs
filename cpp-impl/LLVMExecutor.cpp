#include <ostream>
#include <iostream>
#include "LLVMExecutor.hpp"

namespace simpledb {

LLVMExecutor::LLVMExecutor()
    : globalContext(llvm::getGlobalContext()) {
  // Initialize LLVM ---------------------------------------------------------
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  module = std::unique_ptr<llvm::Module>(new llvm::Module("MainModule", globalContext));
  builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(globalContext));
  customBuilder = std::unique_ptr<SimpleIRBuilder>(new SimpleIRBuilder(globalContext));
}

llvm::IRBuilder<> LLVMExecutor::getBuilder() {
  return *builder;
}

SimpleIRBuilder LLVMExecutor::getSimpleIRBuilder() {
  return *customBuilder;
}

llvm::LLVMContext &LLVMExecutor::getGlobalContext() {
  return globalContext;
}

std::unique_ptr<llvm::Module> LLVMExecutor::getModule() {
  return std::move(module);
}

std::unique_ptr<llvm::ExecutionEngine> LLVMExecutor::compileModule(
    std::unique_ptr<llvm::Module> module, bool debug) {
  // Prepare LLVM code to be executed ----------------------------------------
  std::string error_str;
  llvm::raw_string_ostream verifyError(error_str);
  if (llvm::verifyModule(*module, &verifyError))
    throw std::runtime_error(std::string("Invalid module: \n") + verifyError.str());

  if (debug) {
    // module->dump();

    std::error_code error_code;
    llvm::raw_fd_ostream file("llvm-fnblock.ll", error_code, llvm::sys::fs::F_RW);
    if (!error_code) {
      module->print(file, nullptr);
    }
  }

  // Execute the program
  std::unique_ptr<llvm::ExecutionEngine> engine = std::unique_ptr<llvm::ExecutionEngine>(
      llvm::EngineBuilder(std::move(module))
          .setErrorStr(&error_str)
          .create());
  engine->finalizeObject();

  return std::move(engine);
}

}