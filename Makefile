CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra -g
LIBS=-lm

ifeq ($(OS),Windows_NT)
	LIBS += -lglew32 -lfreeglut -lglu32 -lopengl32
else
	LIBS += -lGLEW -lglut -lGLU -lGL
endif

STATES_C=$(wildcard State/*.c)
STATES_H=$(wildcard State/*.h)
STATES_O=$(addprefix Bin/State/,$(notdir $(STATES_C:.c=.o)))

MANAGERS_O= \
	Bin/ObjectManager.o \
	Bin/AssetManager.o \
	Bin/CollisionManager.o \
	Bin/PhysicsManager.o \
	Bin/RenderingManager.o \
	Bin/InputManager.o \
	Bin/TimeManager.o

OBJ= \
	Bin/Vector.o \
	Bin/Matrix.o \
	Bin/LinkedList.o \
	Bin/DynamicArray.o \
	Bin/Hash.o \
	Bin/HashMap.o \
	Bin/OctTree.o \
	Bin/InputManager.o \
	Bin/FrameOfReference.o \
	Bin/Mesh.o \
	Bin/Image.o \
	Bin/Texture.o \
	Bin/Material.o \
	Bin/ShaderProgram.o \
	Bin/Camera.o \
	Bin/RigidBody.o \
	Bin/SphereCollider.o \
	Bin/AABBCollider.o \
	Bin/ConvexHullCollider.o \
	Bin/Collider.o \
	Bin/GObject.o \
	Bin/Loader.o \
	Bin/ObjectManager.o \
	Bin/RenderingManager.o \
	Bin/AssetManager.o \
	Bin/TimeManager.o \
	Bin/CollisionManager.o \
	Bin/PhysicsManager.o \
	Bin/Implementation.o \
	Bin/main.o

all: NGen

NGen: $(OBJ) $(STATES_O)
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

##
#Math
Bin/Vector.o: Math/Vector.c Math/Vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Matrix.o: Math/Matrix.c Math/Matrix.h Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Data
Bin/LinkedList.o: Data/LinkedList.c Data/LinkedList.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DynamicArray.o: Data/DynamicArray.c Data/DynamicArray.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/OctTree.o: Data/OctTree.c Data/OctTree.h Bin/DynamicArray.o Bin/HashMap.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Hash.o: Data/Hash.c Data/Hash.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/HashMap.o: Data/HashMap.c Data/HashMap.h Bin/DynamicArray.o Bin/Hash.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Rendering
Bin/FrameOfReference.o: Render/FrameOfReference.c Render/FrameOfReference.h Bin/Matrix.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ShaderProgram.o: Render/ShaderProgram.c Render/ShaderProgram.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Camera.o: Render/Camera.c Render/Camera.h Bin/FrameOfReference.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Mesh.o: Render/Mesh.c Render/Mesh.h Bin/Matrix.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Image.o: Render/Image.c Render/Image.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Texture.o: Render/Texture.c Render/Texture.h Bin/Image.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Material.o: Render/Material.c Render/Material.h Bin/Texture.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#State
Bin/State/%.o: State/%.c State/%.h $(MANAGERS_O)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Physics
Bin/RigidBody.o: Physics/RigidBody.c Physics/RigidBody.h Bin/DynamicArray.o Bin/FrameOfReference.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Collision
Bin/SphereCollider.o: Collision/SphereCollider.c Collision/SphereCollider.h Bin/FrameOfReference.o Bin/Mesh.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/AABBCollider.o: Collision/AABBCollider.c Collision/AABBCollider.h Bin/FrameOfReference.o Bin/Mesh.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ConvexHullCollider.o: Collision/ConvexHullCollider.c Collision/ConvexHullCollider.h Bin/LinkedList.o Bin/DynamicArray.o Bin/AABBCollider.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Collider.o: Collision/Collider.c Collision/Collider.h Bin/ConvexHullCollider.o Bin/SphereCollider.o Bin/AABBCollider.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#GObject
Bin/GObject.o: GObject/GObject.c GObject/GObject.h Bin/FrameOfReference.o Bin/Mesh.o Bin/Texture.o Bin/State/State.o Bin/RigidBody.o Bin/Collider.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Loader
Bin/Loader.o: Load/Loader.c Load/Loader.h Bin/Mesh.o Bin/Image.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Managers
Bin/InputManager.o: Manager/InputManager.c Manager/InputManager.h Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ObjectManager.o: Manager/ObjectManager.c Manager/ObjectManager.h Bin/LinkedList.o Bin/GObject.o Bin/OctTree.o Bin/HashMap.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RenderingManager.o: Manager/RenderingManager.c Manager/RenderingManager.h Bin/ObjectManager.o Bin/ShaderProgram.o Bin/Camera.o Bin/GObject.o Bin/LinkedList.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/AssetManager.o: Manager/AssetManager.c Manager/AssetManager.h Bin/HashMap.o Bin/Mesh.o Bin/Texture.o Bin/Loader.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/TimeManager.o: Manager/TimeManager.c Manager/TimeManager.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/CollisionManager.o: Manager/CollisionManager.c Manager/CollisionManager.h Bin/GObject.o Bin/LinkedList.o Bin/OctTree.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/PhysicsManager.o: Manager/PhysicsManager.c Manager/PhysicsManager.h Bin/CollisionManager.o Bin/GObject.o Bin/DynamicArray.o Bin/LinkedList.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Implementation
Bin/Implementation.o: Implementation/Implementation.c Implementation/Implementation.h $(MANAGERS_O) $(STATES_O)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Main
Bin/main.o: main.c Bin/Mesh.o Bin/Implementation.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)



##
#Clean
clean:
	rm -f $(OBJ) $(STATES_O) NGen
