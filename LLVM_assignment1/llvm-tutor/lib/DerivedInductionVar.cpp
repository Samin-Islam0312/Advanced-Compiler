/* DerivedInductionVar.cpp 
 *
 * This pass detects derived induction variables using ScalarEvolution
 * and then eliminates them by rewriting their uses.
 *
 * Compatible with New Pass Manager
*/

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

class DerivedInductionVar
    : public PassInfoMixin<DerivedInductionVar> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    auto &LI = AM.getResult<LoopAnalysis>(F);
    auto &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

    bool Changed = false;

    Module *M = F.getParent();
    const DataLayout &DL = M->getDataLayout();

    // materializes SCEV expressions as IR
    SCEVExpander Exp(SE, DL, "derivediv");

    for (Loop *L : LI) {
      errs() << "Analyzing loop in function " << F.getName() << ":\n";

      BasicBlock *Header = L->getHeader();
      if (!Header)
          continue;

   
      for (PHINode &PN : Header->phis()) {
          if (!PN.getType()->isIntegerTy())
            continue;
          const SCEV *S = SE.getSCEV(&PN);

          // Detect affine AddRec expressions
          if (auto *AR = dyn_cast<SCEVAddRecExpr>(S)) {
            const SCEV *Step = AR->getStepRecurrence(SE);
            const SCEV *Start = AR->getStart();

            // Check if it's affine
            if (AR->isAffine()) {
              errs() << "  Derived induction variable: " << PN.getName()
                     << " = {" << *Start << ",+," << *Step << "}<"
                     << L->getHeader()->getName() << ">\n";

              //eeliminate this induction variable, rewriting uses of the PHI using its SCEV (AR) and SCEVExpander
              SmallVector<User *, 8> Users(PN.user_begin(), PN.user_end());
              for (User *U : Users) {
                if (Instruction *UserI = dyn_cast<Instruction>(U)) {
                  // Expand the SCEV at the use site while replacing the use
                  Value *NewV =
                      Exp.expandCodeFor(AR, PN.getType(), UserI);
                  UserI->replaceUsesOfWith(&PN, NewV);
                  Changed = true;
                }
              }
            }
          }
      }

      //Analyze inner loops in this loop nest 
      for (Loop *InnerL : L->getSubLoops()) {
        errs() << "Analyzing inner loop in function " << F.getName() << ":\n";

        BasicBlock *InnerHeader = InnerL->getHeader();
        if (!InnerHeader)
          continue;

        for (PHINode &PN : InnerHeader->phis()) {
          if (!PN.getType()->isIntegerTy())
            continue;
          const SCEV *S = SE.getSCEV(&PN);

          if (auto *AR = dyn_cast<SCEVAddRecExpr>(S)) {
            const SCEV *Step = AR->getStepRecurrence(SE);
            const SCEV *Start = AR->getStart();

            if (AR->isAffine()) {
              errs() << "  Derived induction variable (inner): " << PN.getName()
                     << " = {" << *Start << ",+," << *Step << "}<"
                     << InnerL->getHeader()->getName() << ">\n";

              // Same transformation for inner-loop IVs eliminate by rewriting uses via ScalarEvolution.
              SmallVector<User *, 8> Users(PN.user_begin(), PN.user_end());
              for (User *U : Users) {
                if (Instruction *UserI = dyn_cast<Instruction>(U)) {
                  Value *NewV =
                      Exp.expandCodeFor(AR, PN.getType(), UserI);
                  UserI->replaceUsesOfWith(&PN, NewV);
                  Changed = true;
                }
              }
            }
          }
        }
      }
    }

    if (Changed)
      return PreservedAnalyses::none();
    return PreservedAnalyses::all();
  }
};

} // namespace

// Register the pass
llvm::PassPluginLibraryInfo getDerivedInductionVarPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DerivedInductionVar", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "derived-iv") {
                    FPM.addPass(DerivedInductionVar());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getDerivedInductionVarPluginInfo();
}
