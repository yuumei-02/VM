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

bool invalid_reg(u8 reg) {
   return reg >= GPR_REG_COUNT;
}

/// Returns [null] on invalid addr
Word* fetch_advance_next_word(VM* vm) {
   if (vm->registers.ip >= vm->memory_size)
      return nullptr;

   Word* word = (Word*) (vm->memory + vm->registers.ip);
   vm->registers.ip += sizeof(Word);

   return word;
}

Trap VM_perform_cycle(VM* self) {
   mcu_assert(self != nullptr, "self can't be null");

   if (self->registers.ip >= self->memory_size) {
      return T_IllegalAddress;
   }
   
   Instr instr = VM_get_instr(self);
   self->registers.ip += 4;

   #define fetch_check_rr() \
      u8 src  = instr.operand & 0x0F;            \
      u8 dest = (instr.operand >> 4) & 0x0F;     \
      if (invalid_reg(src) || invalid_reg(dest)) \
         return T_IllegalRegister;

   #define fetch_check_rv() \
      if (invalid_reg(instr.operand))              \
         return T_IllegalRegister;                 \
      Word* value = fetch_advance_next_word(self); \
      if (value == nullptr)                        \
         return T_IllegalAddress;

   #define fetch_check_va() \
      Word* value = fetch_advance_next_word(self);         \
      if (value == nullptr || *value >= self->memory_size) \
         return T_IllegalAddress;

   // @todo: Signed versions of the instructions
   #define fetch_check_ve() \
      Word* value = fetch_advance_next_word(self);                        \
      if (value == nullptr)                                               \
         return T_IllegalAddress;                                         \
      Word new_addr = (self->registers.ip - (4 + sizeof(Word))) + *value; \
      if (new_addr >= self->memory_size)                                  \
         return T_IllegalAddress;

   #define fetch_check_ra() \
      if (invalid_reg(instr.operand) || self->registers.gpr[instr.operand] >= self->memory_size) \
         return T_IllegalAddress;

   #define fetch_check_re() \
      if (invalid_reg(instr.operand))                                                \
         return T_IllegalAddress;                                                    \
      Word new_addr = (self->registers.ip - 4) + self->registers.gpr[instr.operand]; \
      if (new_addr >= self->memory_size)                                             \
         return T_IllegalAddress;
         
   Instr_debug_print(instr);
   switch (instr.op_code) {
      // Interrups
      case OC_Halt: {
         return T_Halt;
      }

      // Memory
      case OC_Mov: {
         switch (instr.variant) {
            case OV_RR: {
               fetch_check_rr();
               self->registers.gpr[dest] = self->registers.gpr[src];
            } break;

            case OV_RV: {
               fetch_check_rv();
               self->registers.gpr[instr.operand] = *value;
            } break;

            case OV_RA: [[fallthrough]];
            case OV_RE: [[fallthrough]];
            case OV_VA: [[fallthrough]];
            case OV_VE: [[fallthrough]];
            default: {
               return T_IllegalInstructionVariant;
            }
         }
      } break;

      // Arithmatic
      case OC_Add: {
         switch (instr.variant) {
            case OV_RR: {
               fetch_check_rr();
               self->registers.gpr[dest] += self->registers.gpr[src];
            } break;

            case OV_RV: {
               fetch_check_rv();
               self->registers.gpr[instr.operand] += *value;
            } break;

            case OV_RA: [[fallthrough]];
            case OV_RE: [[fallthrough]];
            case OV_VA: [[fallthrough]];
            case OV_VE: [[fallthrough]];
            default: {
               return T_IllegalInstructionVariant;
            }
         }
      } break;
      
      case OC_Min: {
         switch (instr.variant) {
            case OV_RR: {
               fetch_check_rr();
               self->registers.gpr[dest] -= self->registers.gpr[src];
            } break;

            case OV_RV: {
               fetch_check_rv();
               self->registers.gpr[instr.operand] -= *value;
            } break;

            case OV_RA: [[fallthrough]];
            case OV_RE: [[fallthrough]];
            case OV_VA: [[fallthrough]];
            case OV_VE: [[fallthrough]];
            default: {
               return T_IllegalInstructionVariant;
            }
         }
      } break;
      
      case OC_Mul: {
         switch (instr.variant) {
            case OV_RR: {
               fetch_check_rr();
               self->registers.gpr[dest] *= self->registers.gpr[src];
            } break;

            case OV_RV: {
               fetch_check_rv();
               self->registers.gpr[instr.operand] *= *value;
            } break;

            case OV_RA: [[fallthrough]];
            case OV_RE: [[fallthrough]];
            case OV_VA: [[fallthrough]];
            case OV_VE: [[fallthrough]];
            default: {
               return T_IllegalInstructionVariant;
            }
         }
      } break;
      
      case OC_Div: {
         switch (instr.variant) {
            case OV_RR: {
               fetch_check_rr();
               if (self->registers.gpr[src] == 0)
                  return T_DivisionByZero;
                  
               self->registers.gpr[dest] /= self->registers.gpr[src];
            } break;

            case OV_RV: {
               fetch_check_rv();
               if (self->registers.gpr[*value] == 0)
                  return T_DivisionByZero;

               self->registers.gpr[instr.operand] /= *value;
            } break;

            case OV_RA: [[fallthrough]];
            case OV_RE: [[fallthrough]];
            case OV_VA: [[fallthrough]];
            case OV_VE: [[fallthrough]];
            default: {
               return T_IllegalInstructionVariant;
            }
         }
      } break;

      // Jumps
      case OC_Jmp: {
         switch (instr.variant) {
            case OV_RR: [[fallthrough]];
            case OV_RV: [[fallthrough]];

            case OV_RA: {
               fetch_check_ra();
               self->registers.ip = self->registers.gpr[instr.operand];
            } break;
            
            case OV_RE: {
               fetch_check_re();
               self->registers.ip = new_addr;
            } break;
            
            case OV_VA: {
               fetch_check_va();
               self->registers.ip = *value;
            } break;
            
            case OV_VE: {
               fetch_check_ve();
               self->registers.ip = new_addr;
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
      case T_Ok:   return "Ok";
      case T_Halt: return "Halt";
      
      case T_IllegalRegister:           return "IllegalRegister";
      case T_IllegalInstruction:        return "IllegalInstruction";
      case T_IllegalInstructionVariant: return "IllegalInstructionVariant";
      case T_IllegalAddress:            return "IllegalAddress";
      
      case T_DivisionByZero: return "DivisionByZero";
      default:               return "Unknown";
   }
}

