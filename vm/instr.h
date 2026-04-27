#pragma once

#include <mcu/types.h>

// @todo: Make a more sensical encoding
typedef enum : u16 {
   // Interrups
   OC_Halt = 0b0000'0000'0000'0000,

   // Memory
   OC_Mov = 0b0000'1000'0000'0000,

   // Arithmatic
   OC_Add = 0b0000'0100'0000'0000,
   OC_Min = 0b0000'0100'0000'0001,
   OC_Mul = 0b0000'0100'0000'0010,
   OC_Div = 0b0000'0100'0000'0011,

   // Jumps
   OC_Jmp = 0b0000'0010'0000'0000,
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

