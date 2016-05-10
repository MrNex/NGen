float2 TextureMapSphere(__private float3 pointOfCollision, __private float16 transformation, __private float4 sphere, __private float2 tile)
{
	__private float2 uvs = (float2)(0.0f, 0.0f);

	__private float3 xAxis, yAxis, zAxis, translation;
	xAxis = transformation.lo.lo.xyz;
	yAxis = transformation.lo.hi.xyz;
	zAxis = transformation.hi.lo.xyz;
	translation = transformation.odd.odd.xyz;

	__private float3 collisionNormal;
	collisionNormal = pointOfCollision - sphere.xyz;

	__private float3 vertex;
	vertex = (float3)(dot(xAxis, collisionNormal),dot(yAxis, collisionNormal),dot(zAxis, collisionNormal));
	collisionNormal = normalize(collisionNormal);

	__private float theta, phi;
	theta = atan2pi(vertex.x, vertex.z);
	phi = acospi(vertex.y / sphere.w);

	uvs.x = (0.5f * theta + 0.5f) * tile.x;
	uvs.y = (1.0f - phi) * tile.y;
	return uvs;
}

float2 TextureMapAABB(__private float3 pos, __private float3 normal, __private float3 aabbMin, __private float3 aabbMax, __private float2 tile)
{
	__private float2 uvs = (float2)(0.0f, 0.0f);

	__private float3 absN;
	absN = (float3)(fabs(normal.x), fabs(normal.y), fabs(normal.z));

	__private bool xComp, yComp;
	xComp = ((absN.x > absN.y) || (absN.x > absN.z));
	yComp = ((absN.y > absN.x) || (absN.y > absN.z));

	__private float3 dimensions;
	dimensions = aabbMax - aabbMin;
	__private float3 planePos = pos - aabbMin;

	__private float3 possibleUVs = planePos / dimensions;

	uvs.x = (float)(!xComp) * possibleUVs.x + (float)(xComp) * possibleUVs.z;
	uvs.y = (float)(!yComp) * possibleUVs.y + (float)(yComp) * possibleUVs.z;
	uvs.x *= tile.x;
	uvs.y *= tile.y;

	return uvs;
}

float2 TextureMapAABBOld(__private float3 pointOfCollision, __private float3 collisionNormal, __private float3 aabbMin, __private float3 aabbMax, __private float2 tile)
{
	__private float2 uvs;
	uvs = (float2)(0.0f, 0.0f);

	__private bool xComp, yComp, zComp;
	xComp = (fabs(collisionNormal.x) <= fabs(collisionNormal.y));
	yComp = (fabs(collisionNormal.y) <= fabs(collisionNormal.z));
	zComp = !(xComp && yComp);

	__private float3 invertedNormal = (float3)(1.0f * (float)(xComp), 1.0f * (float)(yComp), 1.0f * (float)(zComp));

 	float3 planeSpace = fabs(((pointOfCollision - aabbMin) / (aabbMax - aabbMin)) * invertedNormal);

	xComp = ((planeSpace.x > 0.0f) || (planeSpace.y == 0.0f) || (planeSpace.z == 0.0f));

	uvs.x = (float)(xComp) *  planeSpace.x + (float)(!xComp) * planeSpace.z;

	//yComp = ((planeSpace.y > 0.0f) || (planeSpace.z == 0.0f) || (!xComp));

	uvs.y = (float)(yComp) * planeSpace.y + (float)(!yComp) * planeSpace.z;

	return uvs * tile;
}
