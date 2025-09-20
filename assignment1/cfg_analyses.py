def get_path_lengths(cfg, entry):
  #Shortest edge-count from entry to every reachable node (BFS)
  distance = {entry: 0}  
  queue = [entry]
  while queue:
    u = queue.pop(0)
    neighbors = []
    if u in cfg:
      neighbors = cfg[u]

    for v in neighbors:
      if v not in distance:
        distance[v] = distance[u] + 1
        queue.append(v)
  return distance
    
def reverse_postorder(cfg, entry):
  #DFS then reverse the postorder list
  visited = set() # finished visiting at least once
  order = []

  def dfs(node):
    visited.add(node)
    neighbors = []
    if node in cfg:
      neighbors = cfg[node]

    for n in neighbors:
      if n not in visited:
        dfs(n)
    order.append(node)
  dfs(entry)
  order.reverse()
  return order
    
def find_back_edges(cfg, entry):
  #Edges (u, v) where v is an ancestor of u in the current DFS
  visited = set() #finished visiting at least once
  stack = set()
  back_edges = []
  def dfs(node):
    visited.add(node)
    stack.add(node)

    neighbors = []
    if node in cfg:
      neighbors = cfg[node]

    for n in neighbors:
      if n in stack:
        back_edges.append((node, n))
      elif n not in visited:
        dfs(n)
    stack.remove(node)
  dfs(entry)
  return back_edges
    

def is_reducible(cfg, entry):
  nodes = set()
  for k in cfg.keys():
    nodes.add(k)
  for succs in cfg.values():
    for t in succs:
      nodes.add(t)
          
  #everyone dominates everyone, entry dominates only itself
  dominators = {}
  for n in nodes:
    dominators[n] = set(nodes)
  dominators[entry] = set([entry])
  
  changed = True
  while changed:
    changed = False 
    for n in nodes:
      if n == entry: #skip it
        continue

      # predecessors of n, all nodes with an edge to n
      preds = []
      for p, succs in cfg.items():
        for s in succs:
          if s == n:
            preds.append(p)
            break
      if not preds:
        continue
  
      common = set(nodes)          #start with alll nodes
      for p in preds:
        next_common = set()
        for x in common:
          if x in dominators[p]:  #keeping x if x dominates
            next_common.add(x)
      common = next_common
  
      new_set = set(common)
      new_set.add(n)  #add n because a node always doinates iteself
  
  
      if new_set != dominators[n]:
        dominators[n] = new_set
        changed = True
    
  # reducible if every back edge's head dominates its tail
  backs = find_back_edges(cfg, entry)
  for (u, v) in backs:
    if v not in dominators[u]:
      return False
  return True
