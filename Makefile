.PHONY: clean configure build install gtk test
.PHONY: r c b i

ENV = $(shell cat env.txt)

gtk:
	${ENV} ./install/bin/s21_ussur_mazegtk
test:
	${ENV} ./install/bin/s21_ussur_maze_test
mem:
	${ENV} valgrind --tool=memcheck --suppressions="/usr/share/gtk-3.0/valgrind/gtk.supp" --suppressions="/usr/share/glib-2.0/valgrind/glib.supp" ./install/bin/s21_ussur_mazegtk

r: clean
c: configure
b: build
i: install

clean:
	rm -rf build/
	rm -rf install/

configure:
	cmake -S . -B build -DCMAKE_INSTALL_PREFIX="$(shell pwd)/install/"

build:
	cmake --build build --parallel

install:
	cmake --install build
