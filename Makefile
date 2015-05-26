##
#Working through State Implementations

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra
LIBS=-lm -lGLEW -lglut -lGLU -lGL

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
	Bin/ShaderProgram.o \
	Bin/Camera.o \
	Bin/State.o \
	Bin/ApplyForce.o \
	Bin/CharacterController.o \
	Bin/FirstPersonCamera.o \
	Bin/MeshSpring.o \
	Bin/MeshSwap.o \
	Bin/Remove.o \
	Bin/Reset.o \
	Bin/Revolve.o \
	Bin/Rotate.o \
	Bin/RotateCoordinateAxis.o \
	Bin/Score.o \
	Bin/Spring.o \
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
	Bin/main.o

all: NGen

NGen: $(OBJ)
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

##
#State
Bin/State.o: State/State.c State/State.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ApplyForce.o: State/ApplyForce.c State/ApplyForce.h Bin/State.o Bin/GObject.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/CharacterController.o: State/CharacterController.c State/CharacterController.h Bin/State.o Bin/GObject.o Bin/InputManager.o Bin/RenderingManager.o Bin/AssetManager.o Bin/TimeManager.o Bin/PhysicsManager.o Bin/ObjectManager.o Bin/Remove.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/FirstPersonCamera.o: State/FirstPersonCamera.c State/FirstPersonCamera.h Bin/State.o Bin/InputManager.o Bin/RenderingManager.o Bin/TimeManager.o Bin/PhysicsManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/MeshSwap.o: State/MeshSwap.c State/MeshSwap.h Bin/State.o Bin/GObject.o Bin/InputManager.o Bin/AssetManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/MeshSpring.o: State/MeshSpring.c State/MeshSpring.h Bin/State.o Bin/Mesh.o Bin/TimeManager.o Bin/InputManager.o Bin/DynamicArray.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Remove.o: State/Remove.c State/Remove.h Bin/State.o Bin/GObject.o Bin/TimeManager.o Bin/ObjectManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Reset.o: State/Reset.c State/Reset.h Bin/State.o Bin/GObject.o Bin/TimeManager.o Bin/ObjectManager.o Bin/CollisionManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Revolve.o: State/Revolve.c State/Revolve.h Bin/State.o Bin/GObject.o Bin/Vector.o Bin/TimeManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Rotate.o: State/Rotate.c State/Rotate.h Bin/State.o Bin/GObject.o Bin/Vector.o Bin/TimeManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RotateCoordinateAxis.o: State/RotateCoordinateAxis.c State/RotateCoordinateAxis.h Bin/State.o Bin/GObject.o Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Score.o: State/Score.c State/Score.h Bin/State.o Bin/GObject.o Bin/CollisionManager.o Bin/TimeManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Spring.o: State/Spring.c State/Spring.h Bin/State.o Bin/GObject.o
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
Bin/GObject.o: GObject/GObject.c GObject/GObject.h Bin/FrameOfReference.o Bin/Mesh.o Bin/Texture.o Bin/State.o Bin/RigidBody.o Bin/Collider.o
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
#Main
Bin/main.o: main.c Bin/Mesh.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)


##
#Clean
clean:
	rm -f $(OBJ) NGen
