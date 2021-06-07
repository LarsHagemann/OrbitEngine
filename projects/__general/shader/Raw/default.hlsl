#include "orbit.hlsli"
#include "helper.hlsli"
#include "io.hlsli"
#include "textures.hlsli"

// minimum shader version: 5.1
[RootSignature(OrbitDefaultRS)]
vsout0 vs_default(vsin0_inst input)
{
	vsout0 output = (vsout0)0;
	float4 pos = float4(input.pos, 1.f);
	matrix worldMatrix = instancedWorldMatrix(input);
	matrix transform = mul(mul(worldMatrix, PerFrameBuffer.viewMatrix), PerFrameBuffer.projectionMatrix);
	float3x3 nTransform = calculateNormalTransformation(worldMatrix);
	output.c_world = mul(pos, worldMatrix);
	output.c_screen = mul(pos, transform);
	//output.normal = transformNormal(float4(input.normal, 0.f), nTransform);
	output.normal = mul(input.normal, worldMatrix);
	output.texcoords = correctTextureY(input.texcoords);
	//output.tangent = transformNormal(float4(input.tangent, 0.f), nTransform);
	output.tangent = mul(input.tangent, worldMatrix);
	return output;
}

[RootSignature(OrbitDefaultRS)]
float4 ps_default(psin0 input) : SV_TARGET
{	
	return PerMeshBuffer.material.diffuseAlbedo;
	
	float4 color = getVertexOcclusion(input.texcoords);
	float4 view = -float4(normalize((PerFrameBuffer.scene.camera - input.c_world).xyz), 0.f);
	float4 vertexColor = getVertexColor(input.texcoords);
	float sW = vertexColor.w;
	float4 normal = getVertexNormal(input.texcoords, input.normal, input.tangent);
	float4 roughness = getVertexRoughness(input.texcoords);
	color *= accumulateLight(
		vertexColor,
		normal,
		input.c_world,
		view,
		roughness.y,
		1.6f
	);
	color.w = sW;
	return color;
}