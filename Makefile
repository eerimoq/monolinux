all:
	mkdir -p build
	gcc -Wall -Werror src/main.c -static -o build/init
	scripts/build.sh

run:
	scripts/run.sh

clean:
	rm -rf build
