# How to build the project 
Find the llvm-tu
# Pass 1: DefUseChain Pass
Emitting IR 
```bash 
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm inputs/matmul.c -o matmul.ll
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm ../../inputs/max.c -o max.ll
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm ../../inputs/gcd.c -o gcd.ll
```


Run the pass 
```bash 
opt -load-pass-plugin ./def_use_chains.so -passes=def-use-chains -disable-output matmul.ll
```

# Pass 2: Loop Info

## Canonicalizing inputs 

### `matmul.c`
```bash
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" matmul.ll -o canonical/matmul.canonical.ll
```

### `max.c`
```bash
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" max.ll -o canonical/max.canonical.ll
```

### `gcd.c`
```bash
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" gcd.ll -o canonical/gcd_canonical.ll
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

## C. More loop information


