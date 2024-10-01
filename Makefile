.PHONY: 

configure: builddir

install: installdir

build: builddir
	cmake --build builddir

installdir: build
	cmake --install builddir

builddir:
	cmake -S . -B builddir -DCMAKE_INSTALL_PREFIX="$(shell pwd)/installdir/"

clean:
	rm -rf builddir/
	rm -rf installdir/