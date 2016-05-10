typedef struct GObject
{
	uint2 framePtr;
	uint2 meshPtr;
	uint2 statesPtr;
	uint2 bodyPtr;
	uint2 colliderPtr;
	uint2 lightPtr;
	uint colliderID;
	uint colliderType;	//(ID, Type) 1 = sphere, 2 = AABB
	uint materialID;
	uint pad;
} GObject;
