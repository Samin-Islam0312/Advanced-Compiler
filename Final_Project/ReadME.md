# LLVM FMA Fusion Pass (ARM64 Optimization)

## Project Overview
This project implements an **LLVM Optimization Pass** that detects floating-point multiply-add patterns (`a * b + c`) and transforms them into hardware-accelerated **Fused Multiply-Add (FMA)** intrinsic calls (`@llvm.fma`). 

Designed for the **Apple M1 (ARM64)** architecture, this pass aims to:
1.  **Improve Numerical Precision:** FMA performs only one rounding step instead of two.
2.  **Increase Performance:** Leverages the high-throughput FMA units in modern CPUs.

## Features
* **Basic Fusion:** Transforms `(a * b) + c` → `@llvm.fma(a, b, c)`.
* **Commutativity:** Handles `c + (a * b)` automatically.
* **Fused Multiply-Subtract (FSub):** Detects `(a * b) - c` and `c - (a * b)` and generates FMAs with negated operands.
* **Safe Optimization:**
    * **Fast-Math Aware:** Strictly respects the `allowContract` flag. If a user compiles with `-ffp-contract=off`, the pass does nothing (preserving strict IEEE behavior).
    * **Target Aware:** Queries `TargetTransformInfo` (TTI) to ensure the target CPU supports efficient FMA hardware.
    * **Graph Safety:** Supports "Multi-Use" scenarios (e.g., if the multiplication is used elsewhere, it is preserved while the addition is optimized).
* **Constant Folding:** Uses LLVM's `APFloat` engine to calculate FMA results for constant operands at compile-time.

## Prerequisites
* **LLVM 15+** (Verified on LLVM 21.1.2)
* **CMake 3.13+**
* **C++17 Compiler** (Clang recommended)
* **PolyBench/C** (For performance benchmarking)

## Directory Structure
```text
Final_Project/
├── CMakeLists.txt          # Build configuration
├── README.md               # Documentation
├── pass/
│   └── pass.cpp            # The core LLVM Pass logic
├── benchmarks/
│   ├── testSuite/          # Micro-tests for correctness
│   ├── polyBench/          # PolyBench-C 4.2.1 suite
│   ├── run_polybench.py    # Python script for performance testing
│   └── run_tests.sh        # Shell script for micro-test verification
└── build/                  # Build artifacts
````

## Build Instructions

1. BUILD & COMPILE THE PASS

    ```bash
    mkdir build && cd build
    cmake -DCMAKE_CXX_COMPILER=$(brew --prefix llvm)/bin/clang++ ..
    make
    ```
This generates `build/FMAContractPass.dylib`.*

### Running on a Single File
To run the pass on a C file, you must first compile it to LLVM IR with contraction enabled (`-ffp-contract=fast`).

```bash
# Generate IR
clang -O1 -S -emit-llvm -ffp-contract=fast input.c -o input.ll

# Run the Pass
opt -load-pass-plugin=./build/FMAContractPass.dylib -passes="fma-pass" input.ll -S -o output.ll
```

## Testing & Validation

### 1\. Micro-Test Suite (Correctness)

Verifies that the pass correctly handles specific patterns (subtraction, commutativity, constant folding) and respects legality rules.

```bash
./run_tests.sh
```

**Expected Output:**

  * `base`, `negate`, `multiUse` -\> **[PASS] FMA Intrinsic Detected\!**
  * `noContract` -\> **[PASS] No FMA Detected (Expected behavior).**

### 2\. PolyBench (Performance)

Benchmarks the pass against standard Matrix Multiplication and Linear Algebra kernels.

```bash
python3 run_polybench.py
```

**Sample Results (Apple M1):**
| KERNEL | BASE (s) | OPT (s) | SPEEDUP |
| :--- | :--- | :--- | :--- |
| **gemm** | 0.260s | 0.261s | **1.00x** |
| **2mm** | 1.643s | 2.215s | **0.74x** |
| **bicg** | 0.017s | 0.017s | **0.99x** |

## Future Work
  * **Vectorization:** Extend the detection logic to explicitly handle vector intrinsics if the autovectorizer runs before this pass.
  * **Loop Unrolling:** Integrate with loop passes to expose more FMA opportunities.

## Author

Samin Islam
Advanced Compiler Term Project

```
```