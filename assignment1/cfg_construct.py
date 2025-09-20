TERMINATORS = {"jmp", "br", "ret"}

def formingBB(instrs):
    blocks = []
    current = []

    if instrs is None:        
      instrs = []   #empty 

    for i in instrs:
        if "op" in i:                    
            current.append(i)
            if i["op"] in TERMINATORS:     # end block at terminator
                blocks.append(current) 
                current = []     # empty out for next block, as this block terminates
        else:                                
            if current:
                blocks.append(current)
            current = [i]

    if current: #any more block left?
        blocks.append(current)
    return blocks

def block_map(blocks):
    name2block = {} # name each block (use label if present, else bb0, bb1 and so on)
    for i, block in enumerate(blocks):
        if block and "label" in block[0]:
            name = block[0]["label"]; body = block[1:]
        else:
            name = f"bb{i}"; body = block
        name2block[name] = body
    return name2block

def get_cfg(name2block):
    out = {}  #block_name -> list of successor names, edgelist formatr 
    names = list(name2block.keys())
    for i, name in enumerate(names):
        block = name2block[name]
        if block:
            last = block[-1]
            if "op" in last and (last["op"] == "jmp" or last["op"] == "br"):
                out[name] = list(last["labels"]) if "labels" in last else []
            elif "op" in last and last["op"] == "ret":
                out[name] = []
            else:
                out[name] = [names[i+1]] if i + 1 < len(names) else []
        else:
            out[name] = [names[i+1]] if i + 1 < len(names) else []
    return out




