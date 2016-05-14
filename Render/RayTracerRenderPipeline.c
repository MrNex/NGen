#include "RayTracerRenderPipeline.h"

#include <float.h>

#include "../Manager/EnvironmentManager.h"
#include "../Manager/KernelManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/AssetManager.h"

#include "RayTracerGeometryShaderProgram.h"
#include "RayTracerShadowShaderProgram.h"
#include "RayTracerDirectionalShaderProgram.h"
#include "RayTracerPointShaderProgram.h"
#include "RayTracerStencilShaderProgram.h"
#include "RayTracerGlobalShaderProgram.h"

#include "../Device/KernelProgram.h"
//#include "../Device/RayTracerDirectionalShadowKernelProgram.h"
//#include "../Device/RayTracerPointShadowKernelProgram.h"
//#include "../Device/RayTracerReflectionKernelProgram.h"
//#include "../Device/RayTracerTransmissionKernelProgram.h"
#include "../Device/RayTracerKernelProgram.h"
#include "../Device/ToneReproductionKernelProgram.h"

#include "../Data/LinkedList.h"

#include "../GObject/GObject.h"

#include "RayBuffer.h"
#include "GlobalBuffer.h"

typedef struct RayTracerRenderPipeline_Members
{
	RayBuffer* rBuffer;
	//RayBuffer* reflectionBuffer;
	//RayBuffer* transmissionBuffer;
	GlobalBuffer* gBuffer;
	KernelProgram* rayTracerKernelProg;
	KernelProgram* toneReproductionKernelProg;

	//Reduction memory for openCL use to combine results from different colliders
	cl_mem spheres, aabbs;
	cl_mem sPosition, aPosition;
	cl_mem sDiffuse, aDiffuse;
	cl_mem sNormal, aNormal;
	cl_mem sLocalMaterial, aLocalMaterial;
	cl_mem sGlobalMaterial, aGlobalMaterial;
	cl_mem sSpecular, aSpecular;
	cl_mem sShadow, aShadow;
	cl_mem sDepth, aDepth;

	cl_mem cameraPosition;
	cl_mem gObjects;
	cl_mem materials;
	cl_mem sphereTransformations;
} RayTracerRenderPipeline_Members;

///
//Static Declarations
///

///
//Allocates a new set of members for the RayTracerRenderPipeline
//
//Returns:
//	A pointer to an uninitialized RayTracerRenderPipeline_Members
static RenderPipeline_Members* RayTracerRenderPipeline_AllocateMembers(void);

///
//Initializes a set of RayTracerRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of RayTracerRenderPipeline members to
static void RayTracerRenderPipeline_InitializeMembers(RenderPipeline_Members* mems);

///
//Frees memory allocated by the RayTracerRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the RayTracerRenderPipeline_Members to free
static void RayTracerRenderPipeline_FreeMembers(RenderPipeline_Members* mems);

///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the RayTracerRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void RayTracerRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs);

///
//Renders a set of gameobjects given as a memory pool witht he specified render pipeline
//
//Parameters:
//	pipeline: A pointer to the RayTracerRenderPipeline to render with
//	buffer: A pointer to the rendering buffer to render with
//	pool: A pointer to the memory pool of GObjects to render
static void RayTracerRenderPipeline_RenderWithMemoryPool(RenderPipeline* pipeline, struct RenderingBuffer* buffer, MemoryPool* pool);

///
//Function Definitions
///

///
//Intializes a new RayTracerRenderPipeline
//
//Parameters:
//	pipeline: A pointer to the RenderPipeline to initialize as a RayTracerRenderPipeline
void RayTracerRenderPipeline_Initialize(RenderPipeline* pipeline)
{
	RenderPipeline_Initialize(pipeline, 5);

	pipeline->members = RayTracerRenderPipeline_AllocateMembers();
	RayTracerRenderPipeline_InitializeMembers(pipeline->members);

	pipeline->programs[0] = ShaderProgram_Allocate();
	RayTracerGeometryShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->programs[1] = ShaderProgram_Allocate();
	RayTracerDirectionalShaderProgram_Initialize(pipeline->programs[1]);

	pipeline->programs[2] = ShaderProgram_Allocate();
	RayTracerPointShaderProgram_Initialize(pipeline->programs[2]);

	pipeline->programs[3] = ShaderProgram_Allocate();
	RayTracerStencilShaderProgram_Initialize(pipeline->programs[3]);

	pipeline->programs[4] = ShaderProgram_Allocate();
	RayTracerGlobalShaderProgram_Initialize(pipeline->programs[4]);

	pipeline->Render = (RenderPipeline_RenderFunc)RayTracerRenderPipeline_RenderWithMemoryPool;
	pipeline->FreeMembers = RayTracerRenderPipeline_FreeMembers;
}

