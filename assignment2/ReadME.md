# Assignment 2: Dataflow Analysis

## Task 1 & 2
Task 1 & 2 are written in the pdf file. 

## Tasks 3 & 4
The code for Tasks 3 & 4 is implemented in **`task3_4.py`**. This file defines a reusable dataflow worklist algorithm, provided in the bril repository and two specific analyses.

### Key Components
  - **CFG Imports from Assignment 1)**
  - `formingBB`, `block_map`, `get_cfg` are imported from `assignment1/cfg_construct.py` for forming blocks and CFG construction. 
  
  - **`edges(blocks)`**  
    - Builds predecessor and successor maps from the CFG. Returns `(preds, succs)` as sets for each block.
  
  - **`Analysis` spec (namedtuple)**
  - `Analysis` = namedtuple("Analysis", ["forward", "init", "merge", "transfer"]):
      - Encapsulates an analysis spec: direction, initial value, meet/merge op, and per-block transfer fn.
      - `union(sets)` / `intersect(sets)`: generic set meet operators (may vs must).
      - `df_worklist(blocks, analysis)`: Worklist to fixed-point (adapted from Bril’s examples/df.py).
      - Initializes in_ / out, merges along CFG, applies transfer, and iterates until convergence.
      - `fmt(val)`: pretty-prints sets/maps (e.g., ∅ for empty).
      - `run_df(bril, analysis_name)`: For each function: builds blocks, runs df_worklist, prints in/out per block.
  
  The parameters of a dataflow analysis:  
      - `forward` → direction (forward/backward)  
      - `init` → initial lattice value  
      - `merge` → meet operator (union / intersection)  
      - `transfer` → block transfer function  

### Task 3: Reaching Definitions
- `reachingDefs_DFAnalysis()`: dataflow implementation.  
- Facts are **definition sites** (e.g., `x@block:instrIndex`).  
- Uses per-block **GEN/KILL** sets from `_rd_gen_kill()`.
- `_rd_ids_all(blocks)`: Collects all def-site IDs per variable and the last def per variable within each block.
- `_rd_gen_kill(blocks)`: Computes per-block GEN (last defs in block) and KILL (other defs of the same vars).
- **Spec:**  
  - Direction: Forward  
  - Init: `∅`  
  - Merge: Union  
  - Transfer: `OUT[b] = GEN[b] ∪ (IN[b] – KILL[b])`

### Task 4: Available Expressions
- `availableExp_DFAnalysis()`: dataflow implementation .  
- `canonicalExp()`: recognizes pure ops (e.g., add, mul, sub, div, and, or, xor, eq, lt, le, gt, ge, not). Canonizes commutative ops by sorting operands so a+b ≡ b+a. Returns (expr_string, vars_used_set) or (None, set()).
- `_ae_universe(blocks)`: Gathers the universe U of all pure expressions in the function, plus a map expr → vars.
- `_ae_gen_kill(blocks, U, expr_vars)`: KILL[b]: expressions in U that mention a var defined in b.
                                        GEN[b]: scan b backward and collect expressions whose operands aren’t redefined later in b.
- **Spec:**  
  - Direction: Forward  
  - Init: Universe of expressions (must analysis)  
  - Merge: Intersection  
  - Transfer: `OUT[b] = GEN[b] ∪ (IN[b] – KILL[b])`


### Tests
All test cases for Reaching Definitions and Available Expressions are located in the `tests/` directory. Assigned to use **Turnt** (from the Bril toolchain) to automatically run `.bril` programs through the analyses and compare results against expected outputs.
- Setup: there are two separate subfolders under tests/:
  - tests/rd/ → for Reaching Definitions
  - tests/ae/ → for Available Expressions
  Each folder has its own turnt.toml file that hard-codes the analysis argument (rd or ae).
- Generating Golden Outputs: running the Turnt first time, the expected output (.out files) for each test must be saved:\
   For Reaching Definitions \
  ```turnt --save tests/rd/*.bril```

  For Available Expressions\
  ```turnt --save tests/ae/*.bril```
  
- Running Tests: Once golden outputs are saved, simply run:\

  - For reaching definition analysis \
    ```turnt tests/rd/*.bril```
    
    <img width="468" height="103" alt="Screenshot 2025-10-03 at 11 32 05 PM" src="https://github.com/user-attachments/assets/f11b4635-2c05-4661-a1cd-5b588dcdaa7a" />
    
  - For available expression \
    ```turnt tests/ae/*.bril```
  
    <img width="471" height="122" alt="Screenshot 2025-10-03 at 11 15 28 PM" src="https://github.com/user-attachments/assets/7b6a9fcd-fb9f-4947-896c-9672465a8dcc" />