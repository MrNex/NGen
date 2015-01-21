all: NGen

NGen:
	gcc -std=c99 -c ./Math/Vector.c -o ./Math/Vector.o
	gcc -std=c99 -c ./Math/Matrix.c -o ./Math/Matrix.o
	
	gcc -std=c99 -c ./Render/Mesh.c -o ./Render/Mesh.o 

	gcc -std=c99 -c main.c

	gcc -o NGen main.o ./Render/Mesh.o ./Math/Vector.o ./Math/Matrix.o -lglew32 -lfreeglut -lglu32 -lopengl32
