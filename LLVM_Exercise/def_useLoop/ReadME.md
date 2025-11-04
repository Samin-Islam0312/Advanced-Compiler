# How to build the project 
Find the llvm-tu
# Pass 1: DefUseChain Pass
Emitting IR 
```bash 
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm ../../inputs/matmul.c -o inputIR/matmul.ll
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm ../../inputs/max.c -o inputIR/max.ll
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm ../../inputs/gcd.c -o inputIR/gcd.ll
```
## Building the pass 
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

## Run the pass 
```bash 
opt -load-pass-plugin build/DefUseChains.dylib -passes=def-use-chains inputIR/matmul.ll -disable-output
opt -load-pass-plugin build/DefUseChains.dylib -passes=def-use-chains inputIR/max.ll -disable-output
opt -load-pass-plugin build/DefUseChains.dylib -passes=def-use-chains inputIR/gcd.ll -disable-output
```

 ```bash 
 opt -load-pass-plugin build/DefUseChains.dylib -passes=def-use-chains inputIR/matmul.ll -disable-output 2>&1 | tee output/defuse_matmul.log
 opt -load-pass-plugin build/DefUseChains.dylib -passes=def-use-chains inputIR/max.ll -disable-output 2>&1 | tee output/defuse_max.log
 opt -load-pass-plugin build/DefUseChains.dylib -passes=def-use-chains inputIR/gcd.ll -disable-output 2>&1 | tee output/defuse_gcd.log
```

# Pass 2: Loop Info

## Canonicalizing inputs 

### `matmul.c`
```bash
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" ../defUse/inputIR/matmul.ll -o canonical/matmul_canonical.ll
```

### `max.c`
```bash
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" ../defUse/inputIR/max.ll -o canonical/max_canonical.ll
```

### `gcd.c`
```bash
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" ../defUse/inputIR/gcd.ll -o canonical/gcd_canonical.ll
```

## Building `LoopInfoExample` pass 
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

## Run the pass 
```bash 
opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/matmul_canonical.ll -disable-output
opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/max_canonical.ll -disable-output
opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/gcd_canonical.ll -disable-output
```

 ```bash 
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/matmul_canonical.ll -disable-output 2>&1 | tee output/loopinfo_matmul.log
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/max_canonical.ll -disable-output 2>&1 | tee output/loopinfo_max.log
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/gcd_canonical.ll -disable-output 2>&1 | tee output/loopinfo_gcd.log
```



## A. Printing preheader and latch, both outter and inner loops 
 ```bash 
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/matmul_canonical.ll -disable-output 2>&1 | tee output/preheader_latch/loopinfo_matmul.log
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/max_canonical.ll -disable-output 2>&1 | tee output/preheader_latch/loopinfo_max.log
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/gcd_canonical.ll -disable-output 2>&1 | tee output/preheader_latch/loopinfo_gcd.log
```



## A. Printing out the loop information

```bash  
-passes='print<loops>'
opt input.ll -passes=’function<my-passig>'

-passes-'-loop-simplify'
fixirreducible
-lcssa
SCEV
loop-rotate


```

## B. Implementation for the recursively call of `getSubLoops()`
```bash 
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" \
    canonical/matmul_canonical.ll -disable-output 2>&1 | tee output/recursivePass/loopinfo_matmul_recursive.log
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" \
    canonical/max_canonical.ll -disable-output 2>&1 | tee output/recursivePass/loopinfo_max_recursive.log
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" \
    canonical/gcd_canonical.ll -disable-output 2>&1 | tee output/recursivePass/loopinfo_gcd_recursive.log
```

## C. More loop information

## C. More Loop Information

I printed the additional information below in the **iterative loop info pass**, not in the recursive one:

- **depth** — `L->getLoopDepth()`: nesting depth (1 = outermost).  
- **blocks** — `L->getBlocks().size()`: number of basic blocks in the loop.  
- **backedges** — `L->getNumBackEdges()`: count of edges that branch back to the header.  
- **outermost / innermost** — `L->isOutermost()`, `L->isInnermost()`: whether the loop is at the top or has no subloops.  
- **exiting blocks** — `L->getExitingBlocks(...)`: blocks inside the loop from which control can exit the loop.  
- **exit blocks** — `L->getExitBlocks(...)`: blocks outside the loop that are targets of exits.  

### Commands to run the pass after building and compiling

```bash 
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" \
    canonical/matmul_canonical.ll -disable-output 2>&1 | tee output/more_info/loopinfo_matmul_moreInfo.log
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" \
    canonical/max_canonical.ll -disable-output 2>&1 | tee output/more_info/loopinfo_max_moreInfo.log
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" \
    canonical/gcd_canonical.ll -disable-output 2>&1 | tee output/more_info/loopinfo_gcd_moreInfo.log
```