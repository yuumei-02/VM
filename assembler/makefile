.ONESHELL:
.PHONY: assembler

compiler := gcc
cflags := -Wall -Wextra -pedantic -Wswitch-enum -std=c23 -I../
clibs := -lmcu-debug

assembler:
	$(compiler) $(cflags) *.c -o ../build/bin/assembler $(clibs)

