.ONESHELL:
.PHONY: vm

compiler := gcc
cflags := -Wall -Wextra -pedantic -Wswitch-enum -std=c23
clibs := -lmcu-debug

vm:
	$(compiler) $(cflags) *.c -o ../build/bin/vm $(clibs)

