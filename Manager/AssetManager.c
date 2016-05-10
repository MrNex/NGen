#include  "AssetManager.h"

#include <CL/cl.h>
#include <CL/cl_gl.h>
#include "KernelManager.h"


#include "../Generation/Generator.h"


///
//Declarations

AssetBuffer* assetBuffer;

///
//Allocates a new AssetBuffer
//
//Returns:
//      Pointer to a newly allocated uninitialized asset buffer
static AssetBuffer* AssetManager_AllocateBuffer(void);

///
//Initializes an asset buffer
//
//Parameters:
//      buffer: pointer to the buffer to initialize
static void AssetManager_InitializeBuffer(AssetBuffer* buffer);

///
//Frees resuorces allocated by an Asset Buffer
//Deletes data being held in buffer!!
//
//PArameters:
//      buffer: pointer to The buffer to free
static void AssetManager_FreeBuffer(AssetBuffer* buffer);


///
//Implementations

///
//Initializes the Asset MAnager
void AssetManager_Initialize(void)
{
	assetBuffer = AssetManager_AllocateBuffer();
	AssetManager_InitializeBuffer(assetBuffer);

}

///
//Frees all resources allocated by the asset manager
//And all data with pointers being held by the asset manager
//You should probably only do this when you're closing the engine.
void AssetManager_Free(void)
{
	AssetManager_FreeBuffer(assetBuffer);
}

///
//Gets the internal asset buffer being managed by the asset manager
//
//Returns:
//	Internal asset buffer of asset manager
AssetBuffer AssetManager_GetAssetBuffer(void)
{
	return *assetBuffer;
}

///
//Helper method for AssetManager_LoadAssets
//Loads a single texture
//
//Parameters:
//	file: Filename
//	key: the key to assign this texture for later lookup
static void AssetManager_LoadTexture(const char* file, const char* key)
{
	struct Image* i = Loader_Load24BitBMPFile(file);

	unsigned int tID = MemoryPool_RequestID(assetBuffer->texturePool);
	unsigned int refID = MemoryPool_RequestID(assetBuffer->textureRefsPool);

	if(tID != refID)
	{
		printf("FATAL FAILURE: DETERMINISM ERROR ASSETMANAGER_LOADTEXTURE TID != REFID\n\n");
	}

	GLuint* texture = MemoryPool_RequestAddress(assetBuffer->texturePool, tID);
	cl_mem* ref = MemoryPool_RequestAddress(assetBuffer->textureRefsPool, refID);
	//Texture* t = Texture_Allocate();
	//Texture_Initialize(t, i);
	*texture = Texture_InitializeGLuint(i);
	*ref = Texture_InitializeCLReference(*texture, KernelManager_GetKernelBuffer());
	HashMap_Add(assetBuffer->textureMap, (void*)key, (void*)tID, strlen(key));

	Image_Free(i);
}

