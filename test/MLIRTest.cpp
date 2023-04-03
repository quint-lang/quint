//
// Created by BY210033 on 2023/4/1.
//
#include <iostream>

#include "mlir/ExecutionEngine/ExecutionEngine.h"
#include "mlir/ExecutionEngine/OptUtils.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/Dialect/StandardOps/IR/Ops.h"
#include "llvm/Support/TargetSelect.h"

using namespace mlir;
using namespace llvm;

int main() {
    // Initialize LLVM targets
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    // Create an MLIR module
    MLIRContext context;
    OwningModuleRef module = ModuleOp::create(
            FileLineColLoc::get("example.mlir", 0, 0, &context));

    // Define an MLIR function with a loop
    FuncOp func = FuncOp::create(
            UnknownLoc::get(&context), "sum",
            mlir::FunctionType::get(&context, {MemRefType::get({}, FloatType::getF32(&context))},
                                    {FloatType::getF32(&context)}),
    {});
    OpBuilder builder(&context);
    auto input = func.getArgument(0);
    mlir::Value zero = builder.create<ConstantOp>(UnknownLoc::get(&context),
                                                  FloatAttr::get(builder.getF32Type(), 0.0f));
    mlir::Value sum = builder.create<AllocOp>(UnknownLoc::get(&context),
                                              input.getType().cast<MemRefType>());
    mlir::Value index = builder.create<ConstantOp>(UnknownLoc::get(&context),
                                                   IntegerAttr::get(builder.getIntegerType(32), 0));

    mlir::Value size = builder.create<DimOp>(UnknownLoc::get(&context), input, 0);
    Block *loopBody = func.addBlock();

    std::cout << "Hello World!" << std::endl;
    return 0;
}