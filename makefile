.ONESHELL:
.PHONY: vm assembler

vm: setup
	cd vm
	make vm

assembler: setup
	cd assembler
	make assembler

setup:
	mkdir -p build/bin
