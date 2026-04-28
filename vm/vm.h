#pragma once

#include <mcu/types.h>

#include "instr.h"

#define GPR_REG_COUNT 12

typedef u64 Word;

typedef struct {
   u8* memory;
   usize memory_size;

   struct {
      Word gpr[GPR_REG_COUNT];
      Word ip; // Instruction pointer
      // @reference: https://en.wikipedia.org/wiki/FLAGS_register
      Word fr; // Flag register
   } registers;
} VM;

typedef enum : u32 {
   T_Ok,
   T_Halt,
   
   T_IllegalRegister,
   T_IllegalInstruction,
   T_IllegalInstructionVariant,
   T_IllegalAddress,

   T_DivisionByZero,
} Trap;

const cstr Trap_to_cstr(Trap self);

/// [memory_size] is in bytes
VM VM_new(usize memory_size);
void VM_free(VM* self);

bool VM_load_program_from_array(VM* self, Instr* program, usize program_size);
Trap VM_perform_cycle(VM* self);
void VM_dump_registers(VM* self);

