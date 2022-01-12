build:
	clang -std=c17 main.c camera.c hitablelist.c material.c sphere.c -lpthread -Wall -O2 -o xtiny-raytracer

debug:
	clang main.c camera.c hitablelist.c material.c sphere.c -lpthread -lm -Wall -g -fsanitize=address -o xtiny-raytracer -D DEBUG

render:
	./xtiny-raytracer > image.ppm

clean:
	rm -rf xtiny-raytracer xtiny-raytracer.*