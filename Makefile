CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra -g -pg
LIBS=-lm

ifeq ($(OS),Windows_NT)
	LIBS += -lglew32 -lfreeglut -lglu32 -lopengl32
else
	LIBS += -lGLEW -lglut -lGLU -lGL -lOpenCL
endif

STATES_C=$(wildcard State/*.c)
STATES_H=$(wildcard State/*.h)
STATES_O=$(addprefix Bin/State/,$(notdir $(STATES_C:.c=.o)))

MANAGERS_O= \
	Bin/EnvironmentManager.o \
	Bin/KernelManager.o \
	Bin/ObjectManager.o \
	Bin/AssetManager.o \
	Bin/CollisionManager.o \
	Bin/PhysicsManager.o \
	Bin/RenderingManager.o \
	Bin/InputManager.o \
	Bin/TimeManager.o \
	Bin/SystemManager.o

OBJ= \
	Bin/Vector.o \
	Bin/Matrix.o \
	Bin/Multivector.o\
	Bin/LinkedList.o \
	Bin/DynamicArray.o \
	Bin/Hash.o \
	Bin/HashMap.o \
	Bin/OctTree.o \
	Bin/MemoryPool.o \
	Bin/InputManager.o \
	Bin/FrameOfReference.o \
	Bin/Mesh.o \
	Bin/Image.o \
	Bin/Texture.o \
	Bin/Material.o \
	Bin/Light.o \
	Bin/DirectionalLight.o \
	Bin/PointLight.o \
	Bin/ShaderProgram.o \
	Bin/ForwardShaderProgram.o \
	Bin/DeferredGeometryShaderProgram.o \
	Bin/DeferredDirectionalShaderProgram.o \
	Bin/DeferredPointShaderProgram.o \
	Bin/DeferredStencilShaderProgram.o \
	Bin/RayTracerGeometryShaderProgram.o \
	Bin/RayTracerShadowShaderProgram.o \
	Bin/RayTracerPointShaderProgram.o \
	Bin/RayTracerStencilShaderProgram.o \
	Bin/RayTracerDirectionalShaderProgram.o \
	Bin/RayTracerGlobalShaderProgram.o \
	Bin/Camera.o \
	Bin/GeometryBuffer.o \
	Bin/RayBuffer.o \
	Bin/GlobalBuffer.o \
	Bin/RenderPipeline.o \
	Bin/ForwardRenderPipeline.o \
	Bin/DeferredRenderPipeline.o \
	Bin/RayTracerRenderPipeline.o \
	Bin/KernelProgram.o \
	Bin/RayTracerDirectionalShadowKernelProgram.o \
	Bin/RayTracerPointShadowKernelProgram.o \
	Bin/RayTracerReflectionKernelProgram.o \
	Bin/RayTracerTransmissionKernelProgram.o \
	Bin/RayTracerKernelProgram.o \
	Bin/ToneReproductionKernelProgram.o \
	Bin/RigidBody.o \
	Bin/SphereCollider.o \
	Bin/AABBCollider.o \
	Bin/ConvexHullCollider.o \
	Bin/RayCollider.o \
	Bin/Collider.o \
	Bin/GObject.o \
	Bin/Loader.o \
	Bin/ProgramUniform.o \
	Bin/KernelManager.o \
	Bin/EnvironmentManager.o \
	Bin/ObjectManager.o \
	Bin/RenderingManager.o \
	Bin/AssetManager.o \
	Bin/TimeManager.o \
	Bin/CollisionManager.o \
	Bin/PhysicsManager.o \
	Bin/SystemManager.o \
	Bin/Implementation.o \
	Bin/main.o

all: NGen

NGen: $(OBJ) $(STATES_O)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

##
#Math
#Bin/Compute.o: Math/Compute.c Math/Compute.h
#	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Vector.o: Math/Vector.c Math/Vector.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/Matrix.o: Math/Matrix.c Math/Matrix.h Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

#Bin/KBlade.o: Math/KBlade.c Math/KBlade.h Bin/Compute.o Bin/Multivector.o
#	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS);

Bin/Multivector.o: Math/Multivector.c Math/Multivector.h
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
#Memory
Bin/MemoryPool.o: Data/MemoryPool.c Data/MemoryPool.h Bin/DynamicArray.o Bin/LinkedList.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Rendering
Bin/FrameOfReference.o: Render/FrameOfReference.c Render/FrameOfReference.h Bin/Matrix.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ShaderProgram.o: Render/ShaderProgram.c Render/ShaderProgram.h
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

Bin/Light.o: Render/Light.c Render/Light.h Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DirectionalLight.o: Render/DirectionalLight.c Render/DirectionalLight.h Bin/Light.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/PointLight.o: Render/PointLight.c Render/PointLight.h Bin/Light.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/GeometryBuffer.o: Render/GeometryBuffer.c Render/GeometryBuffer.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayBuffer.o: Render/RayBuffer.c Render/RayBuffer.h Bin/KernelManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/GlobalBuffer.o: Render/GlobalBuffer.c Render/GlobalBuffer.h Bin/KernelManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ForwardShaderProgram.o: Render/ForwardShaderProgram.c Render/ForwardShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DeferredGeometryShaderProgram.o: Render/DeferredGeometryShaderProgram.c Render/DeferredGeometryShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DeferredDirectionalShaderProgram.o: Render/DeferredDirectionalShaderProgram.c Render/DeferredDirectionalShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/EnvironmentManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DeferredPointShaderProgram.o: Render/DeferredPointShaderProgram.c Render/DeferredPointShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/PointLight.o Bin/AssetManager.o Bin/EnvironmentManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DeferredStencilShaderProgram.o: Render/DeferredStencilShaderProgram.c Render/DeferredStencilShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/ProgramUniform.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)




Bin/RayTracerGeometryShaderProgram.o: Render/RayTracerGeometryShaderProgram.c Render/RayTracerGeometryShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/EnvironmentManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerShadowShaderProgram.o: Render/RayTracerShadowShaderProgram.c Render/RayTracerShadowShaderProgram.h Bin/CollisionManager.o Bin/Collider.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerDirectionalShaderProgram.o: Render/RayTracerDirectionalShaderProgram.c Render/RayTracerDirectionalShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/EnvironmentManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerPointShaderProgram.o: Render/RayTracerPointShaderProgram.c Render/RayTracerPointShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/EnvironmentManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerStencilShaderProgram.o: Render/RayTracerStencilShaderProgram.c Render/RayTracerStencilShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/ProgramUniform.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerGlobalShaderProgram.o: Render/RayTracerGlobalShaderProgram.c Render/RayTracerGlobalShaderProgram.h Bin/ShaderProgram.o Bin/RenderingManager.o Bin/AssetManager.o Bin/EnvironmentManager.o Bin/ProgramUniform.o Bin/GlobalBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RenderPipeline.o: Render/RenderPipeline.c Render/RenderPipeline.h Bin/ShaderProgram.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ForwardRenderPipeline.o: Render/ForwardRenderPipeline.c Render/ForwardRenderPipeline.h Bin/RenderPipeline.o Bin/LinkedList.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/DeferredRenderPipeline.o: Render/DeferredRenderPipeline.c Render/DeferredRenderPipeline.h Bin/RenderPipeline.o Bin/GeometryBuffer.o Bin/LinkedList.o Bin/EnvironmentManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerRenderPipeline.o: Render/RayTracerRenderPipeline.c Render/RayTracerRenderPipeline.h Bin/RenderPipeline.o Bin/GeometryBuffer.o Bin/LinkedList.o Bin/EnvironmentManager.o Bin/RayTracerDirectionalShadowKernelProgram.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Device
Bin/KernelProgram.o: Device/KernelProgram.c Device/KernelProgram.h Bin/KernelManager.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerDirectionalShadowKernelProgram.o: Device/RayTracerDirectionalShadowKernelProgram.c Device/RayTracerDirectionalShadowKernelProgram.h Bin/KernelProgram.o Bin/RenderingManager.o Bin/KernelManager.o Bin/CollisionManager.o Bin/RayBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerPointShadowKernelProgram.o: Device/RayTracerPointShadowKernelProgram.c Device/RayTracerPointShadowKernelProgram.h Bin/KernelProgram.o Bin/RenderingManager.o Bin/KernelManager.o Bin/CollisionManager.o Bin/RayBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerReflectionKernelProgram.o: Device/RayTracerReflectionKernelProgram.c Device/RayTracerReflectionKernelProgram.h Bin/KernelProgram.o Bin/RenderingManager.o Bin/KernelManager.o Bin/CollisionManager.o Bin/EnvironmentManager.o Bin/RayBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerTransmissionKernelProgram.o: Device/RayTracerTransmissionKernelProgram.c Device/RayTracerTransmissionKernelProgram.h Bin/KernelProgram.o Bin/RenderingManager.o Bin/KernelManager.o Bin/CollisionManager.o Bin/AssetManager.o Bin/RayBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RayTracerKernelProgram.o: Device/RayTracerKernelProgram.c Device/RayTracerKernelProgram.h Bin/KernelProgram.o Bin/RenderingManager.o Bin/KernelManager.o Bin/CollisionManager.o Bin/AssetManager.o Bin/RayBuffer.o Bin/GlobalBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ToneReproductionKernelProgram.o: Device/ToneReproductionKernelProgram.c Device/ToneReproductionKernelProgram.h Bin/KernelProgram.o Bin/GlobalBuffer.o
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

Bin/RayCollider.o: Collision/RayCollider.c Collision/RayCollider.h 
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
#Compatibility
Bin/ProgramUniform.o: Compatibility/ProgramUniform.c Compatibility/ProgramUniform.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

##
#Managers
Bin/InputManager.o: Manager/InputManager.c Manager/InputManager.h Bin/Vector.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/ObjectManager.o: Manager/ObjectManager.c Manager/ObjectManager.h Bin/LinkedList.o Bin/GObject.o Bin/OctTree.o Bin/HashMap.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/RenderingManager.o: Manager/RenderingManager.c Manager/RenderingManager.h Bin/ObjectManager.o Bin/ForwardShaderProgram.o Bin/Camera.o Bin/GObject.o Bin/LinkedList.o Bin/GeometryBuffer.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/AssetManager.o: Manager/AssetManager.c Manager/AssetManager.h Bin/HashMap.o Bin/Mesh.o Bin/Texture.o Bin/Loader.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/TimeManager.o: Manager/TimeManager.c Manager/TimeManager.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/CollisionManager.o: Manager/CollisionManager.c Manager/CollisionManager.h Bin/GObject.o Bin/LinkedList.o Bin/OctTree.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/PhysicsManager.o: Manager/PhysicsManager.c Manager/PhysicsManager.h Bin/CollisionManager.o Bin/GObject.o Bin/DynamicArray.o Bin/LinkedList.o
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/SystemManager.o: Manager/SystemManager.c Manager/SystemManager.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/EnvironmentManager.o: Manager/EnvironmentManager.c Manager/EnvironmentManager.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

Bin/KernelManager.o: Manager/KernelManager.c Manager/KernelManager.h Bin/KernelProgram.o
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
