## Installed LLVM 21 via Homebrew
Homebrew installs pre-compiled binaries for llvm@21 on macOS/Apple Silicon. Hence installation was fast and didnt take hours of compilation.
```bash
brew install llvm@21
```

## Setup environment variables (per-session)
```bash
export LLVM_PREFIX="$(brew --prefix llvm)"
export PATH="$LLVM_PREFIX/bin:$PATH"
export CC="$LLVM_PREFIX/bin/clang"
export CXX="$LLVM_PREFIX/bin/clang++"
```

# Task 1: Put the IR into canonical SSA

## `matmul.c`
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm inputs/matmul.c -o task1/matmul/matmul.ll
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" task1/matmul/matmul.ll -o task1/matmul/matmul.canonical.ll
```

## `max.c`
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm inputs/max.c -o task1/max/max.ll
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" task1/max/max.ll -o task1/max/max.canonical.ll
```

## `gcd.c`
```bash
clang -O0 -Xclang -disable-O0-optnone -S -emit-llvm inputs/gcd.c -o task1/gcd/gcd.ll
opt -S -passes="mem2reg,lcssa,loop-simplify,loop-rotate" task1/gcd/gcd.ll -o task1/gcd/gcd.canonical.ll
```

# Task 2: 
In cfgs sub-directory 
## `matmul.c` 
### pass=dot-cfg

```bash
opt -passes=dot-cfg ../../Task1/matmul/matmul_canonical.ll -disable-output
    Writing '.matmul.dot'...
    Writing '.main.dot'...
dot -Tpdf .matmul.dot -o matmul_cfg.pdf
```
###  passes=dot-dom
```bash
opt -passes=dot-dom ../../Task1/matmul/matmul_canonical.ll -disable-output
    Writing 'dom.matmul.dot'...
    Writing 'dom.main.dot'...
dot -Tpdf dom.matmul.dot -o matmul_domtree.pdf
```
### pass=dot-cfg
```bash
opt -passes="print<loops>" ../../Task1/matmul/matmul.canonical.ll -disable-output
```
### Expected output \
```bash
Loop info for function 'matmul':
Loop at depth 1 containing: %4<header>,%5,%6,%18,%21,%26,%29,%30<latch><exiting>
    Loop at depth 2 containing: %5<header>,%6,%18,%21,%26<latch><exiting>
        Loop at depth 3 containing: %6<header>,%18<latch><exiting>
Loop info for function 'main':\
Loop at depth 1 containing: %4<header>,%5,%18,%21,%22<latch><exiting>
    Loop at depth 2 containing: %5<header>,%18<latch><exiting>
```
### pass=loop-interchange
```bash
opt -passes=loop-interchange ../../Task1/matmul/matmul_canonical.ll -S -o matmul_interchanged.ll
```

### pass=licm
```bash
opt -passes=licm ../../Task1/matmul/matmul_canonical.ll -S -o matmul_licm.ll
```

## MAX.C
```bash
opt -passes=dot-cfg ../../Task1/max/max_canonical.ll -disable-output
dot -Tpdf .matmul.dot -o matmul_cfg.pdf
```
```bash
opt -passes=dot-dom ../../Task1/max/max_canonical.ll -disable-output
dot -Tpdf dom.matmul.dot -o matmul_domtree.pdf
```
```bash
opt -passes="print<loops>" ../../Task1/max/max_canonical.ll -disable-output
```
```bash
opt -passes=loop-interchange ../../Task1/max/max_canonical.ll -S -o max_interchanged.ll
```
```bash
opt -passes=licm ../../Task1/max/max_canonical.ll -S -o max_licm.ll
```
## GCD.C
```bash
opt -passes=dot-cfg ../../Task1/gcd/gcd_canonical.ll -disable-output
dot -Tpdf .matmul.dot -o matmul_cfg.pdf
```
```bash
opt -passes=dot-dom ../../Task1/gcd/gcd_canonical.ll -disable-output
dot -Tpdf dom.matmul.dot -o matmul_domtree.pdf
```
```bash
opt -passes="print<loops>" ../../Task1/gcd/gcd_canonical.ll -disable-output
```
```bash
opt -passes=loop-interchange ../../Task1/gcd/gcd_canonical.ll -S -o gcd_interchanged.ll
```
```bash
opt -passes=licm ../../Task1/gcd/gcd_canonical.ll -S -o max_licm.ll
```

# Task 3
The goal of this task is to explore the LLVM infrastructure using the llvm-tutor repository, understand how to build and run example LLVM passes, and record the output of those transformations.

## Clone the llvm-tutor
```bash
git clone https://github.com/banach-space/llvm-tutor.git
```
## Build llvm-tutor
```bash
cd llvm-tutor
mkdir build && cd build
```
```bash
cmake -DCMAKE_C_COMPILER="$CC" \
      -DCMAKE_CXX_COMPILER="$CXX" \
      -DLLVM_DIR="$LLVM_PREFIX/lib/cmake/llvm" \
      -DLT_LLVM_INSTALL_DIR="$LLVM_PREFIX" ..
cmake --build .
```
## Running the Hello Pass
Inside llvm-tutor/HelloWorld/build:
```bash
$CC -O1 -S -emit-llvm ../../inputs/input_for_hello.c -o input_for_hello.ll
```
Run the HelloWorld pass
```bash
opt -load-pass-plugin ./lib/libHelloWorld.dylib -passes=hello-world -disable-output input_for_hello.ll
```
Expected Output

```bash
(llvm-tutor) Hello from: foo
(llvm-tutor)   number of arguments: 1
(llvm-tutor) Hello from: bar
(llvm-tutor)   number of arguments: 2
(llvm-tutor) Hello from: fez
(llvm-tutor)   number of arguments: 3
(llvm-tutor) Hello from: main
(llvm-tutor)   number of arguments: 2
```
