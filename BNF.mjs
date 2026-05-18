export default {
  "File": {
    "category": "Structural",
    "traits": [],
    "info": "Top Level container representing a file in Iridium",
    "args": {},
    "flags": {
      "string": {},
      "void": {
        "JSScript": "Indicates script mode code",
        "JSModule": "Indicates module mode code",
        "TLA": "Set if file uses top level async (only valid for JSModule)"
      },
      "bool": {},
      "double": {},
    }
  },
  "ResolveEnvBinding": {
    "category": "RVAL",
    "traits": ["Transitional"],
    "info": "@ResolveEnvBinding is a transitional node in IRI. It indicates a reference to an yet unresolved identifier. Upon resolution, this is replaced by @EnvBindingSEXP or @RemoteEnvBindingSEXP or @GlobalBindingSEXP",
    "args": {},
    "flags": {
      "string": {
        "NAME": "The name of the identifier"
      },
      "void": {
        "ASW": "Always Safe Write flag (used for function arguments)",
      },
      "bool": {},
      "double": {},
    }
  },
  "List": {
    "category": "Structural",
    "traits": [],
    "info": "A generic List container. May contain homogenous or heterogenous elements.",
    "args": {},
    "flags": {
      "string": {
        "TYPE": "Non-empty for homogenous lists."
      },
      "void": {},
      "bool": {},
      "double": {},
    }
  },
  "JSImplicitBindingDeclaration": {
    "crum": ["STMT"],
    "info": "A generic List container. May contain homogenous or heterogenous elements.",
    "args": {},
    "flags": {
      "string": {
        "TYPE": "Non-empty for homogenous lists containing element of a fixed TAG."
      },
      "void": {},
      "bool": {},
      "double": {},
    }
  },

};
