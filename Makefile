CC = g++ 
CFLAGS = 

IPATH = -I/usr/pkg/include/GL -I./include/SDL
LPATH = -L./dev/SDL -L/usr/pkg/lib
LDPATH = -Wl,-R./dev/SDL -lSDL

RM = rm



all: MockingBird

MockingBird: MockingBird.o ObjReader.o
	$(CC) -o $@ $^ -lm -lGL -lglut -lGLU $(LPATH) $(LDPATH)
	
.cpp.o: 
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)

clean:
	$(RM) -f *.o *.gch MockingBird

