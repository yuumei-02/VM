#include <mcu/core.h>
#include <mcu/io.h>

#include <string.h>
#include <errno.h>

#include "lexer.h"

const cstr TokenType_to_cstr(TokenType self) {
   switch (self) {
      case TT_Eof: return "Eof";
      
      case TT_Label:      return "Label";
      case TT_IntLIteral: return "IntLiteral";

      case TT_Instruction: return "Instruction";
      default: return "Unknown";
   }
}

void Token_print(const cstr path, Token self) {
   printf("%s:%zu:%zu:%zu: %s", path, self.y, self.x, self.length, TokenType_to_cstr(self.type));

   switch (self.type) {
      case TT_Label:      println(" (%s)", self.str_literal.chars); break;
      case TT_IntLiteral: println(" (%ld)", self.int_literal);      break;

      case TT_Instruction: println(" (%s)", OpCode_to_cstr(self.instr.op_code)); break;

      case TT_Eof: [[fallthrough]];
      default: {
         printf("\n");
      }
   }
}

Lexer Lexer_new(const cstr path, bool* failure) {
   mcu_assert(path != nullptr, "path can't be null");

   Lexer self = {
      .x = 0,
      .y = 1,
      .mode = LM_Trim,
      .accumulated = String_new(),
      .path = path
   };

   self.handle = fopen(path, "r");
   if (self.handle == nullptr) {
      eprintln("Failed to open file \"%s\", reason: \"%s\"", path, strerror(errno));
      goto failure;
   }

   self.peek = fgetc(self.handle);
   if (self.peek == EOF && ferror(self.handle)) {
      eprintln("Failed to read from file \"%s\", reason: \"%s\"", path, strerror(errno));
      goto failure;
   }

   if (failure != nullptr) *failure = false;
   return self;

failure:
   if (failure != nullptr) *failure = true;
   return self;
}

void Lexer_free(Lexer* self) {
   mcu_assert(self != nullptr, "self can't be null");

   String_free(&self->accumulated);
   if (fclose(self->handle) == EOF) {
      eprintln("Failed to close file \"%s\", reason: \"%s\"", self->path, strerror(errno));
   }
   
   *self = (Lexer) {0};
}

bool Lexer_advance(Lexer* self) {
   if (self->current == '\n') {
      self->x = 1;
      self->y += 1;
   } else {
      self->x += 1;
   }

   self->current = self->peek;
   self->peek = fgetc(self->handle);

   if (self->peek == EOF && ferror(self->handle)) {
      eprintln("Failed to read from file \"%s\", reason: \"%s\"", self->path, strerror(errno));
      return true;
   }

   return false;
}

Token Lexer_next(Lexer* self) {
   mcu_assert(self != nullptr, "self can't be null");

   self->mode = LM_Trim;
   String_clear(&self->accumulated);
}

