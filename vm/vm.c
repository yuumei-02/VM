#include <mcu/core.h>
#include <mcu/memory.h>
#include <mcu/io.h>

#include "vm.h"

VM VM_new(usize memory_size) {
   return (VM) {
      .memory = mcu_malloc(memory_size),
      .memory_size = memory_size
   };
}

void VM_free(VM* self) {
   mcu_assert(self != nullptr, "self can't be null");

   *self = (VM) {0};
}

bool VM_load_program_from_array(VM* self, Instr* program, usize program_size) {
   mcu_assert(self != nullptr, "self can't be null");
   mcu_assert(program != nullptr, "program can't be null");

   if (self->memory_size < program_size)
      return true;

   memset(self->memory, 0, self->memory_size);
   memcpy(self->memory, program, program_size);

   self->registers.ip = 0;
   return false;
}

Instr VM_get_instr(VM* self) {
   u8* base = self->memory + self->registers.ip;
   
   return (Instr) {
      .op_code = *(OpCode*)    (base),
      .variant = *(OpVariant*) (base + 2),
      .operand = *(u8*)        (base + 3)
   };
}

Trap VM_perform_cycle(VM* self) {
   mcu_assert(self != nullptr, "self can't be null");

   if (self->registers.ip >= self->memory_size) {
      return T_IllegalAddress;
   }
   
   Instr instr = VM_get_instr(self);
   self->registers.ip += 4;

   switch (instr.op_code) {
      case OC_Halt: {
         return T_Halt;
      }
      
      case OC_Mov: {
         switch (instr.variant) {
            case OV_RR: mcu_todo("not yet implemented");
            case OV_RV: {
               if (instr.operand >= GPR_REG_COUNT)
                  return T_IllegalRegister;

               /* self->registers.gpr[instr.operand] = self->memory[self->registers.ip++]; */
               self->registers.gpr[instr.operand] = 69;
            } break;
            
            default: {
               return T_IllegalInstructionVariant;
            }
         }
      } break;
   
      default: {
         return T_IllegalInstruction;
      }
   }

   return T_Ok;
}

void VM_dump_registers(VM* self) {
   mcu_assert(self != nullptr, "self can't be null");

   println("registers");
   println("===============================================");
   printf("General Purpose Registers");

   for (usize i = 0; i < GPR_REG_COUNT; ++i) {
      if (i % 3 == 0) printf("\n");
      printf("r%-2zu: %-8zu", i, self->registers.gpr[i]);
   }

   if (GPR_REG_COUNT % 3 == 0) printf("\n");
   printf("ip : %zu        ", self->registers.ip);

   printf("\n");
   println("===============================================");
}

const cstr Trap_to_cstr(Trap self) {
   switch (self) {
      case T_Ok:                        return "Ok";
      case T_Halt:                      return "Halt";
      case T_IllegalRegister:           return "IllegalRegister";
      case T_IllegalInstruction:        return "IllegalInstruction";
      case T_IllegalInstructionVariant: return "IllegalInstructionVariant";
      case T_IllegalAddress:            return "IllegalAddress";
      default:                          return "Unknown";
   }
}

