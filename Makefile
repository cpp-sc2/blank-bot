build: config
	cmake --build build --parallel 6

ladder: ladder-config
	cmake --build build --parallel 6

config:
	cmake -B build -DSC2_VERSION=4.10.0

ladder-config:
	cmake -B build -DBUILD_FOR_LADDER=ON -DSC2_VERSION=4.10.0

clean:
	rm -rf build/

civetclean:
	rm -rf build/_deps/civetweb-subbuild

upload: ladder
	python3 upload.py