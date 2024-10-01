.PHONY: clean configure build install gtk test re
.PHONY: r c b i

ENV = $(shell cat env.txt)

gtk:
	${ENV} ./install/bin/maze_gtk
test:
	${ENV} ./install/bin/test


r: clean
c: configure
b: build
i: install

re: clean configure build

clean:
	rm -rf build/
	rm -rf install/

configure:
	cmake -S . -B build -DCMAKE_INSTALL_PREFIX="$(shell pwd)/install/"

build:
	cmake --build build --parallel

install:
	cmake --install build