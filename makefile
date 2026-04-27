.ONESHELL:
.PHONY: vm

vm: setup
	cd vm
	make vm

setup:
	mkdir -p build/bin
