import sys
import json
from collections import namedtuple

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]           
CFG_DIR = ROOT / "assignment1"
sys.path.insert(0, str(CFG_DIR))

from cfg_construct import formingBB, block_map, get_cfg  

def form_blocks(instrs):
    return formingBB(instrs)

    """ Colecting predecessors/successors as sets from my get_cfg() successor map.
    `blocks` is the name→ body dict produced by block_map(formingBB(...))."""

def edges(blocks):
    succ_list = get_cfg(blocks)                 # { name: [succ1, succ2, ...] }
    succs = {b: set(lst) for b, lst in succ_list.items()}
    preds = {b: set() for b in blocks}
    for u, outs in succs.items():
        for v in outs:
            preds[v].add(u)
    return preds, succs

"""Analysis spec:
	- forward : True (forward) / False (backward)
	- init : initial lattice value (e.g., ∅ or Universe)
	- merge : meet over predecessors/successors (union / intersection)
	- transfer : per-block transfer function
"""

Analysis = namedtuple("Analysis", ["forward", "init", "merge", "transfer"])

def union(sets):
    out = set()
    for s in sets:
        out.update(s)
    return out

def intersect(sets):
    acc = None
    for s in sets:
        if acc is None:	# First item, initialize to a copy to avoid aliasing input.
            acc = set(s)
        else:		 # Later items, intersect with the running result (non-mutating).
            acc = acc & s
    if acc is None:	 # No items at all → return empty set.
        return set()
    else:
        return acc

def df_worklist(blocks, analysis):		#Worklist algorithm to a fixed point
    preds, succs = edges(blocks)

    # Switch edges/direction
    if analysis.forward:
        first_block = list(blocks.keys())[0]           # Entry
        in_edges, out_edges = preds, succs
    else:
        first_block = list(blocks.keys())[-1]          # Exit
        in_edges, out_edges = succs, preds

    # Initialize
    in_  = {first_block: analysis.init}
    out  = {node: analysis.init for node in blocks}

  
    worklist = list(blocks.keys())
    while worklist:	  # Iterate
        node = worklist.pop(0)
        inval = analysis.merge(out[n] for n in in_edges[node])
        in_[node] = inval
        outval = analysis.transfer(blocks[node], inval)
        if outval != out[node]:
            out[node] = outval
            worklist += out_edges[node]

    return (in_, out) if analysis.forward else (out, in_)

def fmt(val):
    if isinstance(val, set):
        return "∅" if not val else ", ".join(v for v in sorted(val))
    elif isinstance(val, dict):
        return "∅" if not val else ", ".join("{}: {}".format(k, v) for k, v in sorted(val.items()))
    else:
        return str(val)


def run_df(bril, analysis_name):
    for func in bril["functions"]:		# Build CFG blocks (dict: name -> list of instrs)
        blocks = block_map(form_blocks(func["instrs"]))

        spec = ANALYSES[analysis_name]
        analysis = spec(blocks) if callable(spec) else spec

        in_, out = df_worklist(blocks, analysis)
        for block in blocks:
            print(f"{block}:")
            print("  in: ", fmt(in_[block]))
            print("  out:", fmt(out[block]))


"""--------------------------------------------------------------|
| TASK 3 : Reaching Definitions (forward, may)                   |
| Facts are definition *sites* as strings: "var@block:instrIndex"|
----------------------------------------------------------------"""

def _rd_ids_all(blocks):
    """ collecting all def IDs and the last def per variable in each block.
                   Returns:
         var_to_ids : var -> set of def-ids
         lastDef_inBlock: id(block) -> {var -> last def-id in that block}   """
    var_to_ids = {}
    lastDef_inBlock = {}
    for name, block in blocks.items():
        last = {}
        for ii, instr in enumerate(block):
            v = instr.get("dest")
            if not v:
                continue
            did = f"{v}@{name}:{ii}"
            var_to_ids.setdefault(v, set()).add(did)
            last[v] = did
        lastDef_inBlock[id(block)] = last
    return var_to_ids, lastDef_inBlock