///
//Allocates a new set of members for the RayTracerRenderPipeline
//
//Returns:
//	A pointer to an uninitialized RayTracerRenderPipeline_Members
static RenderPipeline_Members* RayTracerRenderPipeline_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerRenderPipeline_Members));
}

///
//Initializes a set of RayTracerRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of RayTracerRenderPipeline members to initialize
static void RayTracerRenderPipeline_InitializeMembers(RenderPipeline_Members* mems)
{
	RayTracerRenderPipeline_Members* members = (RayTracerRenderPipeline_Members*)mems;
	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();
	KernelBuffer* kBuffer = KernelManager_GetKernelBuffer();
	
	members->rBuffer = RayBuffer_Allocate();
	RayBuffer_Initialize(members->rBuffer, envBuffer->windowWidth, envBuffer->windowHeight);	
	members->rBuffer->passType[0] = 1;

	members->gBuffer = GlobalBuffer_Allocate();
	GlobalBuffer_Initialize(members->gBuffer, envBuffer->windowWidth, envBuffer->windowHeight);

	members->rayTracerKernelProg = KernelProgram_Allocate();
	RayTracerKernelProgram_Initialize(members->rayTracerKernelProg, kBuffer);

	members->toneReproductionKernelProg = KernelProgram_Allocate();
	ToneReproductionKernelProgram_Initialize(members->toneReproductionKernelProg, kBuffer);

	//Initialize OpenCL reduction memory
	cl_image_format format;
	format.image_channel_order = CL_RGBA;
	format.image_channel_data_type = CL_FLOAT;

	cl_image_desc properties;
	properties.image_type = CL_MEM_OBJECT_IMAGE2D;
	properties.image_width= envBuffer->windowWidth;
	properties.image_height = envBuffer->windowHeight;
	properties.image_depth = 0;
	properties.image_array_size = 0;
	properties.image_row_pitch = 0;
	properties.image_slice_pitch = 0;
	properties.num_mip_levels = 0;
	properties.num_samples = 0;
	properties.mem_object = NULL;

	cl_int err = 0;

	unsigned char numSpheres = collisionBuffer->sphereData->pool->capacity;
	unsigned char numAABBs = collisionBuffer->aabbData->pool->capacity;

	members->spheres = clCreateBuffer
	(
		kBuffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(struct ColliderData_Sphere) * numSpheres,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateBuffer :: spheres");

	members->aabbs = clCreateBuffer
	(
		kBuffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(struct ColliderData_AABB) * numAABBs,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateBuffer :: AABBs");


	members->sPosition = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sPosition");

	members->aPosition = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aPosition");

	members->sNormal = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sNormal");

	members->aNormal = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aNormal");

	members->sLocalMaterial = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sLocalMaterial");

	members->aLocalMaterial = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aLocalMaterial");
	members->sGlobalMaterial = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sGlobalMaterial");

	members->aGlobalMaterial = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aGlobalMaterial");


	format.image_channel_data_type = CL_UNORM_INT8;

	members->sDiffuse = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sDiffuse");

	members->aDiffuse = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aDiffuse");



	members->sSpecular = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sSpecular");

	members->aSpecular = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aSpecular");

	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_UNORM_INT8;

	members->sShadow = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sShadow");

	members->aShadow = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aShadow");

	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_FLOAT;

	members->sDepth = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: sDepth");

	members->aDepth = clCreateImage
	(
		kBuffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateImage :: aDepth");

	members->cameraPosition = clCreateBuffer
	(
		kBuffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 3,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateBuffer :: cameraPosition");

	unsigned int numGObjects = objectBuffer->objectPool->pool->capacity;
	members->gObjects = clCreateBuffer
	(
		kBuffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(GObject) * numGObjects,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateBuffer :: gObjects");

	unsigned int numMaterials = assetBuffer->materialPool->pool->capacity;
	members->materials = clCreateBuffer
	(
		kBuffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(Material) * numMaterials,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateBuffer :: cameraPosition");

	members->sphereTransformations = clCreateBuffer
	(
		kBuffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 16 * numSpheres,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_InitializeMembers :: clCreateBuffer :: cameraPosition");
}

///
//Frees memory allocated by the RayTracerRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the RayTracerRenderPipeline_Members to free
static void RayTracerRenderPipeline_FreeMembers(RenderPipeline_Members* mems)
{
	RayTracerRenderPipeline_Members* members = (RayTracerRenderPipeline_Members*)mems;
	RayBuffer_Free(members->rBuffer);
	//RayBuffer_Free(members->reflectionBuffer);
	//RayBuffer_Free(members->transmissionBuffer);
	GlobalBuffer_Free(members->gBuffer);
	/*
	KernelProgram_Free(members->directionalShadowKernelProg);
	KernelProgram_Free(members->pointShadowKernelProg);
	KernelProgram_Free(members->reflectionKernelProg);
	KernelProgram_Free(members->transmissionKernelProg);
	*/
	KernelProgram_Free(members->rayTracerKernelProg);
	KernelProgram_Free(members->toneReproductionKernelProg);

	cl_int err = 0;

	err = clReleaseMemObject(members->spheres);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: spheres");
	err = clReleaseMemObject(members->aabbs);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aabbs");

	err = clReleaseMemObject(members->sPosition);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sPosition");
	err = clReleaseMemObject(members->aPosition);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aPosition");
	err = clReleaseMemObject(members->sDiffuse);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sDiffuse");
	err = clReleaseMemObject(members->aDiffuse);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aDiffuse");
	err = clReleaseMemObject(members->sNormal);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sNormal");
	err = clReleaseMemObject(members->aNormal);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aNormal");
	err = clReleaseMemObject(members->sLocalMaterial);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sLocalMaterial");
	err = clReleaseMemObject(members->aLocalMaterial);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aLocalMaterial");
	err = clReleaseMemObject(members->sGlobalMaterial);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sGlobalMaterial");
	err = clReleaseMemObject(members->aGlobalMaterial);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aGlobalMaterial");
	err = clReleaseMemObject(members->sSpecular);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sSpecular");
	err = clReleaseMemObject(members->aSpecular);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aSpecular");
	err = clReleaseMemObject(members->sShadow);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sShadow");
	err = clReleaseMemObject(members->aShadow);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aShadow");
	err = clReleaseMemObject(members->sDepth);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sDepth");
	err = clReleaseMemObject(members->aDepth);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: aDepth");
	
	err = clReleaseMemObject(members->cameraPosition);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: cameraPosition");

	err = clReleaseMemObject(members->gObjects);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: gObjects");
	err = clReleaseMemObject(members->materials);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: materials");
	err = clReleaseMemObject(members->sphereTransformations);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_FreeMembers :: clReleaseMemObject :: sphereTransformations");
	free(members);
}


///
//Renders a set of gameobjects given as a memory pool witht he specified render pipeline
//
//Parameters:
//	pipeline: A pointer to the RayTracerRenderPipeline to render with
//	buffer: A pointer to the rendering buffer to render with
//	pool: A pointer to the memory pool of GObjects to render
static void RayTracerRenderPipeline_RenderWithMemoryPool(RenderPipeline* pipeline, struct RenderingBuffer* buffer, MemoryPool* pool)
{
	RayTracerRenderPipeline_Members* members = (RayTracerRenderPipeline_Members*)pipeline->members;
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	//Clear rBuffer
	for(int i = 0; i <= RayBuffer_TextureType_GLOBALMATERIAL; i++)
	{
		RayBuffer_ClearTexture(members->rBuffer, i);
		//RayBuffer_ClearTexture(members->reflectionBuffer, i);
		//RayBuffer_ClearTexture(members->transmissionBuffer, i);
	}

	//Clear gBuffer
	for(int i = 0; i < GlobalBuffer_TextureType_NUMTEXTURES; i++)
	{
		GlobalBuffer_ClearTexture(members->gBuffer, i);
	}
	
	RayBuffer_BindForGeometryPass(members->rBuffer);


	RenderingBuffer* renderingBuffer = RenderingManager_GetRenderingBuffer();
	Camera* cam = renderingBuffer->camera;

	Vector cameraPosition;
	Vector_INIT_ON_STACK(cameraPosition, 3);

	//TODO: Write a function for this.. seriously..
	//Get the position of the camera	
	Matrix_SliceColumn(&cameraPosition, cam->translationMatrix, 3, 0, 3);
	Vector_Scale(&cameraPosition, -1.0f);


	struct ColliderData_Sphere* spheres = collisionBuffer->worldSphereData->pool->data;
	struct ColliderData_AABB* aabbs = collisionBuffer->worldAABBData->pool->data;

	float* transformations = collisionBuffer->sphereTransformations->pool->data;

	unsigned char numSpheres, numAABBs;
	numSpheres = collisionBuffer->worldSphereData->pool->size;
	numAABBs = collisionBuffer->worldAABBData->pool->size;

	unsigned int numGObjects, numMaterials;;
	numGObjects = objectBuffer->objectPool->pool->size;
	numMaterials = assetBuffer->materialPool->pool->size;

	GObject* gObjects = objectBuffer->objectPool->pool->data;
	Material* materials = assetBuffer->materialPool->pool->data;

	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();

	cl_int err = 0;



	//Perform geometry pass
	pipeline->programs[0]->Render(pipeline->programs[0], buffer, pool);

	glFinish();
	
	//Perform ray trace pass
	unsigned int numBeforeRayTrace = 8;
	cl_event beforeRayTrace[numBeforeRayTrace];
	RayBuffer_BindForKernels(members->rBuffer, &beforeRayTrace[0]);
	GlobalBuffer_BindForKernels(members->gBuffer, &beforeRayTrace[1]);
	//Spheres
	err = clEnqueueWriteBuffer
	(
		kBuf->clQueue,
		members->spheres,
		CL_FALSE,
		0,
		sizeof(struct ColliderData_Sphere) * numSpheres,
		spheres,
		0,
		NULL,
		&beforeRayTrace[2]
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_RenderWithMemoryPool :: clEnqueueWriteBuffer :: spheres");

	//AABB
	err = clEnqueueWriteBuffer
	(
		kBuf->clQueue,
		members->aabbs,
		CL_FALSE,
		0,
		sizeof(struct ColliderData_AABB) * numAABBs,
		aabbs,
		0,
		NULL,
		&beforeRayTrace[3]
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_RenderWithMemoryPool :: clEnqueueWriteBuffer :: aabbs");

	//gObjects
	err = clEnqueueWriteBuffer
	(
		kBuf->clQueue,
		members->gObjects,
		CL_FALSE,
		0,
		sizeof(GObject) * numGObjects,
		gObjects,
		0,
		NULL,
		&beforeRayTrace[4]
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_RenderWithMemoryPool :: clEnqueueWriteBuffer :: gObjects");

	err = clEnqueueWriteBuffer
	(
		kBuf->clQueue,
		members->materials,
		CL_FALSE,
		0,
		sizeof(Material) * numMaterials,
		materials,
		0,
		NULL,
		&beforeRayTrace[5]
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_RenderWithMemoryPool :: clEnqueueWriteBuffer :: materials");

	err = clEnqueueWriteBuffer
	(
		kBuf->clQueue,
		members->cameraPosition,
		CL_FALSE,
		0,
		sizeof(float) * 3,
		cameraPosition.components,
		0,
		NULL,
		&beforeRayTrace[6]
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_RenderWithMemoryPool :: clEnqueueWriteBuffer :: cameraPosition");

	err = clEnqueueWriteBuffer
	(
		kBuf->clQueue,
		members->sphereTransformations,
		CL_FALSE,
		0,
		sizeof(float) * 16 * numSpheres,
		transformations, //Ptr to data
		0,
		NULL,
		&beforeRayTrace[7]
	);
	KernelManager_CheckCLErrors(err, "RayTracerRenderPipeline_RenderWithMemoryPool :: clEnqueueWriteBuffer :: sphereTransformations");


	clWaitForEvents(numBeforeRayTrace, &beforeRayTrace[0]);

	struct RayTracerKernelProgram_ExecutionParameters rayTracerParams;
	rayTracerParams.srcBuffer = members->rBuffer;
	rayTracerParams.destBuffer = members->gBuffer;
	rayTracerParams.aabbs = &members->aabbs;
	rayTracerParams.spheres = &members->spheres;
	rayTracerParams.cameraPosition = &members->cameraPosition;
	rayTracerParams.gObjects = &members->gObjects;
	rayTracerParams.materials = &members->materials;
	rayTracerParams.sphereTransformations = &members->sphereTransformations;

	members->rayTracerKernelProg->Execute(members->rayTracerKernelProg, KernelManager_GetKernelBuffer(), &rayTracerParams);

	members->toneReproductionKernelProg->Execute(members->toneReproductionKernelProg, KernelManager_GetKernelBuffer(), members->gBuffer);
	cl_event releaseFromKernels[2];
	RayBuffer_ReleaseFromKernels(members->rBuffer, &releaseFromKernels[0]);
	GlobalBuffer_ReleaseFromKernels(members->gBuffer, &releaseFromKernels[1]);
	clWaitForEvents(2, &releaseFromKernels[0]);

	/*	
	//Create parameter struct for the PointShadowKernelProgram
	struct RayTracerPointShadowKernelProgram_ExecutionParameters pointShadowParams;
	pointShadowParams.buffer = members->rBuffer;
	pointShadowParams.light = NULL;
	pointShadowParams.aabbs = &members->aabbs;
	pointShadowParams.spheres = &members->spheres;
	
	glEnable(GL_STENCIL_TEST);
	
	//struct LinkedList_Node* current = gameObjs->head;
	//GObject* gameObj = NULL;
	//while(current != NULL)
	//{
	for(unsigned int i = 0; i < pool->pool->capacity; i++)
	{
		GObject* gameObj = (GObject*)MemoryPool_RequestAddress(pool, i);
		if(gameObj->light != NULL)
		{
			pointShadowParams.light = gameObj;

			RayBuffer_ClearTexture(members->rBuffer, RayBuffer_TextureType_SHADOW);
			glFinish();
		
			RayBuffer_BindForKernels(members->rBuffer, &beforeExecution[0]);
			clWaitForEvents(1, &beforeExecution[0]);
		
			members->pointShadowKernelProg->Execute(members->pointShadowKernelProg, KernelManager_GetKernelBuffer(), &pointShadowParams);

			RayBuffer_ReleaseFromKernels(members->rBuffer, &beforeExecution[0]);
			clWaitForEvents(1, &beforeExecution[0]);

			//Removed because of culling of objects inside of stencil sphere when camera is outside
			//RayBuffer_BindForStencilPass(members->rBuffer);
			//pipeline->programs[3]->Render(pipeline->programs[3], buffer, gameObj);


			RayBuffer_BindForLightPass(members->rBuffer);
			pipeline->programs[2]->Render(pipeline->programs[2], buffer, gameObj);
		}
	}

	glDisable(GL_STENCIL_TEST);
	*/
	
//	RayBuffer_BindForFinalPass(members->rBuffer);

	GlobalBuffer_BindForFinalPass(members->gBuffer);
	//pipeline->programs[4]->Render(pipeline->programs[4], buffer, members->gBuffer);


	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

//	glBindFramebuffer(GL_READ_FRAMEBUFFER, members->transmissionBuffer->fbo);
//	glReadBuffer(GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_POSITION);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, members->gBuffer->fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_FINAL);
//	glReadBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_TRANSMISSION);
//	glReadBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_REFLECTION);
//	glReadBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_LOCAL);
//	glReadBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_GLOBALMATERIAL);
	glBlitFramebuffer
	(
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	
	glFlush();
}
