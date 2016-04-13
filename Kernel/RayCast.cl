bool RayCastSphere(float3 rayDirection, float3 rayPosition, float4 sphere)
{
	float3 spherePosition;
	float scalarProj, perpCompSq;
	
	spherePosition = sphere.xyz - rayPosition.xyz;
	
	scalarProj = dot(spherePosition, rayDirection);
	perpCompSq = dot(spherePosition, spherePosition) - scalarProj * scalarProj;

	return (scalarProj >= 0.0f) && (perpCompSq <= (sphere.w * sphere.w));
}

float RayCastSpheref(float3 rayDirection, float3 rayPosition, float4 sphere)
{
	float3 spherePosition;
	float scalarProj, perpCompSq;
	
	spherePosition = sphere.xyz - rayPosition.xyz;
	
	scalarProj = dot(spherePosition, rayDirection);
	perpCompSq = dot(spherePosition, spherePosition) - scalarProj * scalarProj;

	return (float)((scalarProj >= 0.0f) && (perpCompSq <= (sphere.w * sphere.w))) * scalarProj;
}

bool RayCastAABB(float3 rayDirection, float3 rayPosition, float3 aabbMin, float3 aabbMax)
{

	__private float tE;
	__private float tL;
	__private float tEnter;
	__private float tLeaving;
	__private float temp;
	tEnter = 0.0f;
	tLeaving = 10000.0f;



	__private float3 minBound;
	__private float3 maxBound;

	minBound = aabbMin - (rayPosition + rayDirection * 0.0001f);
	maxBound = aabbMax - (rayPosition + rayDirection * 0.0001f);

	if(rayDirection.x == 0.0f)
	{
		if((minBound.x > 0.0f) || (maxBound.x < 0.0f))
		{
			return false;
		}
	}
	else
	{
		if(rayDirection.x > 0.0f)
		{
			tE = minBound.x / rayDirection.x;
			tL = maxBound.x / rayDirection.x;
		}
		else
		{	
			tL = minBound.x / rayDirection.x;
			tE = maxBound.x / rayDirection.x;
		}

		if(tE > tEnter)
		{
			tEnter = tE;
		}
		if(tL < tLeaving)
		{
			tLeaving = tL;
		}
	}

	
	if(rayDirection.y != 0.0f)
	{
		if(rayDirection.y > 0.0f)
		{
			tE = minBound.y / rayDirection.y;
			tL = maxBound.y / rayDirection.y;
		}
		else
		{	
			tL = minBound.y / rayDirection.y;
			tE = maxBound.y / rayDirection.y;
		}

		if(tE > tEnter)
		{
			tEnter = tE;
		}
		if(tL < tLeaving)
		{
			tLeaving = tL;
		}
	}
	else
	{
		if((minBound.y > 0.0f) || (maxBound.y < 0.0f))
		{
			return false;
		}
	}


	if(rayDirection.z == 0.0f)
	{
		if((minBound.z > 0.0f) || (maxBound.z < 0.0f))
		{
			return false;
		}
	}
	else
	{
		if(rayDirection.z > 0.0f)
		{
			tE = minBound.z / rayDirection.z;
			tL = maxBound.z / rayDirection.z;
		}
		else
		{	
			tL = minBound.z / rayDirection.z;
			tE = maxBound.z / rayDirection.z;
		}

		if(tE > tEnter)
		{
			tEnter = tE;
		}
		if(tL < tLeaving)
		{
			tLeaving = tL;
		}
	}

	if(tLeaving < tEnter)
	{
		return false;
	}
	if(tLeaving < 0.0f)
	{
		return false;
	}

	return true;	
}
