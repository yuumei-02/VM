#pragma once

#include <mcu/types.h>

typedef enum : u16 {
   OC_Halt = 0b0000'0000'0000'0000,
   OC_Mov  = 0b0000'0000'1000'0000,
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

