.PHONY: clean configure build install gtk test re
.PHONY: r c b i

ENV = $(shell cat env.txt)

gtk:
	${ENV} ./install/bin/s21_ussur_mazegtk
test:
	${ENV} ./install/bin/s21_ussur_maze_test


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