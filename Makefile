CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra
LIBS=-lm -lGLEW -lglut -lGLU -lGL

OBJ= \
	Bin/Vector.o \
	Bin/Matrix.o \
	Bin/Mesh.o \
	Bin/main.o

all: NGen

NGen: $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LIBS)
##
#Math
Bin/Vector.o: Math/Vector.c Math/Vector.h
	$(CC) $(CFLAGS) -c $< -o $@

Bin/Matrix.o: Math/Matrix.c Math/Matrix.h Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@


##
#Rendering
Bin/Mesh.o: Render/Mesh.c Render/Mesh.h Bin/Matrix.o
	$(CC) $(CFLAGS) -c $< -o $@

##
#Main
Bin/main.o: main.c Bin/Mesh.o
	$(CC) $(CFLAGS) -c $< -o $@
