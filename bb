



           [SB]
             |
            / \ _______
           /   \       \
         [SID] [SID]  [SID]
                 |
                 |
               [SID]

Flattening ():
  1. Pre-order traversal of the tree, each time a node is visited, it gets assigned a stack index.
  2. At SID boundary, the next connects to parent SID/SB.
  3. At SB boundary, the next connects to nothing, -1.

A stack frame defines a bunch of bindings.
These bindings can be referenced.
(i) EnvBindingSEXP, whose stackIDX will be fixed by the pre-order traversal.
(ii) RemoteEnvBindingSEXP, these bindings are interesting,
  - They are referenced using a stack offset index.
  - These inturn point to parent stack frame indices.