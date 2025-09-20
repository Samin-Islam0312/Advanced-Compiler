# Assignment 1 — Bril CFG + Analyses

This folder implements **CFG construction** and a few **basic analyses** over **Bril** programs.  

---
## Files
- `cfg_construct.py` – **CFG builder**
  - `formingBB(instrs)` — split a flat label/instruction list into basic blocks (new block at a label; block ends at `jmp`/`br`/`ret`).
  - `block_map(blocks)` — name each block by its label (else `bb0`, `bb1`, …) → `{name: [instrs...]}`.
  - `get_cfg(name2block)` — compute successors → `{name: [succs...]}`.  
    Rules: `jmp`/`br` use their labeled targets; `ret` has none; otherwise **fall through** to the next block.
- `cfg_analyses.py` – **Analyses**
  - `get_path_lengths(cfg, entry)` — shortest edge-count from entry (BFS). Unreachable nodes omitted.
  - `reverse_postorder(cfg, entry)` — reverse postorder (DFS).
  - `find_back_edges(cfg, entry)` — edges `(u,v)` where `v` is an ancestor of `u` in DFS.
  - `is_reducible(cfg, entry)` — `True` iff every back-edge head dominates its tail (single-entry loops only).
- `cfg_driver.py` – **Minimal CLI** that wires the above together.
- `test/` – Bril test programs.

---

## What this does
1. **Input:** a Bril program (text).
2. **Parse:** convert to JSON via `bril2json`.
3. **CFG build:** form basic blocks, name them, compute successors.
4. **Analyses:** run one command (BFS distance, reverse postorder, back edges, reducibility).

> Library files take the JSON **dict** from `bril2json`. The driver reads JSON on **stdin** and prints results.

---

## Requirements 
- Python **3.8+** (macOS/Linux/Windows).
- Bril command-line tools
Bril repo: [sampsyo/bril](https://github.com/sampsyo/bril) · 
Bril install video: <https://vod.video.cornell.edu/media/1_jc91ke0h>

---
After setting up the dependencies and cloning this repo, run the commands below 

```bash
# CFG edges for a test file (default command is `cfg`)
bril2json < test/testcase1.bril | python3 cfg_driver.py
````

## Examples, run from inside `assignment1/`

```bash
bril2json < test/testcase2.bril | python3 cfg_driver.py blocks
bril2json < test/testcase3.bril | python3 cfg_driver.py path_len
bril2json < test/testcase4.bril | python3 cfg_driver.py rPostOrder
bril2json < test/testcase4.bril | python3 cfg_driver.py backEdges
bril2json < test/testcase5.bril | python3 cfg_driver.py reducible     # expect: true
bril2json < test/testcase6.bril | python3 cfg_driver.py reducible     # expect: false (irreducible)
```


## To make your own test cases in Bril
Put a `.bril` file in `test/` and run:

```bash
bril2json < test/my_case.bril | python3 cfg_driver.py <command>
```

## Syntax notes

* **Comments:** `#` to end of line

  ```bril
  x: int = const 1;  # trailing comment
  ```

* **Labels:** define with a leading dot and reference with a dot

  ```bril
  .entry:
    jmp .next;
  .next:
    ret;
  ```

* **Semicolons:** every instruction ends with `;`.
