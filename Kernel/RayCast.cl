bool RayCastSphere(__private float3 rayDirection, __private float3 rayPosition, __private float4 sphere)
{
	__private float3 spherePosition;
	__private float scalarProj, perpCompSq;
	
	spherePosition = sphere.xyz - rayPosition.xyz;
	
	scalarProj = dot(spherePosition, rayDirection);
	perpCompSq = dot(spherePosition, spherePosition) - scalarProj * scalarProj;

	return (scalarProj >= 0.0f) && (perpCompSq <= (sphere.w * sphere.w));
}

float RayCastSpheref(__private float3 rayDirection, __private float3 rayPosition, __private float4 sphere)
{
	__private float3 spherePosition;
	__private float scalarProj, perpCompSq;
	
	spherePosition = sphere.xyz - rayPosition.xyz;
	
	scalarProj = dot(spherePosition, rayDirection);
	perpCompSq = dot(spherePosition, spherePosition) - scalarProj * scalarProj;

	__private float fExpr = (float)((scalarProj >= 0.0f) && (perpCompSq <= (sphere.w * sphere.w)));
	__private float result = fExpr * (scalarProj);


	return result;
}

bool RayCastAABB(__private float3 rayDirection, __private float3 rayPosition, __private float3 aabbMin, __private float3 aabbMax)
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

	minBound = aabbMin - (rayPosition + rayDirection * 0.001f);
	maxBound = aabbMax - (rayPosition + rayDirection * 0.001f);

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

float RayCastAABBf(__private float3 rayDirection,__private float3 rayPosition,__private  float3 aabbMin,__private float3 aabbMax)
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

	minBound = aabbMin - (rayPosition + rayDirection * 0.001f);
	maxBound = aabbMax - (rayPosition + rayDirection * 0.001f);

	if(rayDirection.x == 0.0f)
	{
		if((minBound.x > 0.0f) || (maxBound.x < 0.0f))
		{
			return 0.0f;
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
			return 0.0f;
		}
	}


	if(rayDirection.z == 0.0f)
	{
		if((minBound.z > 0.0f) || (maxBound.z < 0.0f))
		{
			return 0.0f;
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
		return 0.0f;
	}
	if(tLeaving < 0.0f)
	{
		return 0.0f;
	}

	return ((tEnter > 0.0f) * tEnter + 0.0f);
}

float3 RayCastAABBv(__private float3 rayDirection, __private float3 rayPosition, __private float3 aabbMin, __private float3 aabbMax)
{

	__private float3 normal = (float3)(0.0f, 0.0f, 0.0f);
	__private float tE;
	__private float tL;
	__private float tEnter;
	__private float tLeaving;
	__private float temp;
	tEnter = 0.0f;
	tLeaving = 10000.0f;

	__private float3 minBound;
	__private float3 maxBound;

	minBound = aabbMin - (rayPosition + rayDirection * 0.000f);
	maxBound = aabbMax - (rayPosition + rayDirection * 0.000f);

	__private float side = 0.0f;
	__private float3 zero = (float3)(0.0f, 0.0f, 0.0f);

	if(rayDirection.x == 0.0f)
	{
		if((minBound.x > 0.0f) || (maxBound.x < 0.0f))
		{
			return zero;
		}
	}
	else
	{
		if(rayDirection.x > 0.0f)
		{
			tE = minBound.x / rayDirection.x;
			tL = maxBound.x / rayDirection.x;
			side = -1.0f;
		}
		else
		{	
			tL = minBound.x / rayDirection.x;
			tE = maxBound.x / rayDirection.x;
			side = 1.0f;
		}

		if(tE > tEnter)
		{
			tEnter = tE;
			normal = (float3)(side, 0.0f, 0.0f);
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
			side = -1.0f;
		}
		else
		{	
			tL = minBound.y / rayDirection.y;
			tE = maxBound.y / rayDirection.y;
			side = 1.0f;
		}

		if(tE > tEnter)
		{
			tEnter = tE;
			normal = (float3)(0.0f, side, 0.0f);
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
			return zero;
		}
	}


	if(rayDirection.z == 0.0f)
	{
		if((minBound.z > 0.0f) || (maxBound.z < 0.0f))
		{
			return zero;
		}
	}
	else
	{
		if(rayDirection.z > 0.0f)
		{
			tE = minBound.z / rayDirection.z;
			tL = maxBound.z / rayDirection.z;
			side = -1.0f;
		}
		else
		{	
			tL = minBound.z / rayDirection.z;
			tE = maxBound.z / rayDirection.z;
			side = 1.0f;
		}

		if(tE > tEnter)
		{
			tEnter = tE;
			normal = (float3)(0.0f, 0.0f, side);
		}
		if(tL < tLeaving)
		{
			tLeaving = tL;
		}
	}

	if(tLeaving < tEnter)
	{
		return zero;
	}
	if(tLeaving < 0.0f)
	{
		return zero;
	}

	return ((tEnter > 0.0f) * tEnter + 0.0f) * normal;
}
