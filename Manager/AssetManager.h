#include "../Data/HashMap.h"
#include "../Data/MemoryPool.h"
#include "../Render/Mesh.h"
#include "../Render/Texture.h"
#include "../Render/Material.h"
#include "../Load/Loader.h"



typedef struct AssetBuffer
{
	HashMap* meshMap;
	HashMap* textureMap;

	MemoryPool* texturePool;	//Memory pool of GLuints
	MemoryPool* textureRefsPool;	//Memory pool of clmem

	MemoryPool* materialPool;	//MemoryPool of materials

	cl_mem textureArray;
} AssetBuffer;

///
//Internals
extern AssetBuffer* assetBuffer;

///
//Functions

///
//Initializes the Asset MAnager
void AssetManager_Initialize(void);

///
//Frees all resources allocated by the asset manager
//And all data with pointers being held by the asset manager
//You should probably only do this when you're closing the engine.
void AssetManager_Free(void);

///
//Gets the internal asset buffer being managed by the asset manager
//
//Returns:
//	Internal asset buffer of asset manager
AssetBuffer AssetManager_GetAssetBuffer(void);

///
//Loads all of the engines assets into the internal asset buffer
void AssetManager_LoadAssets(void);

///
//Looks up a mesh from the asset manager's internal buffer
//
//Parameters:
//	key: Name of the mesh to lookup
//
//Returns:
//	Pointer to the requested mesh, or NULL if mesh was not found
Mesh* AssetManager_LookupMesh(char* key);

///
//Looks up a texture from he asset manager's internal buffer
//
//Parameters
//	key: The name of the texture to lookup
//
//Returns:
//	GLuint containing ID of texture unit
GLuint AssetManager_LookupTexture(char* key);

///
//Looks up a texture's texture pool ID given the associated key
//
//Parameters:
//	key: The name of the texture to lookup
//
//returns:
//	Texture pool ID associated with the desired texture
unsigned int AssetManager_LookupTextureID(char* key);

///
//Looks up a texture from the asset managers' internal buffers
//using the texture pool ID of a texture
//
//Parmaeters:
//	id: The texture pool ID of the desired texture
//
//Returns:
//	GLuint containing the texture unit ID of desired texture
GLuint AssetManager_LookupTextureByID(unsigned int id);

///
//Looks up a texture's clReference from he asset manager's internal buffer
//
//Parameters
//	key: The name of the texture to lookup
//
//Returns:
//	cl_mem containing reference to texture
cl_mem AssetManager_LookupTextureCLReference(char* key);

///
//Looks up a texture's clReference from the asset managers' internal buffers
//using the texture pool ID of a texture
//
//Parmaeters:
//	id: The texture pool ID of the desired texture
//
//Returns:
//	cl_mem containing reference todesired texture
cl_mem AssetManager_LookupTextureCLReferenceByID(unsigned int id);

///
//Looks up a material from the asset manager's internal buffers
//Using the MaterialPool ID of a material
//
//Parameters:
//	id: The materialPool ID of the desired texture
//
//Returns:
//	A pointer to the desired material
Material* AssetManager_LookupMaterialByID(unsigned int id);

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
void AssetManager_BindTexturesForKernelUse(KernelBuffer* buf, cl_event* waitFor, unsigned int numWaitFor, cl_event* eReturn);

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
void AssetManager_ReleaseTexturesFromKernelUse(KernelBuffer* buf, cl_event* waitFor, unsigned int numWaitFor, cl_event* eReturn);
