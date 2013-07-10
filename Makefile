BIN = ./bin/
SRC = ./src/

CC = g++
CFLAGS =

IPATH = -I/usr/pkg/include/GL -I./dev/SDL/include -I./include
LPATH = -L./dev/SDL -lSDL
LDPATH = -Wl,-R./dev/SDL 

RM = rm


all: $(BIN)MockingBird

$(BIN)MockingBird: $(BIN)MockingBird.o $(BIN)ObjReader.o $(BIN)BananaClient.o
	$(CC) -o $@ $^ -lm -lGL -lglut -lGLU $(LPATH) $(LDPATH)
	
$(BIN)%.o: $(SRC)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)

clean:
	$(RM) -f $(BIN)*.o $(BIN)*.gch $(BIN)MockingBird

