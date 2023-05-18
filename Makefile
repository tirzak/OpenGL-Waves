
all: tess

tess:
	g++ camera.cpp L21.cpp -std=c++11 -lglfw -lGLEW -lOpenGL -I/home/tirzak/Downloads/glm -o waves.out

clean:
	rm -f waves.out
