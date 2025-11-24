#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ADT/DenseMap.h"

#include <string>

using namespace llvm;

struct MemorySSADemoPass : PassInfoMixin<MemorySSADemoPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    // Get MemorySSA for this function
    auto &MSSAResult = AM.getResult<MemorySSAAnalysis>(F);
    auto &MSSA = MSSAResult.getMSSA();

    errs() << "Analyzing function: " << F.getName() << "\n";

    // === 1) Open a DOT file for this function ===
    std::string FileName = "mssa_" + F.getName().str() + ".dot";
    std::error_code EC;
    raw_fd_ostream DotOS(FileName, EC, sys::fs::OF_Text);

    if (EC) {
      errs() << "Error opening " << FileName << ": " << EC.message() << "\n";
      return PreservedAnalyses::all();
    }

    DotOS << "digraph \"MSSA_" << F.getName() << "\" {\n";
    DotOS << "  node [shape=box,fontname=\"Courier\"];\n";

    // === 2) Infrastructure: give every MemoryAccess a node id and label ===
    DenseMap<MemoryAccess *, unsigned> IDMap;

    auto getID = [&](MemoryAccess *MA) -> unsigned {
      auto It = IDMap.find(MA);
      if (It != IDMap.end())
        return It->second;
      unsigned NewID = IDMap.size();
      IDMap[MA] = NewID;
      return NewID;
    };

    auto emitNode = [&](MemoryAccess *MA) {
      if (!MA)
        return;
      unsigned Id = getID(MA);

      std::string LabelStr;
      raw_string_ostream RSO(LabelStr);
      MA->print(RSO); // use existing MemorySSA printer
      RSO.flush();

      DotOS << "  n" << Id << " [label=\"";
      for (char c : LabelStr) {
        if (c == '\"')
          DotOS << '\\';
        if (c == '\n')
          DotOS << "\\l";
        else
          DotOS << c;
      }
      DotOS << "\"];\n";
    };

    // === 3) Walk basic blocks and build both text + graph view ===
    for (auto &BB : F) {
      errs() << "BasicBlock: " << BB.getName() << "\n";

      // ----- Block-entry MemoryPhi (one per BB at most) -----
      if (auto *Phi = MSSA.getMemoryAccess(&BB)) {
        if (auto *MPhi = dyn_cast<MemoryPhi>(Phi)) {
          errs() << "  MemoryPhi for block " << BB.getName() << ":\n";

          emitNode(MPhi);

          for (unsigned i = 0; i < MPhi->getNumIncomingValues(); ++i) {
            auto *IncomingAcc = MPhi->getIncomingValue(i);
            auto *Pred = MPhi->getIncomingBlock(i);

            errs() << "    from " << Pred->getName() << ": ";
            IncomingAcc->print(errs());
            errs() << "\n";

            emitNode(IncomingAcc);
            DotOS << "  n" << getID(IncomingAcc)
                  << " -> n" << getID(MPhi) << ";\n";
          }
        }
      }

      // ----- Instruction-level MemoryDefs / MemoryUses -----
      for (auto &I : BB) {
        if (auto *MA = MSSA.getMemoryAccess(&I)) {
          errs() << "  ";
          MA->print(errs());
          errs() << "\n";

          emitNode(MA);

          // Edge from defining access (clobber chain) to this MemoryAccess.
          if (auto *MD = dyn_cast<MemoryDef>(MA)) {
            MemoryAccess *Def = MD->getDefiningAccess();
            if (Def) {
              emitNode(Def);
              DotOS << "  n" << getID(Def)
                    << " -> n" << getID(MD) << ";\n";
            }
          } else if (auto *MU = dyn_cast<MemoryUse>(MA)) {
            MemoryAccess *Def = MU->getDefiningAccess();
            if (Def) {
              emitNode(Def);
              DotOS << "  n" << getID(Def)
                    << " -> n" << getID(MU) << ";\n";
            }
          }
        }
      }
    }

    // === 4) Finish DOT graph ===
    DotOS << "}\n";

    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MemorySSADemoPass", "v0.9",
          [](PassBuilder &PB) {
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([] { return MemorySSAAnalysis(); });
                });

            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "memssa-demo") {
                    FPM.addPass(MemorySSADemoPass());
                    return true;
                  }
                  return false;
                });
          }};
}
