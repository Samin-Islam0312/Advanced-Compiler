#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

struct FMAContractPass : public PassInfoMixin<FMAContractPass> {

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    bool Changed = false;
    
    // Access Target Transform Info for cost modeling
    const auto &TTI = FAM.getResult<TargetIRAnalysis>(F);

    for (auto &BB : F) {
      for (auto I = BB.begin(); I != BB.end(); ) {
        Instruction *Inst = &*I++; 

        // 1. Detection: Support both FAdd and FSub
        unsigned Opcode = Inst->getOpcode();
        if (Opcode != Instruction::FAdd && Opcode != Instruction::FSub)
          continue;

        BinaryOperator *BinOp = cast<BinaryOperator>(Inst);

        // Strict Requirement: User must allow contraction
        if (!BinOp->hasAllowContract()) 
          continue;

        Value *Op0 = BinOp->getOperand(0);
        Value *Op1 = BinOp->getOperand(1);

        Instruction *MulOp = nullptr;
        Value *Addend = nullptr;
        
        // We need to track if we need to negate operands for FSub cases
        bool NegateMul = false;
        bool NegateAddend = false;

        // Logic to find the Multiplier (FMul) among operands
        // Op0 is the FMul
        if (auto *M = dyn_cast<Instruction>(Op0)) {
            if (M->getOpcode() == Instruction::FMul && M->hasAllowContract()) {
                MulOp = M;
                Addend = Op1;
                
                // If this was a SUB: (A*B) - C
                // FMA is (A*B) + (-C)
                if (Opcode == Instruction::FSub) {
                    NegateAddend = true;
                }
            }
        }

        // Op1 is the FMul (Only valid for FAdd usually, unless careful with FSub)
        if (!MulOp) {
            if (auto *M = dyn_cast<Instruction>(Op1)) {
                if (M->getOpcode() == Instruction::FMul && M->hasAllowContract()) {
                    MulOp = M;
                    Addend = Op0;

                    // If this was a SUB: C - (A*B)
                    // FMA is -(A*B) + C  => (-A)*B + C
                    if (Opcode == Instruction::FSub) {
                        NegateMul = true;
                    }
                }
            }
        }

        if (!MulOp) continue;

        // 2. Hardware Cost Check
        Type *RetTy = MulOp->getType();
        SmallVector<Type*, 3> ArgTys = { RetTy, RetTy, RetTy };
        IntrinsicCostAttributes ICA(Intrinsic::fma, RetTy, ArgTys);
        
        // If cost is high (emulated), skip
        if (TTI.getIntrinsicInstrCost(ICA, TTI::TCK_RecipThroughput) > InstructionCost::getMax())
            continue;

        // 3. Compile-Time Constant Folding 
        if (auto *CA = dyn_cast<ConstantFP>(MulOp->getOperand(0))) {
          if (auto *CB = dyn_cast<ConstantFP>(MulOp->getOperand(1))) {
             if (auto *CC = dyn_cast<ConstantFP>(Addend)) {
                 
                 APFloat ValA = CA->getValueAPF();
                 APFloat ValB = CB->getValueAPF();
                 APFloat ValC = CC->getValueAPF();

                 // Handle negations for FSub logic
                 if (NegateMul) ValA.changeSign();
                 if (NegateAddend) ValC.changeSign();

                 // Perform Fused Multiply Add in Software (IEEE 754)
                 ValA.fusedMultiplyAdd(ValB, ValC, APFloat::rmNearestTiesToEven);
                 
                 // Replace instruction with constant
                 Value *ConstRes = ConstantFP::get(F.getContext(), ValA);
                 BinOp->replaceAllUsesWith(ConstRes);
                 
                 // Cleanup
                 BinOp->eraseFromParent();
                 if (MulOp->use_empty()) MulOp->eraseFromParent();
                 
                 Changed = true;
                 continue; 
             }
          }
        }

        // 4. IR Transformation
        IRBuilder<> Builder(BinOp);
        Builder.setFastMathFlags(BinOp->getFastMathFlags());

        Value *A = MulOp->getOperand(0);
        Value *B = MulOp->getOperand(1);
        Value *C = Addend;

        // Apply Negations if needed (insert fneg instructions)
        if (NegateMul) {
            A = Builder.CreateFNeg(A, "fma.neg.mul");
        }
        if (NegateAddend) {
            C = Builder.CreateFNeg(C, "fma.neg.add");
        }

        Function *FmaFn = Intrinsic::getOrInsertDeclaration(F.getParent(), Intrinsic::fma, {BinOp->getType()});
        CallInst *FmaCall = Builder.CreateCall(FmaFn, {A, B, C});
        
        BinOp->replaceAllUsesWith(FmaCall);

        // Always remove the Add/Sub because those are replaced 
        BinOp->eraseFromParent();

        // Only remove the Mul if it has no other users (Multi-Use Support)
        if (MulOp->use_empty()) {
            MulOp->eraseFromParent();
        }

        Changed = true;
      }
    }

    return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FMAContractPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "fma-pass") {
                    FPM.addPass(FMAContractPass());
                    return true;
                  }
                  return false;
                });
          }};
}