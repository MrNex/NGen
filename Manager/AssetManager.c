#include  "AssetManager.h"

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
//Loads all of the engines assets into the internal asset buffer
void AssetManager_LoadAssets(void)
{
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


	HashMap_Add(assetBuffer->meshMap, "Suzanne", Loader_LoadOBJFile("./Assets/Models/suzanne.obj"), strlen("Suzanne"));
	HashMap_Add(assetBuffer->meshMap, "Tetrahedron", Loader_LoadOBJFile("./Assets/Models/tetrahedron.obj"),strlen("Tetrahedron"));
	HashMap_Add(assetBuffer->meshMap, "Trash Can", Loader_LoadOBJFile("./Assets/Models/trashcan.obj"), strlen("Trash Can"));
	HashMap_Add(assetBuffer->meshMap, "Bottle", Loader_LoadOBJFile("./Assets/Models/bottle.obj"), strlen("Bottle"));
	HashMap_Add(assetBuffer->meshMap, "Target", Loader_LoadOBJFile("./Assets/Models/target.obj"), strlen("Target"));
	HashMap_Add(assetBuffer->meshMap, "Arrow", Loader_LoadOBJFile("./Assets/Models/arrow.obj"), strlen("Arrow"));

	
	

	
	


	//Load textures
	struct Image* i = Loader_Load24BitBMPFile("./Assets/Textures/test.bmp");
	Texture* t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Test", t, strlen("Test"));
	
	i = Loader_Load24BitBMPFile("./Assets/Textures/earth.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Earth", t, strlen("Earth"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/white.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "White", t, strlen("White"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/trash.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Trash Can", t, strlen("Trash Can"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/arrow.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Arrow", t, strlen("Arrow"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/bottle.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Bottle", t, strlen("Bottle"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/target.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Target", t, strlen("Target"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/concrete.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Floor", t, strlen("Floor"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/wall2.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Wall", t, strlen("Wall"));

	i = Loader_Load24BitBMPFile("./Assets/Textures/wood.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Table", t, strlen("Table"));
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
Texture* AssetManager_LookupTexture(char* key)
{
	return (Texture*)HashMap_LookUp(assetBuffer->textureMap, key, strlen(key))->data;
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
		Mesh* m = *(Mesh**)DynamicArray_Index(buffer->meshMap->data, i);
		if(m != NULL)
		{
			Mesh_Free(m);
		}
	}
	HashMap_Free(buffer->meshMap);

	for (unsigned int i = 0; i < buffer->textureMap->data->capacity; i++)
	{
		Texture* t = *(Texture**)DynamicArray_Index(buffer->textureMap->data, i);
		if(t != NULL)
		{
			Texture_Free(t);
		}
	}

	HashMap_Free(buffer->textureMap);
}
