#include <mcu/core.h>
#include <mcu/io.h>

#include "instr.h"

const cstr OpCode_to_cstr(OpCode self) {
   switch (self) {
      case OC_Halt: return "Halt";
      
      case OC_Mov: return "Mov";

      case OC_Add: return "Add";
      case OC_Min: return "Min";
      case OC_Mul: return "Mul";
      case OC_Div: return "Div";

      case OC_Jmp: return "Jmp";
   
      default: return "unknown";
   }
}

const cstr OpVariant_to_cstr(OpVariant self) {
   switch (self) {
      case OV_RR: return "RR";
      case OV_RV: return "RV";
      
      case OV_RA: return "RA";
      case OV_RE: return "RE";
      case OV_VA: return "VA";
      case OV_VE: return "VE";
      
      default: return "unknown";
   }
}

void Instr_debug_print(Instr self) {
   switch (self.op_code) {
      case OC_Halt: {
         println("%s", OpCode_to_cstr(self.op_code));
      } break;

      case OC_Mov: [[fallthrough]];
      case OC_Add: [[fallthrough]];
      case OC_Min: [[fallthrough]];
      case OC_Mul: [[fallthrough]];
      case OC_Div: [[fallthrough]];
      case OC_Jmp: {
         println("%s<%s> %u",
            OpCode_to_cstr(self.op_code),
            OpVariant_to_cstr(self.variant),
            self.operand);
      } break;
   
      default: {
         println("unknown");
      }
   }
}

