# Task1: Simple LICM; inside the LICM directory

Starter code→ SimpleLICM.cpp. Implement the worklist algorithm to determine if an instruction is loop invariant. Focus → register-to-register computations
- do not hoist instructions that read or write memory, and 
- do not hoist phi instructions. 
- do not use the LLVM isLoopInvariant() function. 
- Add the passes in llvm-tutor/lib

Task 2: Induction variable elimination (IVE):
Inside the Induction folder. Two example codes were provied by the instructor; `AffineRecurrence.cpp` and `DerivedInductionVar.cpp` that implement affine-recurrence and derived-iv analysis passes, respectively. These passes use the LLVM `ScalarEvolution` class. 
- Extend DerivedInductionVar.cpp to identify induction variables in the inner loops of loop nests.
- Extend the pass into a transformation pass called that actually eliminates the induction variables.

Implement and test in the llvm-tutor infrastructure so that the instructor can use that infrastructure to build and test your pass. 
