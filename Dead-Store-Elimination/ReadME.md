# MemorySSA - Dead Store Elimination
## Directory description

## Compiling the demo pass 
```bash
clang++ -std=c++17 -fPIC -shared MemorySSADemo.cpp -o dylibs/libMemorySSADemo.dylib $(llvm-config --cxxflags --ldflags) -lLLVM 
```
Compiling the test examples to LLVM IR 
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/demo1.c -o irFiles/demo1.ll

clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/demo2.c -o irFiles/demo2.ll

clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/test1.c -o irFiles/test1.ll

clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/branch_example.c -o irFiles/branch_example.ll

clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/array_loop1.c -o irFiles/array_loop1.ll
```


## Running the pass with mem2reg 
```bash 
opt -load-pass-plugin dylibs/libMemorySSADemo.dylib \
-passes=mem2reg,memssa-demo \
irFiles/demo1.ll \
-disable-output
```
and the output
```bash
Analyzing function: demo
BasicBlock: 
  1 = MemoryDef(liveOnEntry)
  2 = MemoryDef(1)
```
```bash
opt -load-pass-plugin dylibs/libMemorySSADemo.dylib \
    -passes=mem2reg,memssa-demo \
    irFiles/demo2.ll \
    -disable-output
```
and the output
```bash
Analyzing function: demo
BasicBlock: 
  1 = MemoryDef(liveOnEntry)
  2 = MemoryDef(1)
  MemoryUse(2)
```

```bash 
opt -load-pass-plugin dylibs/libMemorySSADemo.dylib \
    -passes=mem2reg,memssa-demo \
    irFiles/array_loop1.ll \
    -disable-output
```
and the output
```bash
Analyzing function: array_loop
BasicBlock: 
BasicBlock: 
  MemoryPhi for block :
    from : 0 = MemoryDef(liveOnEntry)
    from : 2 = MemoryDef(1)
BasicBlock: 
  MemoryUse(3)
  1 = MemoryDef(3)
  MemoryUse(1)
  2 = MemoryDef(1)
BasicBlock: 
BasicBlock: 
```

```bash
opt -load-pass-plugin dylibs/libMemorySSADemo.dylib \
    -passes=mem2reg,memssa-demo \
    irFiles/branch_example.ll \
    -disable-output
```
and the output
```bash
Analyzing function: branch_example
BasicBlock: 
  1 = MemoryDef(liveOnEntry)
BasicBlock: 
  2 = MemoryDef(1)
BasicBlock: 
  3 = MemoryDef(1)
BasicBlock: 
  MemoryPhi for block :
    from : 2 = MemoryDef(1)
    from : 3 = MemoryDef(1)
  MemoryUse(5)
  4 = MemoryDef(5)
```

```bash
opt -passes=mem2reg irFiles/demo1.ll -S -o irFiles/demo1_simplified.ll

opt -passes=mem2reg irFiles/demo2.ll -S -o irFiles/demo2_simplified.ll

opt -passes=mem2reg irFiles/test1.ll -S -o irFiles/test1_simplified.ll

opt -passes=mem2reg irFiles/branch_example.ll -S -o irFiles/branch_example_simplified.ll

opt -passes=mem2reg irFiles/array_loop1.ll -S -o irFiles/array_loop1_simplified.ll
```

## Pass extension for graphical view of the MemorySSA graph.
```bash
clang++ -std=c++17 -fPIC -shared MemorySSAGraph.cpp -o dylibs/libMemorySSAGdylib \
$(llvm-config --cxxflags --ldflags --system-libs --libs core analysis passes)
```

```bash
opt -load-pass-plugin dylibs/libMemorySSAGdylib \
    -passes=memssa-demo \
    irFiles/demo1_simplified.ll \
    -disable-output
```
```bash 
opt -load-pass-plugin dylibs/libMemorySSADSE.dylib \
    -passes=mem2reg,memssa-dse \
    tests/task5_tests/mayAnalysis/irFiles/test1.ll \
    -disable-output
```
```bash
dot -Tpdf mssa_demo1.dot -o graphs/mssa_demo1.pdf
```
```bash
opt -load-pass-plugin dylibs/libMemorySSAGdylib \
    -passes=memssa-demo \
    irFiles/demo2_simplified.ll \
    -disable-output
```
```bash
dot -Tpdf mssa_demo.dot -o graphs/mssa_demo2.pdf
```
```bash
opt -load-pass-plugin dylibs/libMemorySSAGdylib \
    -passes=memssa-demo \
    irFiles/array_loop1_simplified.ll \
    -disable-output
```
```bash
dot -Tpdf mssa_array_loop.dot -o graphs/mssa_array_loop.pdf
```
```bash
opt -load-pass-plugin dylibs/libMemorySSAGdylib \
    -passes=memssa-demo \
    irFiles/branch_example_simplified.ll \
    -disable-output
```
```bash
dot -Tpdf mssa_branch_example.dot -o graphs/mssa_branch_example.pdf
```

## Intraprocedural dead store elimination (DSE) pass 

Implemented the lecture DSE algorithm using MemorySSA; restricted MustAlias to Ptr == PrevPtr and only considered same-basic-block stores, 
which makes post-dominance trivial. Intervening uses are checked by scanning between the two stores in that block for loads from the same pointer.

```bash
clang++ -std=c++17 -fPIC -shared DeadStoreElimination/DeadStoreE.cpp -o dylibs/libMemorySSADSE.dylib \
$(llvm-config --cxxflags --ldflags --system-libs --libs core analysis passes)
```

## Compiling the test cases 
In the `tests/mayAnalysis` and `tests/mustAnalysis` directories. The IR files are kept in `irFiles` folder in each of the 
may and must analysis folder in the `tests` directory

### May Analysis
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/task5_tests/mayAnalysis/test1.c -o tests/task5_tests/mayAnalysis/irFiles/test1.ll 
```
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/task5_tests/mayAnalysis/test2.c -o tests/task5_tests/mayAnalysis/irFiles/test2.ll
```
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/task5_tests/mayAnalysis/test3.c -o tests/task5_tests/mayAnalysis/irFiles/test3.ll
```

### Must Analysis
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/task5_tests/mustAnalysis/test1.c -o tests/task5_tests/mustAnalysis/irFiles/test1.ll 
```
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/task5_tests/mustAnalysis/test2.c -o tests/task5_tests/mustAnalysis/irFiles/test2.ll
```
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm tests/task5_tests/mustAnalysis/test3.c -o tests/task5_tests/mustAnalysis/irFiles/test3.ll
```

## Run the pass on the IR files 
```bash
opt -load-pass-plugin dylibs/libMemorySSADSE.dylib \                                                  
    -passes=mem2reg,memssa-dse \
    tests/task5_tests/mayAnalysis/irFiles/test1.ll \
    -disable-output
```
and the output 

```bash
Running MemorySSADSEPass on function: dse_may1
```
```bash
opt -load-pass-plugin dylibs/libMemorySSADSE.dylib \
-passes=mem2reg,memssa-dse \
tests/task5_tests/mustAnalysis/irFiles/test1.ll \
-disable-output
```
And the output
```bash
Running MemorySSADSEPass on function: dse_must1
  DSE: removing dead store:   store i32 10, ptr %0, align 4
```

```bash
opt -load-pass-plugin dylibs/libMemorySSADSE.dylib \
-passes=mem2reg,memssa-dse \
tests/task5_tests/mustAnalysis/irFiles/test3.ll \
-disable-output
```

and the output

```bash
Running MemorySSADSEPass on function: dse_must_basic2
  DSE: removing dead store:   store i32 %1, ptr %0, align 4
```
