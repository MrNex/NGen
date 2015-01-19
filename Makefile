all: NGen

NGen:
	gcc -c -std=c99 ./Math/Vector.c -o ./Math/Vector.o
	gcc -c -std=c99 ./Math/Matrix.c -o ./Math/Matrix.o
	gcc -c -std=c99  -lfreeglut -lopengl32 main.c
	gcc -o NGen -std=c99 main.o ./Math/Vector.o ./Math/Matrix.o  -lfreeglut -lopengl32
