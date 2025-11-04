#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/CFG.h"

#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"

#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

using namespace llvm;

struct SimpleLICM : public PassInfoMixin<SimpleLICM> {
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR,
                        LPMUpdater &) {
    DominatorTree &DT = AR.DT;

    BasicBlock *Preheader = L.getLoopPreheader();
    if (!Preheader) {
      errs() << "No preheader, skipping loop\n";
      return PreservedAnalyses::all();
    }

    SmallPtrSet<Instruction *, 8> InvariantSet;
    bool Changed = true;

    // Worklist algorithm to identify loop-invariant instructions
    auto isCandidate = [&](Instruction *I) {
      if (isa<PHINode>(I))
        return false;
      if (I->isTerminator())
        return false;
      // skipping memory readers/writers and additional stuff
      if (I->mayReadOrWriteMemory() || I->mayHaveSideEffects())
        return false;
      // must be inside the loop
      if (!L.contains(I))
        return false;
      return true;
    };

    // check whether the operands already invariant/defined outside
    auto operandsInvariant = [&](Instruction *I) {
      for (Value *Op : I->operands()) {
        if (isa<Constant>(Op))
          continue;

        if (Instruction *OpI = dyn_cast<Instruction>(Op)) {
          // if producer is outside loop continue
          if (!L.contains(OpI))
            continue;
          // else producer = invariant
          if (!InvariantSet.count(OpI))
            return false;
        } else {
          // ffunction arg -> outside loop -> fine
          continue;
        }
      }
      return true;
    };

    // loop until no new invariants found
    while (Changed) {
      Changed = false;
      for (BasicBlock *BB : L.blocks()) {
        if (BB == Preheader)
          continue;

        for (Instruction &Inst : *BB) {
          Instruction *I = &Inst;

          if (InvariantSet.count(I))
            continue;
          if (!isCandidate(I))
            continue;
          if (!operandsInvariant(I))
            continue;

          // I as loop-invariant
          InvariantSet.insert(I);
          Changed = true;
        }
      }
    }
    // Actually hoist the instructions we found
    for (Instruction *I : InvariantSet) {
      if (isSafeToSpeculativelyExecute(I) && dominatesAllLoopExits(I, &L, DT)) {
        errs() << "Hoisting:\t" << *I << "\n";
        I->moveBefore(Preheader->getTerminator());
      }
    }

    return PreservedAnalyses::none();
  }

  bool dominatesAllLoopExits(Instruction *I, Loop *L, DominatorTree &DT) {
    SmallVector<BasicBlock *, 8> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    for (BasicBlock *EB : ExitBlocks) {
      if (!DT.dominates(I, EB))
        return false;
    }
    return true;
  }
};

llvm::PassPluginLibraryInfo getSimpleLICMPluginInfo() {
  errs() << "SimpleLICM plugin: getSimpleLICMPluginInfo() called\n";
  return {LLVM_PLUGIN_API_VERSION, "simple-licm", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, LoopPassManager &LPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "simple-licm") {
                    LPM.addPass(SimpleLICM());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  errs() << "SimpleLICM plugin: llvmGetPassPluginInfo() called\n";
  return getSimpleLICMPluginInfo();
}
