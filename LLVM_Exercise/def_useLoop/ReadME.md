# Inputs
The inputs can be found in the `../input` folder, from the previous class activity provided by the instructor. The passes and asked results can be found in the passes' respective files, `defUse` and `LoopInfo`. 

# Pass 1: DefUseChain Pass, inside `defUse`
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

# Pass 2: Loop Info, inside `LoopInfo`

## Canonicalizing inputs, inside `canonical`

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

## Run the pass, inputs are from the `canonical` folder
```bash 
opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/matmul_canonical.ll -disable-output
opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/max_canonical.ll -disable-output
opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/gcd_canonical.ll -disable-output
```


## A. Printing preheader and latch, both outter and inner loops 
 ```bash 
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/matmul_canonical.ll -disable-output 
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/max_canonical.ll -disable-output 
 opt -load-pass-plugin build/LoopInfoExample.dylib -passes=loop-info-example canonical/gcd_canonical.ll -disable-output 
```


## B. Implementation for the recursively call of `getSubLoops()`
```bash 
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" canonical/matmul_canonical.ll -disable-output 
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" canonical/max_canonical.ll -disable-output 
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" canonical/gcd_canonical.ll -disable-output 
```

## C. More loop information

I printed _ _ information in the iterative loop info pass, not in the recursive one 

```bash 
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" canonical/matmul_canonical.ll -disable-output 
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" canonical/max_canonical.ll -disable-output 
```
```bash
opt -load-pass-plugin build/LoopInfoExample.dylib -passes="recursive_getSubLoop" canonical/gcd_canonical.ll -disable-output 
```
