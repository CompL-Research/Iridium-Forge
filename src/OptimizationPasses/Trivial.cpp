// 
// This file contains the trivial optimization passes.
// 

// 
// Set operations: 
// 
// - SUCC(x): returns the set of successors of x.
// - PRED(x): returns the set of predecessors of x.
// - ADD_SUCC(x, y): adds y to the successor set of x.
// - REMOVE_SUCC(x, y): removes y to the successor set of x.
// 

// 
// 1. Redundant goto elimination / BB merge
// 
//   [ BB1 ] --> [ BB2 ]
// 
//   REDUNDANT_GOTO_ELIM(BB1, BB2): 
//      ∧ BB2 ∈ SUCC(BB1) 
//      ∧ BB1 ∈ PRED(BB2) 
//      ∧ |SUCC(BB1)| = 1
//      ∧ |PRED(BB2)| = 1
//      ∧ BB1' =  [...INST(BB1), ...INST(BB2)]
//      ∧ SUCC(BB1') = SUCC(BB2)
//      ∧ (∀p ∈ PRED(BB1). ADD_SUCC(p, BB1'). REMOVE_SUCC(p, BB1))
// 
//    Note: Traversal of CFG basically guarantees the first two predicates to hold trivially.
// 


// 
// 2. Dead implicit bindings elimination
// 
//    DEAD_STORE_REMOVAL(BDUChain):
//      ∧ ∀s ∈ BDUChain.localBindings
//        ∧ |s| = 1
// 

// 
// 3. Dead Store removal
// 
//   -- Can we get rid of sloppy declarations?
//      -- Or when can we get rid of sloppy declarations??
//  
//    DEAD_STORE_REMOVAL(BDUChain):
//      ∧ ∀s ∈ BDUChain
//        ∧ |BDUChain(s)| = 1
//        ∧ JSSloppyDeclSEXP?
//        ∧ JSImplicitBindingDeclarationSEXP
