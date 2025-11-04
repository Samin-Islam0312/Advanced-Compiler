# Induction Variable
This pass uses ScalarEvolution to identify induction variables (as SCEVAddRecExpr) in both outer and inner loops. For each detected induction PHI, it attempts to eliminate it by rewriting its uses using `SCEVExpander::expandCodeFor`. In practice, for canonical primary IV PHIs, the expander often reuses the existing PHI value, so the IR does not change textually, even though the pass conceptually rewrites the uses.

The extended pass is insider the derived folder, and the affine folder is the pass that has been provied by the instructor as a helper pass to do the assignmetn.

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

## Test cases 
Inside the tests directory there are two test cases, testcase1..c is a simple single loop test. testcase2.c is a nested-loop test case. I canonicalized them into `testcase1_canonical.ll` and `testcase2_canonical.ll` and kept insde the tests directory and used them as the input. 

### Canonicalize test cases 
```bash
clang -O0 -emit-llvm -S ./tests/testcase1.c -o ./tests/testcase1.ll
clang -O0 -emit-llvm -S ./tests/testcase2.c -o ./test/testcase2.ll
```

```bash 
opt -passes='mem2reg,loop-simplify,lcssa' -S ./tests/testcase1.ll -o ./tests/testcase1_canonical.ll
opt -passes='mem2reg,loop-simplify,lcssa' -S ./tests/testcase2.ll -o ./tests/testcase2_canonical.ll
```

## Run pass 
To run the pass on the canonicalized IR files, I ran on previous canonicalized IR file inside `tests` directory.

```bash
opt -load-pass-plugin ./build/DerivedInductionVar.dylib -passes=derived-iv -S \
    -o output/case1.ll ./tests/testcase1_canonical.ll
```

```bash
opt -load-pass-plugin ./build/DerivedInductionVar.dylib -passes=derived-iv -S \
    -o output/case2.ll ./tests/testcase2_canonical.ll
```


