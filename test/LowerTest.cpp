//#include <iostream>
//#include "mlir/IR/Builders.h"
//#include "mlir/IR/MLIRContext.h"
//#include "mlir/IR/Module.h"
//#include "mlir/ExecutionEngine/OptUtils.h"
//#include "mlir/ExecutionEngine/JitRunner.h"
//#include "llvm/IR/Verifier.h"
//
//using namespace mlir;
//
//int main() {
//    // 创建 MLIR 上下文
//    MLIRContext context;
//
//    // 创建一个函数，并将其添加到新的 MLIR 模块中
//    OpBuilder builder(&context);
//    ModuleOp module = ModuleOp::create(builder.getUnknownLoc());
//    FuncOp func = FuncOp::create(builder.getUnknownLoc(), "hello", builder.getFunctionType({}, {}));
//    module.push_back(func);
//
//    // 在函数中插入返回语句
//    builder.setInsertionPointToEnd(&func.getBody().front());
//    builder.create<ReturnOp>(builder.getUnknownLoc());
//
//    // 将 MLIR 降低为 LLVM IR
//    llvm::LLVMContext llvmContext;
//    std::unique_ptr<llvm::Module> llvmModule = mlir::translateModuleToLLVMIR(module, llvmContext);
//    if (!llvm::verifyModule(*llvmModule, &llvm::errs())) {
//        llvm::errs() << "LLVM IR verification failed\n";
//        return -1;
//    }
//
//    // 使用 MLIR 的 JIT 来执行函数
//    auto jit = mlir::JitRunner(llvmModule.get(), &llvmContext);
//    auto entryFn = jit->getSymbolAddress<llvm::JITTargetAddress>("hello");
//    entryFn();
//
//    return 0;
//}
