#include <mcu/core.h>
#include <mcu/handlers.h>

#include "lexer.h"

i32 assemble(const cstr path) {
   bool failure;
   Lexer lexer = Lexer_new(path, &failure);
   if (failure) return 1;

   Lexer_free(&lexer);
   return 0;
}

i32 main(i32 argc, cstr argv[]) {
   if (argc < 2) {
      eprintln("[!] Missing input file");
      return 1;
   }

   for (i32 i = 1; i < argc; ++i) {
      if (assemble(argv[i])) {
         return 1;
      }
   }

   return 0;
}