///
//Loads all of the engines assets into the internal asset buffer
void AssetManager_LoadAssets(void)
{
	//Get a reference to the Kernel Buffer
	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();

	//Load meshes
	HashMap_Add(assetBuffer->meshMap, "Cube", Loader_LoadOBJFile("./Assets/Models/cube.obj"), strlen("Cube"));
	//HashMap_Add(assetBuffer->meshMap, "Cube", Generator_GenerateCubeMesh(2.0f));
	HashMap_Add(assetBuffer->meshMap, "Sphere", Loader_LoadOBJFile("./Assets/Models/sphere.obj"), strlen("Sphere"));
	//HashMap_Add(assetBuffer->meshMap, "Sphere", Generator_GenerateSphereMesh(2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Cylinder", Loader_LoadOBJFile("./Assets/Models/cylinder.obj"), strlen("Cylinder"));
	//HashMap_Add(assetBuffer->meshMap, "Cylinder", Generator_GenerateCylinderMesh(1.0f, 2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Cone", Loader_LoadOBJFile("./Assets/Models/cone.obj"), strlen("Cone"));
	//HashMap_Add(assetBuffer->meshMap, "Cone", Generator_GenerateConeMesh(1.0f, 2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Pipe", Loader_LoadOBJFile("./Assets/Models/pipe.obj"), strlen("Pipe"));
	//HashMap_Add(assetBuffer->meshMap, "Pipe", Generator_GenerateTubeMesh(1.0f, 0.5f, 2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Torus", Loader_LoadOBJFile("./Assets/Models/torus.obj"), strlen("Torus"));
	//HashMap_Add(assetBuffer->meshMap, "Torus", Generator_GenerateTorusMesh(2.0f, 1.0f, 10));
	HashMap_Add(assetBuffer->meshMap, "CubeWire", Loader_LoadOBJFile("./Assets/Models/cubewire.obj"), strlen("CubeWire"));

	HashMap_Add(assetBuffer->meshMap, "Square", Loader_LoadOBJFile("./Assets/Models/square.obj"), strlen("Square"));


	HashMap_Add(assetBuffer->meshMap, "Suzanne", Loader_LoadOBJFile("./Assets/Models/suzanne.obj"), strlen("Suzanne"));
	HashMap_Add(assetBuffer->meshMap, "Tetrahedron", Loader_LoadOBJFile("./Assets/Models/tetrahedron.obj"),strlen("Tetrahedron"));
	HashMap_Add(assetBuffer->meshMap, "Trash Can", Loader_LoadOBJFile("./Assets/Models/trashcan.obj"), strlen("Trash Can"));
	HashMap_Add(assetBuffer->meshMap, "Bottle", Loader_LoadOBJFile("./Assets/Models/bottle.obj"), strlen("Bottle"));
	HashMap_Add(assetBuffer->meshMap, "Target", Loader_LoadOBJFile("./Assets/Models/target.obj"), strlen("Target"));
	HashMap_Add(assetBuffer->meshMap, "Arrow", Loader_LoadOBJFile("./Assets/Models/arrow.obj"), strlen("Arrow"));

	//Load textures

	AssetManager_LoadTexture("./Assets/Textures/concrete.bmp","Concrete");
	AssetManager_LoadTexture("./Assets/Textures/earth.bmp","Earth");
	AssetManager_LoadTexture("./Assets/Textures/checkered.bmp","Checkered");
	AssetManager_LoadTexture("./Assets/Textures/wall.bmp", "Wall");
	AssetManager_LoadTexture("./Assets/Textures/wood.bmp","Wood");
	AssetManager_LoadTexture("./Assets/Textures/white.bmp","White");


	AssetManager_LoadTexture("./Assets/Textures/granite.bmp","Granite");

	AssetManager_LoadTexture("./Assets/Textures/test.bmp","Test");


	AssetManager_LoadTexture("./Assets/Textures/stone.bmp","Stone");

	AssetManager_LoadTexture("./Assets/Textures/trash.bmp","Trash Can");

	AssetManager_LoadTexture("./Assets/Textures/arrow.bmp","Arrow");


	AssetManager_LoadTexture("./Assets/Textures/bottle.bmp","Bottle");

	AssetManager_LoadTexture("./Assets/Textures/target.bmp","Target");






	AssetManager_LoadTexture("./Assets/Textures/Jacob.bmp","Jacob");


	///
	//We must generate a default material
	//So that any object which is initialized and thus has a materialID of 0
	//Will use the default material during rendering
	//Make default material
	//unsigned int materialID = Material_Allocate();
	//unsigned int testTextureID = AssetManager_LookupTextureID("Test");
	//Material_Initialize(materialID, testTextureID);

	//Material* mat = AssetManager_LookupMaterialByID(materialID);
	//mat->specularCoefficient = 2.0f;
	//mat->specularPower = 16.0f;

	//Fill the texture array for OpenCL
	cl_int err = 0;
	cl_image_format format;
	cl_image_desc desc;

	format.image_channel_order = CL_RGBA;
	format.image_channel_data_type = CL_UNORM_INT8;

	desc.image_type = CL_MEM_OBJECT_IMAGE2D_ARRAY;
	desc.image_width = 1024;
	desc.image_height = 1024;
	desc.image_depth = 0;
	desc.image_array_size = 6;
	desc.image_row_pitch = 0;
	desc.image_slice_pitch = 0;
	desc.num_mip_levels = desc.num_samples = 0;
	desc.mem_object = NULL;

	assetBuffer->textureArray = clCreateImage
	(
		kBuf->clContext,
		CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY,
		&format,
		&desc,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clCreateImage :: textureArray");



	cl_event binding;
	AssetManager_BindTexturesForKernelUse(kBuf, NULL, 0, &binding);

	//Copy the images into texture array
	
	const size_t srcOrigin[3] = { 0, 0, 0};
	const size_t region[3] = { 1024, 1024, 1 };

	cl_event copy[6];

	//Concrete
	const size_t dstOriginConcrete[3] = { 0, 0, 0};
	cl_mem copyTexture = AssetManager_LookupTextureCLReference("Concrete");
	err = clEnqueueCopyImage
	(
		kBuf->clQueue,
		copyTexture,
		assetBuffer->textureArray,
		&srcOrigin[0],
		&dstOriginConcrete[0],
		&region[0],
		1,
		&binding,
		&copy[0]
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clEnqueueCopyImage :: Concrete");

	//Copy earth
	const size_t dstOriginEarth[3] = { 0, 0, 1};
	copyTexture = AssetManager_LookupTextureCLReference("Earth");
	err = clEnqueueCopyImage
	(
		kBuf->clQueue,
		copyTexture,
		assetBuffer->textureArray,
		&srcOrigin[0],
		&dstOriginEarth[0],
		&region[0],
		1,
		&binding,
		&copy[1]
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clEnqueueCopyImage :: earth");



	//Stone
	
	const size_t dstOriginStone[3] = { 0, 0, 2};
	copyTexture = AssetManager_LookupTextureCLReference("Checkered");
	err = clEnqueueCopyImage
	(
		kBuf->clQueue,
		copyTexture,
		assetBuffer->textureArray,
		&srcOrigin[0],
		&dstOriginStone[0],
		&region[0],
		1,
		&binding,
		&copy[2]
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clEnqueueCopyImage :: checkered");



	//Wall
	
	const size_t dstOriginWall[3] = { 0, 0, 3};
	copyTexture = AssetManager_LookupTextureCLReference("Wall");
	err = clEnqueueCopyImage
	(
		kBuf->clQueue,
		copyTexture,
		assetBuffer->textureArray,
		&srcOrigin[0],
		&dstOriginWall[0],
		&region[0],
		1,
		&binding,
		&copy[3]
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clEnqueueCopyImage :: wall");

	//Wood
	
	const size_t dstOriginWood[3] = { 0, 0, 4};
	copyTexture = AssetManager_LookupTextureCLReference("Wood");
	err = clEnqueueCopyImage
	(
		kBuf->clQueue,
		copyTexture,
		assetBuffer->textureArray,
		&srcOrigin[0],
		&dstOriginWood[0],
		&region[0],
		1,
		&binding,
		&copy[4]
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clEnqueueCopyImage :: wood");

	//White
	const size_t dstOriginWhite[3] = { 0, 0, 5};
	copyTexture = AssetManager_LookupTextureCLReference("White");
	err = clEnqueueCopyImage
	(
		kBuf->clQueue,
		copyTexture,
		assetBuffer->textureArray,
		&srcOrigin[0],
		&dstOriginWhite[0],
		&region[0],
		1,
		&binding,
		&copy[5]
	);
	KernelManager_CheckCLErrors(err, "AssetManager_LoadTextures :: clEnqueueCopyImage :: white");


	cl_event cleanup;
	AssetManager_ReleaseTexturesFromKernelUse(kBuf, &copy[0], 6, &cleanup);

	clWaitForEvents(1, &cleanup);

}

///
//Looks up a mesh from the asset manager's internal buffer
//
//Parameters:
//	key: Name of the mesh to lookup
//
//Returns:
//	Pointer to the requested mesh, or NULL if mesh was not found
Mesh* AssetManager_LookupMesh(char* key)
{
	return (Mesh*)HashMap_LookUp(assetBuffer->meshMap, key, strlen(key))->data;
}

///
//Looks up a texture from he asset manager's internal buffer
//
//Parameters
//	key: The name of the texture to lookup
//
//Returns:
//	Pointer to the requested texture, or NULL if the texture was not found
GLuint AssetManager_LookupTexture(char* key)
{
	unsigned int id = (unsigned int)HashMap_LookUp(assetBuffer->textureMap, key, strlen(key))->data;
	return AssetManager_LookupTextureByID(id);
}


///
//Looks up a texture's texture pool ID given the associated key
//
//Parameters:
//	key: The name of the texture to lookup
//
//returns:
//	Texture pool ID associated with the desired texture
unsigned int AssetManager_LookupTextureID(char* key)
{
	
	unsigned int id = (unsigned int)HashMap_LookUp(assetBuffer->textureMap, key, strlen(key))->data;
	return id;
}

///
//Looks up a texture from the asset managers' internal buffers
//using the texture pool ID of a texture
//
//Parmaeters:
//	id: The texture pool ID of the desired texture
//
//Returns:
//	GLuint containing the texture unit ID of desired texture
GLuint AssetManager_LookupTextureByID(unsigned int id)
{

	GLuint t = *(GLuint*)MemoryPool_RequestAddress(assetBuffer->texturePool, id);
	return t;
}

///
//Looks up a texture's clReference from he asset manager's internal buffer
//
//Parameters
//	key: The name of the texture to lookup
//
//Returns:
//	cl_mem containing reference to texture
cl_mem AssetManager_LookupTextureCLReference(char* key)
{

	unsigned int id = (unsigned int)HashMap_LookUp(assetBuffer->textureMap, key, strlen(key))->data;	
	return AssetManager_LookupTextureCLReferenceByID(id);
}	

///
//Looks up a texture's clReference from the asset managers' internal buffers
//using the texture pool ID of a texture
//
//Parmaeters:
//	id: The texture pool ID of the desired texture
//
//Returns:
//	cl_mem containing reference todesired texture
cl_mem AssetManager_LookupTextureCLReferenceByID(unsigned int id)
{
	return *(cl_mem*)MemoryPool_RequestAddress(assetBuffer->textureRefsPool, id);
}

///
//Looks up a material from the asset manager's internal buffers
//Using the MaterialPool ID of a material
//
//Parameters:
//	id: The materialPool ID of the desired texture
//
//Returns:
//	A pointer to the desired material
Material* AssetManager_LookupMaterialByID(unsigned int id)
{
	return (Material*)MemoryPool_RequestAddress(assetBuffer->materialPool, id);
}

///
//Binds the textures contained in the AssetManager for use by OpenCL kernels
//When in this mode, use of textures by OpenGL shaders or associated functions is undefined.
//Unbind before utilizing textures with OpenGL again.
//
//Parameters:
//	buf: A pointer to the active kernel buffer containing the context and device to bind the textures to
//	waitFor: A pointer to an array of events to wait for before binding the textures
//	numWaitFor: The number of events to wait for
//	eReturn: A pointer to the destination to store the event of the binding
void AssetManager_BindTexturesForKernelUse(KernelBuffer* buf, cl_event* waitFor, unsigned int numWaitFor, cl_event* eReturn)
{
	cl_int err = 0;
	err = clEnqueueAcquireGLObjects
	(
		buf->clQueue,
		assetBuffer->textureRefsPool->pool->size,
		assetBuffer->textureRefsPool->pool->data,
		numWaitFor,
		waitFor,
		eReturn
	);
	
	KernelManager_CheckCLErrors(err, "AssetManager_BindTexturesForKernelUse :: clEnqueueAcquireGLObjects");

}

///
//Unbinds the textures contained in the asset manager for use by openCL kernels
//When in this mode, use of textures by OpenCL kernels or associated functions is undefined.
//Bind before utilizing textures with OpenCL again
//Keep unbound otherwise
//
//Parameters:
//	buf: A pointer to the active kernel buffer containing the context and device to unbind the textures from
//	waitFor: A pointer to an array of events to wait for before binding the textures
//	numWaitFor: The number of events to wait for
//	eReturn: A pointer to the destination to store the event of the binding
void AssetManager_ReleaseTexturesFromKernelUse(KernelBuffer* buf, cl_event* waitFor, unsigned int numWaitFor, cl_event* eReturn)
{
	cl_int err = 0;
	err = clEnqueueReleaseGLObjects
	(
		buf->clQueue,
		assetBuffer->textureRefsPool->pool->size,
		assetBuffer->textureRefsPool->pool->data,
		numWaitFor,
		waitFor,
		eReturn
	);

	KernelManager_CheckCLErrors(err, "AssetManager_ReleaseTexturesFromKernelUse :: clEnqueueReleaseGLObects");


}

///
//Allocates a new AssetBuffer
//
//Returns:
//	Pointer to a newly allocated uninitialized asset buffer
static AssetBuffer* AssetManager_AllocateBuffer(void)
{
	AssetBuffer* buffer = (AssetBuffer*)malloc(sizeof(AssetBuffer));
	return buffer;
}

///
//Initializes an asset buffer
//
//Parameters:
//	buffer: pointer to the buffer to initialize
static void AssetManager_InitializeBuffer(AssetBuffer* buffer)
{
	buffer->meshMap = HashMap_Allocate();
	buffer->textureMap = HashMap_Allocate();
	HashMap_Initialize(buffer->meshMap);
	HashMap_Initialize(buffer->textureMap);

	buffer->texturePool = MemoryPool_Allocate();
	MemoryPool_Initialize(buffer->texturePool, sizeof(GLuint));

	buffer->textureRefsPool = MemoryPool_Allocate();
	MemoryPool_Initialize(buffer->textureRefsPool, sizeof(cl_mem));

	buffer->materialPool = MemoryPool_Allocate();
	MemoryPool_Initialize(buffer->materialPool, sizeof(Material));

}

///
//Frees resuorces allocated by an Asset Buffer
//Deletes data being held in buffer!!
//
//PArameters:
//	buffer: pointer to The buffer to free
static void AssetManager_FreeBuffer(AssetBuffer* buffer)
{
	for (unsigned int i = 0; i < buffer->meshMap->data->capacity; i++)
	{
		struct HashMap_KeyValuePair* pair = *(struct HashMap_KeyValuePair**)DynamicArray_Index(buffer->meshMap->data, i);
		if(pair != NULL)
		{
			Mesh* m = (Mesh*)pair->data;
			Mesh_Free(m);
			//Following line will be done by HashMap_Free
			//HashMap_KeyValuePair_Free(pair);
		}
	}
	printf("Meshes freed\n");
	HashMap_Free(buffer->meshMap);
	printf("MeshMap freed\n");

	for (unsigned int i = 0; i < buffer->textureMap->data->capacity; i++)
	{
		struct HashMap_KeyValuePair* pair = *(struct HashMap_KeyValuePair**)DynamicArray_Index(buffer->textureMap->data, i);
		if(pair != NULL)
		{
			unsigned int ID = (unsigned int)pair->data;
			GLuint t = *(GLuint*)MemoryPool_RequestAddress(buffer->texturePool, ID);
			cl_mem tRef = *(cl_mem*)MemoryPool_RequestAddress(buffer->textureRefsPool, ID);
			Texture_FreeCLReference(tRef);
			Texture_FreeGLuint(t);
			
		}
	}
	printf("Textures freed.\n");
	HashMap_Free(buffer->textureMap);

	printf("Texture Map Freed.\n");

	MemoryPool_Free(buffer->texturePool);
	MemoryPool_Free(buffer->textureRefsPool);

	MemoryPool_Free(buffer->materialPool);

	clReleaseMemObject(buffer->textureArray);
}


