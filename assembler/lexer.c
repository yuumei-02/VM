#include <mcu/core.h>
#include <mcu/io.h>

#include <string.h>
#include <errno.h>

#include "lexer.h"

void Token_free(Token self) {
   switch (self.type) {
      case TT_Identifier: [[fallthrough]];
      case TT_BadToken: {
         String_free(&self.str_literal);
      } break;
   
      case TT_Eof:         [[fallthrough]];
      case TT_Colon:       [[fallthrough]];
      case TT_Comma:       [[fallthrough]];
      case TT_IntLiteral:  [[fallthrough]];
      case TT_Instruction: [[fallthrough]];
      default: {
         break;
      }
   }
}

const cstr TokenType_to_cstr(TokenType self) {
   switch (self) {
      case TT_Eof: return "Eof";

      case TT_Colon: return "Colon";
      case TT_Comma: return "Comma";
      
      case TT_Identifier: return "Identifier";
      case TT_IntLiteral: return "IntLiteral";
      case TT_BadToken:   return "BadToken";

      case TT_Instruction: return "Instruction";
      default: return "Unknown";
   }
}

void Token_print(const cstr path, Token self) {
   printf("%s:%zu:%zu:%zu: %s", path, self.y, self.x, self.length, TokenType_to_cstr(self.type));

   switch (self.type) {
      case TT_BadToken:   [[fallthrough]];
      case TT_Identifier: println(" (%s)", self.str_literal.chars); break;
      case TT_IntLiteral: println(" (%ld)", self.int_literal);      break;

      case TT_Instruction: println(" (%s)", OpCode_to_cstr(self.instr.op_code)); break;

      case TT_Colon: [[fallthrough]];
      case TT_Comma: [[fallthrough]];
      case TT_Eof:   [[fallthrough]];
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

bool char_is_identifier_allowed(char c) {
   if (c >= '0' && c <= '9') return true;
   if (c == '-' || c == '_') return true;
   if (c >= 'a' && c <= 'z') return true;
   if (c >= 'A' && c <= 'Z') return true;
   return false;
}

Token Lexer_next(Lexer* self, bool* failure) {
   mcu_assert(self != nullptr, "self can't be null");

   self->mode = LM_Trim;
   String_clear(&self->accumulated);

   Token token = {
      .x = self->x,
      .y = self->y,
      .length = 1,
      .type = TT_Eof
   };

   bool int_is_negative = false;

   loop {
      if (Lexer_advance(self)) goto failure;
      if (self->current == EOF) break;

   reparse_char:
      switch (self->mode) {
         case LM_Trim: {
            token.x = self->x;
            token.y = self->y;

            switch (self->current) {
               case ':': token.type = TT_Colon; return token;
               case ',': token.type = TT_Comma; return token;
               
               case '/': {
                  switch (self->peek) {
                     case '/': {
                        if (Lexer_advance(self)) goto failure;
                        self->mode = LM_Comment;
                     } break;
                     
                     default: break;
                  }
               } break;

               case '-': {
                  if (self->peek >= '0' && self->peek <= '9') {
                     token.length += 1;
                     int_is_negative = true;
                     self->mode = LM_Integer;
                  }
               } break;

               case ' ':  break;
               case '\n': break;

               default: {
                  if (self->current >= '0' && self->current <= '9') {
                     int_is_negative = false;
                     self->mode = LM_Integer;
                     goto reparse_char;
                  }
                  
                  if (char_is_identifier_allowed(self->current)) {
                     self->mode = LM_Normal;
                     goto reparse_char;
                  }
               }
            }
         } break;
         
         case LM_Comment: {
            if (self->peek == '\n' || self->peek == EOF) {
               self->mode = LM_Trim;
            }
         } break;

         case LM_Normal: {
            String_append(&self->accumulated, self->current);

            if (!char_is_identifier_allowed(self->peek)) {
               token.str_literal = String_clone(self->accumulated);
               token.length = self->accumulated.length;
               token.type = TT_Identifier;
               return token;
            }
         } break;

         case LM_Integer: {
            token.int_literal *= 10;
            token.int_literal += self->current - '0';

            if (!(self->peek >= '0' && self->peek <= '9')) {
               if (int_is_negative) token.int_literal = -token.int_literal;
               token.type = TT_IntLiteral;
               return token;
            }

            token.length += 1;
         } break;

         default: {
            panic("unreachable");
         }
      }
   }

   if (failure != nullptr) *failure = false;
   return token;

failure:
   if (failure != nullptr) *failure = true;
   return token;
}

