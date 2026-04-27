#pragma once

#include <mcu/types.h>

// @todo: Make a more sensical encoding
typedef enum : u16 {
   // Interrups
   OC_Halt = 0b0000'0000'0000'0000,

   // Memory
   OC_Mov  = 0b0000'0000'1000'0000,

   // Arithmatic
   OC_Add  = 0b0000'0000'0001'0000,
   OC_Min  = 0b0000'0000'0001'0001,
   OC_Mul  = 0b0000'0000'0001'0010,
   OC_Div  = 0b0000'0000'0001'0011,
} OpCode;

/// [R] register
/// [V] value
/// [M] memory addr
typedef enum : u8 {
   OV_RR,
   OV_RV,
} OpVariant;

typedef struct {
   OpCode op_code;
   OpVariant variant;
   u8 operand;
} __attribute__((packed)) Instr;

