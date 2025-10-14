// LoopInfoExample.cpp
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/ScalarEvolution.h"

using namespace llvm;

namespace {
struct LoopInfoExample : public PassInfoMixin<LoopInfoExample> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << "Analyzing function: " << F.getName() << "\n";

    // Get LoopInfo for this function
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    for (Loop *L : LI) {
      errs() << "Found a loop with header: ";
      L->getHeader()->printAsOperand(errs(), false);
      errs() << "\n";
      errs() << "depth: " << L->getLoopDepth() << "\n";     //additional info
      errs() << "blocks: " << L->getBlocks().size()
             << ", backedges: " << L->getNumBackEdges()
             << ", outermost: " << (L->isOutermost() ? "yes" : "no")
             << ", innermost: " << (L->isInnermost() ? "yes" : "no")
             << "\n";
      // PRINTING ---> preheader and latch 
      errs() << "preheader: ";
      if (auto *PH = L->getLoopPreheader()) PH->printAsOperand(errs(), false); else errs() << "(none)";
      errs() << "\n";
      errs() << "latch: ";
      if (auto *LT = L->getLoopLatch())     LT->printAsOperand(errs(), false); else errs() << "(none)";
      errs() << "\n";
      
      SmallVector<BasicBlock*, 4> Exiting, Exit;
      L->getExitingBlocks(Exiting);
      L->getExitBlocks(Exit);

      errs() << "exiting blocks:";
      if (Exiting.empty()) errs() << " (none)";
      for (auto *B : Exiting) { errs() << " "; B->printAsOperand(errs(), false); }
      errs() << "\n";

      errs() << "exit blocks:   ";
      if (Exit.empty()) errs() << " (none)";
      for (auto *B : Exit) { errs() << " "; B->printAsOperand(errs(), false); }
      errs() << "\n";

      // Print subloops
      // Recursively call getSubLoops() to print the loops in a loop next along with their depths.    
      for (Loop *SubL : L->getSubLoops()) {
        errs() << "Subloop with header: ";
        SubL->getHeader()->printAsOperand(errs(), false);
        errs() << "\n";

        // PRINTING ---> preheader and latch 
        errs() << "preheader: ";
        if (auto *SPH = SubL->getLoopPreheader()) SPH->printAsOperand(errs(), false); else errs() << "(none)";
        errs() << "\n";

        errs() << "latch: ";
        if (auto *SLT = SubL->getLoopLatch())     SLT->printAsOperand(errs(), false); else errs() << "(none)";
        errs() << "\n";
      }
    }

    return PreservedAnalyses::all();
  }
};
} 

// Recursive pass for task B 
namespace {
struct RecursiveGetSubLoop : public PassInfoMixin<RecursiveGetSubLoop> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << "Analyzing function (recursive): " << F.getName() << "\n";

    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

    // Minimal recursive printer kept inside run()
    std::function<void(Loop*)> printLoop = [&](Loop *L) {
      unsigned depth = L->getLoopDepth();
      for (unsigned i = 0; i < depth; ++i) errs() << "  ";

      errs() << "Loop (depth " << depth << ") header: ";
      L->getHeader()->printAsOperand(errs(), false);
      errs() << "\n";

      for (unsigned i = 0; i < depth; ++i) errs() << "  ";
      errs() << "preheader: ";
      if (auto *PH = L->getLoopPreheader()) PH->printAsOperand(errs(), false);
      else errs() << "(none)";
      errs() << "\n";

      for (unsigned i = 0; i < depth; ++i) errs() << "  ";
      errs() << "latch:     ";
      if (auto *LT = L->getLoopLatch()) LT->printAsOperand(errs(), false);
      else errs() << "(none)";
      errs() << "\n";

      for (Loop *SubL : L->getSubLoops())
        printLoop(SubL); // recurse
    };

    for (Loop *L : LI)
      printLoop(L);

    return PreservedAnalyses::all();
  }
};
} 

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
              if (Name == "recursive_getSubLoop") {
                FPM.addPass(RecursiveGetSubLoop());
                return true;
              }
              return false;
            });
      }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLoopInfoExamplePluginInfo();
}

