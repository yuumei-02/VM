#pragma once

#include <mcu/types.h>

// @todo: Make a more sensical encoding
typedef enum : u16 {
   // Interrups
   OC_Halt,

   // Memory
   OC_Mov,

   // Arithmatic
   OC_Add,
   OC_Min,
   OC_Mul,
   OC_Div,

   // Jumps
   OC_Jmp,
} OpCode;

/// [R] register [V] value    [M] memory addr
/// [A] absolute [E] relative
typedef enum : u8 {
   OV_RR,
   OV_RV,

   // For jump instructions
   OV_RA, OV_RE,
   OV_VA, OV_VE
} OpVariant;

typedef struct {
   OpCode op_code;
   OpVariant variant;
   u8 operand;
} __attribute__((packed)) Instr;

void Instr_debug_print(Instr self);
const cstr OpCode_to_cstr(OpCode self);

