#include <mcu/core.h>
#include <mcu/handlers.h>
#include <mcu/io.h>

#include "vm.h"

#define MAKE_INSTR(opc, var, op) \
   { .op_code = opc, .variant = var, .operand = (u8) op }

Instr program[] = {
   MAKE_INSTR(OC_Mov, OV_RV, 0),
   MAKE_INSTR(OC_Mov, OV_RV, 1),
   MAKE_INSTR(OC_Halt, 0, 0),
};

i32 main() {
   VM vm = VM_new(1024);
   i32 result;

   if (VM_load_program_from_array(&vm, program, sizeof(program))) {
      eprintln("[!] Failed to load program from file, reason: \"OOM\"");
      goto failure;
   }

   loop {
      Trap trap = VM_perform_cycle(&vm);
      if (trap != T_Ok) {
         if (trap == T_Halt) break;

         eprintln("VM failure, reason: \"%s\"", Trap_to_cstr(trap));
         goto failure;
      }
   }

   VM_dump_registers(&vm);
   result = 0;
   goto cleanup;

failure:
   VM_dump_registers(&vm);
   result = 1;

cleanup:
   VM_free(&vm);
   return result;
}