def _rd_gen_kill(blocks):
    var_to_ids, lastDef_inBlock = _rd_ids_all(blocks)	# Per-block GEN/KILL keyed by id(block)
    GEN, KILL = {}, {}
    for name, block in blocks.items():
        blockID = id(block)
        last = lastDef_inBlock[blockID]    # var → last def-id inside this block
        GEN[blockID]  = set(last.values())
        kill = set()
        for v, did in last.items():
            kill |= (var_to_ids.get(v, set()) - {did})
        KILL[blockID] = kill
    return GEN, KILL

def reachingDefs_DFAnalysis(blocks):
    GEN, KILL = _rd_gen_kill(blocks)
    return Analysis(
        forward=True,
        init=set(),  # IN[entry] = ∅ ; OUT[*] starts ∅
        merge=union, # may-analysis → union 
        transfer=lambda block, INb: GEN.get(id(block), set()) | (INb - KILL.get(id(block), set())),
    )


"""-----------------------------------------------|
| TASK 4 : Available Expressions (forward, must)  |
|-------------------------------------------------| """

""" 
commutative
eq → == ;         or → bitwise or;       xor → bitwise XOR """ 
_COMM = {"add", "mul", "and", "or", "eq", "xor"}     

""" 
lt → less than, 
le → less than or equal, 
gt → greater than, 
ge → greater than or equal, 
not → negation"""
_PURE = _COMM | {"sub", "div", "lt", "le", "gt", "ge", "not"}

#Return (expr_string, vars_used_set) if the instr is a pure expr, else (None, set())
def canonicalExp(instr):
    op = instr.get("op")
    if op not in _PURE:
        return None, set()
    args = list(instr.get("args", []))
    if not args:
        return None, set()
    if op in _COMM:
        args = sorted(args)  # canonicalize commutative ops
    expr = f"{op}({', '.join(args)})" if len(args) > 1 else f"{op} {args[0]}"
    return expr, set(args)

def _ae_universe(blocks):
    U, expr_vars = set(), {}	#Universe of pure expressions and map expr → vars used
    for block in blocks.values():
        for instr in block:
            e, vs = canonicalExp(instr)
            if e:
                U.add(e)
                expr_vars[e] = vs
    return U, expr_vars

def _ae_gen_kill(blocks, U, expr_vars):
    GEN, KILL = {}, {}	    # Per-block GEN/KILL keyed by id(block)
    for name, block in blocks.items():
        blockID = id(block)

        # Vars defined in this block (for kills)
        defs = {instr["dest"] for instr in block if "dest" in instr}
        KILL[blockID] = {e for e in U if expr_vars[e] & defs}

        # GEN: scan backward; include exprs whose operands aren't redefined later in the block
        killed_vars, gen_set = set(), set()
        for instr in reversed(block):
            if "dest" in instr:
                killed_vars.add(instr["dest"])
            e, vs = canonicalExp(instr)
            if e and not (vs & killed_vars):
                gen_set.add(e)
        GEN[blockID] = gen_set
    return GEN, KILL

def availableExp_DFAnalysis(blocks):
    U, expr_vars = _ae_universe(blocks)
    GEN, KILL = _ae_gen_kill(blocks, U, expr_vars)
    return Analysis(
        forward=True,
        init=set(U),                  # MUST analysis: seed OUT[*] with Universe, and meet operator = intersection
        merge=intersect,              
        transfer=lambda block, INb: GEN.get(id(block), set()) | (INb - KILL.get(id(block), set())),
    )


ANALYSES = {
    "rd": reachingDefs_DFAnalysis,
    "ae": availableExp_DFAnalysis,
}

if __name__ == "__main__":
    bril = json.load(sys.stdin)
    if len(sys.argv) < 2 or sys.argv[1] not in ANALYSES:
        sys.exit("usage: df.py [rd|ae]\n  rd = Reaching Definitions\n  ae = Available Expressions")
    run_df(bril, sys.argv[1])

