#ifndef _HLSLI_ORBIT_MATERIAL
#define _HLSLI_ORBIT_MATERIAL

struct Material
{
	float4 diffuseAlbedo;   // diffuse color of the material
	float4 specularColor;   // specular color of the material
	float  roughness;       // roughness of the material (the specular exponent is derived from the roughness)
	uint flags; 			// flags for texture maps
};

#define FLAG_HAS_COLOR_MAP (1 << 0)
#define FLAG_HAS_NORMAL_MAP (1 << 1)
#define FLAG_HAS_OCCLUSION_MAP (1 << 2)
#define FLAG_HAS_ROUGHNESS_MAP (1 << 3)

bool isMaterialFlagSet(uint flags, uint flag)
{
	return (flags & flag) == flag;
}

#endif