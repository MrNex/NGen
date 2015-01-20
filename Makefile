all: NGen

NGen:
	gcc -std=c99 -c ./Math/Vector.c -o ./Math/Vector.o
	gcc -std=c99 -c ./Math/Matrix.c -o ./Math/Matrix.o
	gcc -std=c99 -c  -lfreeglut -lopengl32 main.c
	gcc -o NGen main.o ./Math/Vector.o ./Math/Matrix.o  -lfreeglut -lopengl32
