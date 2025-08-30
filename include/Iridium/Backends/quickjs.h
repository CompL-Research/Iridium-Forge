// {̧
//   uint8_t is_strict_mode : 1;
//   uint8_t has_prototype : 1;
//   uint8_t has_simple_parameter_list : 1;
//   uint8_t is_derived_class_constructor : 1;
//   uint8_t need_home_object : 1;
//   uint8_t func_kind : 2;
//   uint8_t new_target_allowed : 1;
//   uint8_t super_call_allowed : 1;
//   uint8_t super_allowed : 1;
//   uint8_t arguments_allowed : 1;
//   uint8_t backtrace_barrier : 1;

//   uint8_t *byte_code_buf; /* (self pointer) */
//   int byte_code_len;
//   JSAtom func_name;
//   JSVarDef *vardefs;         /* arguments + local variables (arg_count + var_count) (self pointer) */
//   JSClosureVar *closure_var; /* list of variables in the closure (self pointer) */
//   uint16_t arg_count;
//   uint16_t var_count;
//   uint16_t defined_arg_count; /* for length function property */
//   uint16_t stack_size;        /* maximum stack size */
//   JSContext *realm;           /* function realm */
//   JSValue *cpool;             /* constant pool (self pointer) */
//   int cpool_count;
//   int closure_var_count;
//   JSAtom filename;
//   int line_num;
//   int col_num;
//   int source_len;
//   int pc2line_len;
//   uint8_t *pc2line_buf;
//   char *source;
// } JSFunctionBytecode;
