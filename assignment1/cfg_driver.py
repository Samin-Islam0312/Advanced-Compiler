import sys, json
from cfg_construct import formingBB, block_map, get_cfg
from cfg_analyses import get_path_lengths, reverse_postorder, find_back_edges, is_reducible

if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "cfg"

    prog = json.load(sys.stdin)
    func = prog["functions"][0]            # first function
    body = func["instrs"]                  

    blocks = formingBB(body)
    name2block = block_map(blocks)
    cfg = get_cfg(name2block)

    keys = list(name2block.keys())
    entry = keys[0] if keys else None

    if cmd == "blocks":
        print(name2block)
    elif cmd == "cfg":
        for name in keys:
            for s in (cfg[name] if name in cfg else []):
                print(f"{name} -> {s}")
    elif cmd == "path_len" and entry is not None:
        print(get_path_lengths(cfg, entry))
    elif cmd == "rPostOrder" and entry is not None:
        print(reverse_postorder(cfg, entry))
    elif cmd == "backEdges" and entry is not None:
        print(find_back_edges(cfg, entry))
    elif cmd == "reducible" and entry is not None:
        print("true" if is_reducible(cfg, entry) else "false")

