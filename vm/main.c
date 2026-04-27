#include <mcu/core.h>
#include <mcu/handlers.h>
#include <mcu/io.h>

#include "vm.h"

#define MAKE_INSTR(opc, var, op) \
   { .op_code = opc, .variant = var, .operand = (u8) op }

#define REG_DEST_SRC(dest, src) \
   (dest << 4) | (src & 0x0F)

#define RESERVE_WORD() \
   {0}, {0}

// @todo: Write an assembler
Instr program[] = {
   MAKE_INSTR(OC_Jmp, OV_VE, 0),                  // 0
   RESERVE_WORD(),                                // 1
   
   MAKE_INSTR(OC_Mov, OV_RV, 0),                  // 3
   RESERVE_WORD(),                                // 4
   MAKE_INSTR(OC_Mov, OV_RR, REG_DEST_SRC(1, 0)), // 6
   
   MAKE_INSTR(OC_Add, OV_RR, REG_DEST_SRC(0, 1)), // 7
   
   MAKE_INSTR(OC_Jmp, OV_VA, 0),                  // 8
   RESERVE_WORD(),                                // 9
   
   MAKE_INSTR(OC_Halt, 0, 0),                     // 11
};

i32 main() {
   VM vm = VM_new(1024);
   i32 result;

   Word val = 8 * sizeof(Instr);
   memcpy(program + 1, &val, sizeof(Word));

   val = 1;
   memcpy(program + 4, &val, sizeof(Word));

   val = 3 * sizeof(Instr);
   memcpy(program + 9, &val, sizeof(Word));

   if (VM_load_program_from_array(&vm, program, sizeof(program))) {
      eprintln("[!] Failed to load program from file, reason: \"OOM\"");
      goto failure;
   }

   for (usize i = 0; i < 25; ++i) {
      /* println("cycle : %zu", i); */
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

