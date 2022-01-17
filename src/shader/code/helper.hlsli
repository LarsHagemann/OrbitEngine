#ifndef _HLSLI_ORBIT_HELPER
#define _HLSLI_ORBIT_HELPER

#include "io.hlsli"
#include "textures.hlsli"

float2 correctTextureY(float2 texcoords)
{
	return float2(texcoords.x, 1.f - texcoords.y);
}

float3x3 calculateNormalTransformation(matrix worldTransform)
{
	return float3x3(
		worldTransform._m00_m01_m02,
		worldTransform._m10_m11_m12,
		worldTransform._m20_m21_m22
	);
}

// INPUT:
// 	inNormal: the normal to transform
// 	worldTransform: the object's world transformation matrix
float4 transformNormal(float4 inNormal, float3x3 normalTransform)
{
	return float4(normalize(mul(inNormal.xyz, normalTransform)), 0.f);
}

bool isMaterialFlagSet(int flag)
{
#ifdef ORBIT_DIRECTX_11
	return isMaterialFlagSetImpl(material.flags, flag);
#else
	return isMaterialFlagSetImpl(PerMeshBuffer.material.flags, flag);
#endif
}

float4 getVertexNormal(float2 uv, float4 inNormal, float4 inTangent)
{
	if (isMaterialFlagSet(FLAG_HAS_NORMAL_MAP))
	{
		float4 T = normalize(inTangent - dot(inTangent, inNormal) * inNormal);
		float4 N = inNormal;
		float3 B = normalize(cross(N.xyz, T.xyz));
		float3x3 TBN = float3x3(T.xyz, B, N.xyz);
		float3 normal = normalMap.Sample(MinMagMipLinearWrap, uv).xyz * 2.f - 1.f;
		return normalize(float4(mul(TBN, normal), 0.f));
	}
	return inNormal;
}

float4 getVertexOcclusion(float2 uv)
{
	if (isMaterialFlagSet(FLAG_HAS_OCCLUSION_MAP))
		return occlusionMap.Sample(MinMagMipLinearWrap, uv);
	
	return float4(1.f, 1.f, 1.f, 1.f);
}

float4 getVertexColor(float2 uv)
{
	if (isMaterialFlagSet(FLAG_HAS_COLOR_MAP))
		return colorMap.Sample(MinMagMipAnisotropicClamp, uv);

#ifdef ORBIT_DIRECTX_11
	return material.diffuseAlbedo;
#else
	return PerMeshBuffer.material.diffuseAlbedo;
#endif
}

float4 getVertexRoughness(float2 uv)
{
	if (isMaterialFlagSet(FLAG_HAS_ROUGHNESS_MAP))
		return roughnessMap.Sample(MinMagMipLinearWrap, uv);

#ifdef ORBIT_DIRECTX_11
	float r = material.roughness;
#else
	float r = PerMeshBuffer.material.roughness;
#endif
	
	return float4(r,r,r,r);
}

// INPUT:
// 	input: the input data for the instanced buffer
// OUTPUT:
// 	the world matrix of the current instance
matrix instancedWorldMatrix(vsin0_inst input)
{
	matrix wM;
    wM._m00_m10_m20_m30 = input.vRowX.xyzw;
    wM._m01_m11_m21_m31 = input.vRowY.xyzw;
    wM._m02_m12_m22_m32 = input.vRowZ.xyzw;
    wM._m03_m13_m23_m33 = input.vRowW.xyzw;
	return wM;
}

// calculates the light's distance vector depending on its type
// INPUT: 
// 	inPos: the pixel's world position
// 	light: the light to calculate the light vector for
// OUTPUT:
// 	light vector, which is the negative of the light direction
float4 calculateLightVector(float4 inPos, Light light)
{
	uint type = 1 * light.type;
	float4 lDir = float4(0.f, 0.f, 0.f, 0.f);
	if (type != POINTLIGHT)
		lDir = light.dir;
	else 
		lDir = inPos - light.pos;
	return -normalize(lDir);
}

// calculates the squared light distance
// INPUT:
// 	inPos: the pixel's world position
// 	light: the light to calculate the distance for
// OUTPUT:
// 	the squared distance from the light to the pixel
float lightDistance(float4 inPos, Light light)
{
	uint type = 1 * light.type;
	if (type != DIRECTIONALLIGHT)
	{
		float4 dist = float4(inPos.xyz - light.pos.xyz, 0.f);
		return length(dist);
	}
	return 0.f;
}

float calculateAttenuation(float d, Light light)
{
	uint type = 1 * light.type;
	if (type != DIRECTIONALLIGHT)
	{
		return saturate((light.falloffEnd - d) / (light.falloffEnd - light.falloffBegin));
	}
	return 1.f;
}

// INPUT:
// 	R0: Index of refraction (from material or refraction map)
// 	normal: the normal vector (from vertex data or normal map)
// 	lightVec: the vector from the light source to the pixel
float4 SchlickFresnel(float R0, float4 normal, float4 lightVec)
{
	float f0 = pow(1.f - saturate(dot(normal, lightVec)), 5);
	return R0 + (1.f - R0) * f0;
}

// INPUT:
// 	Kd: pixel color (from diffuse map or material)
// 	light: the light source to calculate BlinnPhong for
// 	R: roughness (from roughness map or material)
// 	IOR: index of reflection (from reflection map or material)
// 	normal: normal vector (from normal map or vertex data)
// 	view: the normalized vector from the camera to the pixel
// 	worldPos: the world position of the current pixel
float4 BlinnPhong(float4 Kd, float4 lightVec, float4 lightStrength, float R, float IOR, float4 normal, float4 view)
{
	float m = max((1.f - R) * 256.f, 1.f);
	float4 halfVec = normalize(lightVec - view);
	float rFactor = (m + 8.f) * pow(max(dot(halfVec, normal), 0.f), m) / 8.f;
	float4 fresnel = SchlickFresnel(R, normal, lightVec);
	float4 specular = fresnel * rFactor;
	specular /= (specular + 1.f);
	return (Kd + specular) * lightStrength;
}

// INPUT:
// 	vertexColor: 
float4 accumulateLight(float4 vertexColor, float4 vertexNormal, float4 worldPos, float4 view, float roughness, float IOR)
{
#ifdef ORBIT_DIRECTX_11
	float4 color = vertexColor * scene.ambientLight;
	for (uint j = 0; j < scene.numLights; ++j) {
		Light light = lights[j];
		if (!light.enabled)
			continue;
#else
	float4 color = vertexColor * PerFrameBuffer.scene.ambientLight;
	for (uint j = 0; j < PerFrameBuffer.scene.numLights; ++j) {
		Light light = PerFrameBuffer.lights[j];
#endif
		float distance = lightDistance(worldPos, light);
		float factor = calculateAttenuation(distance, light);
		if (factor <= 0.01f)
			continue;
		
		float4 lDir = calculateLightVector(worldPos, light);
		float ndotl = saturate(dot(vertexNormal, lDir));
		float4 lightStrength = light.color * ndotl * factor;
		color += BlinnPhong(vertexColor, lDir, lightStrength, roughness, IOR, vertexNormal, view);
	}
	return color;
}

#endif