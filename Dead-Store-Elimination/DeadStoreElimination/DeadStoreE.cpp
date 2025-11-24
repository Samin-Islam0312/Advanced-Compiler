#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/SmallVector.h"

using namespace llvm;

struct MemorySSADSEPass : PassInfoMixin<MemorySSADSEPass> {
  static bool hasInterveningUse(StoreInst *Prev, StoreInst *Cur) {
    BasicBlock *BB = Prev->getParent();
    if (BB != Cur->getParent())
      return true; // different blocks => assume there is a use(being conservative)

    Value *Ptr = Prev->getPointerOperand();

    for (Instruction *I = Prev->getNextNode(); I && I != Cur;
         I = I->getNextNode()) {
      if (auto *LI = dyn_cast<LoadInst>(I)) {
        if (LI->getPointerOperand() == Ptr)
          return true; // found a read of the same location
      }

      // can be added more like - volatile, calls that may access memory
    }
    return false;
  }

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    if (F.isDeclaration())
      return PreservedAnalyses::all();

    // Get MemorySSA and prepare an updater so we can keep it consistent.
    auto &MSSAResult = AM.getResult<MemorySSAAnalysis>(F);
    MemorySSA &MSSA = MSSAResult.getMSSA();
    MemorySSAUpdater Updater(&MSSA);

    errs() << "Running MemorySSADSEPass on function: " << F.getName() << "\n";

    SmallVector<StoreInst *, 16> DeadStores;

    // walking basic blocks and instructions backwards.
    for (auto &BB : llvm::reverse(F)) {
    for (auto &I : llvm::reverse(BB)) {

      if (auto *MA = MSSA.getMemoryAccess(&I)) {
        auto *MD = dyn_cast<MemoryDef>(MA);
        if (!MD)
          continue;

        // Current store
        Instruction *Inst = MD->getMemoryInst();
        if (!Inst)
          continue;

        auto *SI = dyn_cast_if_present<StoreInst>(Inst);
        if (!SI)
          continue; // not a store → skip

        // Previous clobber
        MemoryAccess *PrevMA =
            MSSA.getWalker()->getClobberingMemoryAccess(MD);

        auto *PrevMD = dyn_cast<MemoryDef>(PrevMA);
        if (!PrevMD)
          continue;

        Instruction *PrevInst = PrevMD->getMemoryInst();
        if (!PrevInst)
          continue;

        auto *PrevSI = dyn_cast_if_present<StoreInst>(PrevInst);
        if (!PrevSI)
          continue;

        // MustAlias(Inst, PrevInst) → same pointer operand.
        Value *Ptr     = SI->getPointerOperand();
        Value *PrevPtr = PrevSI->getPointerOperand();
        if (Ptr != PrevPtr)
          continue;

        // Same basic block? If not, skip
        if (PrevSI->getParent() != SI->getParent())
          continue;

        // no intervening uses of that pointer
        if (hasInterveningUse(PrevSI, SI))
          continue;

        errs() << "  DSE: removing dead store: ";
        PrevSI->print(errs());
        errs() << "\n";

        DeadStores.push_back(PrevSI);
      }
    }
  }

    //removing the dead stores and update MemorySSA.
    for (StoreInst *SI : DeadStores) {
      if (auto *MA = MSSA.getMemoryAccess(SI)) {
        Updater.removeMemoryAccess(MA);
      }
      SI->eraseFromParent();
    }
    return DeadStores.empty() ? PreservedAnalyses::all()
                              : PreservedAnalyses::none();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MemorySSADSEPass", "v0.1",
          [](PassBuilder &PB) {
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([] { return MemorySSAAnalysis(); });
                });

            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "memssa-dse") {
                    FPM.addPass(MemorySSADSEPass());
                    return true;
                  }
                  return false;
                });
          }};
}
