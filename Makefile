.PHONY: 

configure: builddir

install: installdir

test: installdir
	./installdir/bin/test

build: builddir
	cmake --build builddir

installdir: build
	cmake --install builddir

builddir:
	cmake -S . -B builddir -DCMAKE_INSTALL_PREFIX="$(shell pwd)/installdir/"

clean:
	rm -rf builddir/
	rm -rf installdir/