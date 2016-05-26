remake: clean all

all: pinball

pinball: pinball.cpp ppm.cpp alexR.cpp omarO.cpp hassenS.cpp gameObjects.cpp
	g++ pinball.cpp ppm.cpp alexR.cpp omarO.cpp vector.cpp libggfonts.a hassenS.cpp gameObjects.cpp /usr/lib/x86_64-linux-gnu/libopenal.so /usr/lib/libalut.so -Wall -Wextra -opinball -lX11 -lGL -lGLU -lm -lrt
clean:
	rm -f pinball
	rm -f *.o
