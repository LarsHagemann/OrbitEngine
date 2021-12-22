#ifndef _HLSLI_ORBIT
#define _HLSLI_ORBIT
#include "material.hlsli"
#include "scene.hlsli"
#include "light.hlsli"
#include "rs.hlsli"

// Constant buffer resources:
// b0: Frame resources
// 	the view matrix
// 	the projection matrix
// 	the scene buffer
// 	the light data
// b2: Mesh data
// 	material of the mesh

#ifdef ORBIT_DIRECTX_11

cbuffer PerFrameBuffer : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	Scene scene;
	Light lights[MAX_LIGHTS];
};

cbuffer PerMeshBuffer : register(b1)
{
	Material material;
};

#else

struct PerFrame
{
	matrix viewMatrix;
	matrix projectionMatrix;
	Scene scene;
	Light lights[MAX_LIGHTS];
};

struct PerMesh
{
	Material material;
};

ConstantBuffer<PerFrame> PerFrameBuffer : register(b0);
ConstantBuffer<PerMesh> PerMeshBuffer   : register(b1);

#endif

#endif














