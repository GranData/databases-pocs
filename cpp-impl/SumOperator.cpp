#include <iostream>

#include "SumOperator.hpp"
#include "LLVMExecutor.hpp"

namespace simpledb {

SumOperator::SumOperator() {
}

int64_t SumOperator::run(const int32_t *values, size_t size) {
  // The main idea here is to reproduce this behavior in LLVM.
  //
  // acc = 0
  // for (size_t i = 0; i < size; i++) {
  //   if (values[i] == null)
  //     continue
  //   acc = acc + values[i]
  // }
  // return acc

  LLVMExecutor executor;
  llvm::LLVMContext &globalContext = executor.getGlobalContext();
  std::unique_ptr<llvm::Module> module = executor.getModule();
  llvm::IRBuilder<> builder = executor.getSimpleIRBuilder();


  // Define functions, blocks, etc -----------------------------------------------------------------
  llvm::Constant *initial = llvm::ConstantInt::get(builder.getInt64Ty(), 0);
  llvm::Constant *elementCount = llvm::ConstantInt::get(builder.getInt64Ty(), (uint64_t) size);

  // Set global variables
  llvm::GlobalVariable *rowScanCurrentIndex = new llvm::GlobalVariable(
      *module.get(),
      builder.getInt64Ty(),
      false,
      llvm::GlobalValue::LinkageTypes::ExternalLinkage,
      initial,
      "gvar.row-scan.current-index");

  llvm::GlobalVariable *rowScanEndIndex = new llvm::GlobalVariable(
      *module.get(),
      builder.getInt64Ty(),
      false,
      llvm::GlobalValue::LinkageTypes::ExternalLinkage,
      elementCount,
      "gvar.row-scan.end-index");


  /* --- ( Main Function Definition ) ------------------------------------- */
  // function: Int64 Sum()
  llvm::FunctionType *funcType = llvm::FunctionType::get(builder.getInt64Ty(), false);
  llvm::Function *SumOperatorFunc = llvm::Function::Create(funcType,
                                                           llvm::Function::ExternalLinkage,
                                                           "Sum",
                                                           module.get());

  // Blocks definition
  llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(globalContext,
                                                          "entrypoint.block",
                                                          SumOperatorFunc);

  llvm::BasicBlock *nextRowBlock = llvm::BasicBlock::Create(globalContext,
                                                            "nextrow.block",
                                                            SumOperatorFunc);

  llvm::BasicBlock *returnBlock = llvm::BasicBlock::Create(globalContext,
                                                           "return.block",
                                                           SumOperatorFunc);

  llvm::BasicBlock *updateIterationBlock = llvm::BasicBlock::Create(globalContext,
                                                                    "update-iteration.block",
                                                                    SumOperatorFunc);

  llvm::BasicBlock *aggregationBlock = llvm::BasicBlock::Create(globalContext,
                                                                "aggregation.block",
                                                                SumOperatorFunc);

  llvm::BasicBlock *validateCurrentIndexBlock = llvm::BasicBlock::Create(
      globalContext, "validate-current-index.block", SumOperatorFunc);


  /* --- ( EntryBlock ) --------------------------------------------------- */
  builder.SetInsertPoint(entryBlock);
  // Create an `accumulator` variable an set it to 0 (accumulator = 0)
  llvm::Value *accumulator = builder.CreateAlloca(builder.getInt64Ty(), nullptr, "var.accumulator");
  builder.CreateStore(builder.getInt64(0), accumulator);
  builder.CreateBr(nextRowBlock);


  /* --- ( UpdateIterationBlock ) ----------------------------------------- */
  builder.SetInsertPoint(updateIterationBlock);
  // Increment current row index
  llvm::Value *rowCurrentIndex = builder.CreateLoad(rowScanCurrentIndex, "var.row.prev-index");
  llvm::Value *nextIndex = builder.CreateAdd(rowCurrentIndex, builder.getInt64(1));
  builder.CreateStore(nextIndex, rowScanCurrentIndex);
  builder.CreateBr(nextRowBlock);


  /* --- ( NextRowBlock ) ------------------------------------------------- */
  builder.SetInsertPoint(nextRowBlock);

  llvm::Value *currentIndex = builder.CreateLoad(rowScanCurrentIndex, "var.row.current-index");

  // Convert `values` pointer to Int, so we can convert such value to a IR pointer.
  llvm::ConstantInt *const pointerAsInt = llvm::ConstantInt::get(builder.getInt64Ty(),
                                                                 (uint64_t) values);
  llvm::PointerType *const pointerToInt32Type = llvm::PointerType::get(builder.getInt32Ty(), 0);
  llvm::Value *intPointer = builder.CreateIntToPtr(pointerAsInt, pointerToInt32Type,
                                                   "var.pointer-to-int-address");

  // Get values from the array in the idx position.
  llvm::Value *valuePtr = builder.CreateGEP(intPointer, currentIndex, "var.index.value-pointer");
  llvm::Value *obtainedValue = builder.CreateLoad(valuePtr, "var.array-at-index.value");

  // filter of the value is null, which is represented by the lowest int32 value.
  int int32NullValue = std::numeric_limits<int32_t>::lowest();
  llvm::Constant *nullValue = llvm::ConstantInt::get(builder.getInt32Ty(), int32NullValue, true);
  llvm::Value *nullPredicate = builder.CreateICmpEQ(obtainedValue, nullValue, "cmp.is-null-check");
  builder.CreateCondBr(nullPredicate, validateCurrentIndexBlock, aggregationBlock);


  /* --- ( AggregationBlock ) ----------------------------------------------------- */
  builder.SetInsertPoint(aggregationBlock);
  // Add the current value of the array at the given index with the accumulator value.
  llvm::Value *casted = builder.CreateIntCast(obtainedValue, builder.getInt64Ty(), true);
  llvm::Value *acc = builder.CreateLoad(accumulator, "var.accumulator.read-value");
  llvm::Value *Add = builder.CreateAdd(acc, casted, "op.sum.accumulator-array-at-index");
  // Move the calculated value to the stack variables `accumulator`
  builder.CreateStore(Add, accumulator);
  builder.CreateBr(validateCurrentIndexBlock);

  /* --- ( ValidateCurrentIndexBlock ) ------------------------------------ */
  builder.SetInsertPoint(validateCurrentIndexBlock);
  // Check if we reach the end of the array
  llvm::Value *length = builder.CreateLoad(rowScanEndIndex, "var.array-length");
  llvm::Value *predicate = builder.CreateICmpSGE(currentIndex, length, "cmp.is-last-element");
  builder.CreateCondBr(predicate, returnBlock, updateIterationBlock);

  /* --- ( ReturnBlock ) -------------------------------------------------- */
  builder.SetInsertPoint(returnBlock);
  // Load the value from the `accumulator` to a CPU register and return such value
  llvm::Value *val = builder.CreateLoad(accumulator, "var.accumulator.read-value");
  builder.CreateRet(val);

  /* --- ( Prepare LLVM code to be executed ) ----------------------------- */
  // First alternative to call a generated llvm Function
  std::unique_ptr<llvm::ExecutionEngine> engine = executor.compileModule(std::move(module), true);
  llvm::GenericValue ret = engine->runFunction(SumOperatorFunc, std::vector<llvm::GenericValue>());

  return (int64_t) ret.IntVal.getSExtValue();
}

}