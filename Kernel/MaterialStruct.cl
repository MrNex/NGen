typedef struct Material
{
	float16 colorMatrix;
	float4 specularColor;
	float2 tile;

	float ambientCoefficient;
	float diffuseCoefficient;
	float specularCoefficient;
	float specularPower;

	float localCoefficient;
	float reflectedCoefficient;
	float transmittedCoefficient;

	uint texturePoolID;

	float indexOfRefraction;
} Material;
