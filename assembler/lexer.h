#include <mcu/core.h>
#include <mcu/io.h>

#include "vm/instr.h"

typedef enum {
   // Miscellaneous
   TT_Eof,

   // Single char
   TT_Colon, TT_Comma,

   // Literals
   TT_Identifier,
   TT_IntLiteral,
   TT_BadToken,

   // Keywords
   TT_Instruction,
} TokenType;

typedef struct {
   usize x;
   usize y;
   usize length;
   
   TokenType type;
   union {
      String str_literal;
      i64 int_literal;
      Instr instr;
   };
} Token;

typedef enum {
   LM_Trim,
   LM_Normal,
   LM_Integer,
   LM_Comment
} LexerMode;

typedef struct {
   usize x;
   usize y;

   i32 current;
   i32 peek;
   LexerMode mode;
   String accumulated;

   const cstr path;
   FILE* handle;
} Lexer;

void Token_free(Token self);

const cstr TokenType_to_cstr(TokenType self);
void Token_print(const cstr path, Token self);

/// [failure] is allowed to be [null]
Lexer Lexer_new(const cstr path, bool* failure);
void Lexer_free(Lexer* self);

Token Lexer_next(Lexer* self, bool* failure);

