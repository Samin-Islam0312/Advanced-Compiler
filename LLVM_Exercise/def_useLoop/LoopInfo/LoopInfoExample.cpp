// LoopInfoExample.cpp
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct LoopInfoExample : public PassInfoMixin<LoopInfoExample> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << "Analyzing function: " << F.getName() << "\n";

    // Get LoopInfo for this function
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    for (Loop *L : LI) {
      errs() << "  Found a loop with header: ";
      L->getHeader()->printAsOperand(errs(), false);
      errs() << "\n";

      // Print subloops
      // Recursively call getSubLoops() to print the loops in a loop next along with their depths.    
      for (Loop *SubL : L->getSubLoops()) {
        errs() << "    Subloop with header: ";
        SubL->getHeader()->printAsOperand(errs(), false);
        errs() << "\n";
      }
    }

    return PreservedAnalyses::all();
  }
};
} // namespace

// Register pass plugin
llvm::PassPluginLibraryInfo getLoopInfoExamplePluginInfo() {
  return {
      LLVM_PLUGIN_API_VERSION, "loop-info-example", LLVM_VERSION_STRING,
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "loop-info-example") {
                FPM.addPass(LoopInfoExample());
                return true;
              }
              return false;
            });
      }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLoopInfoExamplePluginInfo();
}

