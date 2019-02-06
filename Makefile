build:
	gcc main.c camera.c hitablelist.c material.c sphere.c -lm -Wall -O3 -march=native -o xtiny-raytracer

render:
	./xtiny-raytracer > image.ppm