# Loop Invariant Code Motion
As per the requirement of the assignment, I have used the instructor's skeleton LLVM pass file for Simple Loop Invariant Code Motion (LICM). The pass identifies and hoists loop-invariant instructions out of loops when it is safe to do so. The implementation uses a worklist-style algorithm to iteratively detect invariant instructions based on their operands. Only register-to-register computations are considered — memory operations, PHI nodes, and instructions with side effects are ignored. Each detected invariant instruction is then safely moved to the loop preheader if it can be speculatively executed and dominates all loop exits. The instructions to run the pass successfully is given below/ 

## Export
```bash
export LLVM_PREFIX="$(brew --prefix llvm)"
export PATH="$LLVM_PREFIX/bin:$PATH"
export CC="$LLVM_PREFIX/bin/clang"
export CXX="$LLVM_PREFIX/bin/clang++"
```

## Build 
```bash
mkdir build && cd build
```
```bash
cmake -DCMAKE_C_COMPILER="$CC" \          
      -DCMAKE_CXX_COMPILER="$CXX" \ 
      -DLLVM_DIR="$LLVM_PREFIX/lib/cmake/llvm" \
      -DCMAKE_BUILD_TYPE=Release ..  
```
```bash
make
```

## Run pass 
To run the pass on the canonicalized IR files, I ran on previous canonicalized IR file inside `LLVM_Exercise/Task1/matmul/` directory

```bash
opt -load-pass-plugin ./build/SimpleLICM.dylib \
    -passes=simple-licm \
    -S \
    -o output/matmul_licm.ll \
    ../../LLVM_Exercise/Task1/matmul/matmul_canonical.ll
```

## Output
The transformed IR are put inside output directory `output/matmul_licm.ll`
