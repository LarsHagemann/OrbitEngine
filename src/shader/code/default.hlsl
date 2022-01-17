#include "orbit.hlsli"
#include "helper.hlsli"
#include "io.hlsli"
#include "textures.hlsli"

#define ALPHA_THRESHOLD 0.05f

[RootSignature(OrbitDefaultRS)]
vsout0 vs_default(vsin0_inst input)
{
	vsout0 output = (vsout0)0;
	float4 pos = float4(input.pos, 1.f);
	matrix worldMatrix = instancedWorldMatrix(input);
#ifdef ORBIT_DIRECTX_11
	matrix transform = mul(mul(worldMatrix, viewMatrix), projectionMatrix);
#else
	matrix transform = mul(mul(worldMatrix, PerFrameBuffer.viewMatrix), PerFrameBuffer.projectionMatrix);
#endif
	float3x3 nTransform = calculateNormalTransformation(worldMatrix);
	output.c_world   = mul(pos, worldMatrix);
	output.c_screen  = mul(pos, transform);
	output.normal    = float4(normalize(mul(input.normal, nTransform)), 0);
	output.texcoords = correctTextureY(input.texcoords);
	output.tangent   = mul(float4(input.tangent, 0), worldMatrix);
	return output;
}

[RootSignature(OrbitDefaultRS)]
float4 ps_default(psin0 input) : SV_TARGET
{		
	float4 color = getVertexOcclusion(input.texcoords);
#ifdef ORBIT_DIRECTX_11
	float4 view = -float4(normalize((scene.camera - input.c_world).xyz), 0.f);
#else
	float4 view = -float4(normalize((PerFrameBuffer.scene.camera - input.c_world).xyz), 0.f);
#endif

	float4 vertexColor = getVertexColor(input.texcoords);

	float sW = vertexColor.w;
	if (sW < ALPHA_THRESHOLD)
		discard;

	float4 normal = getVertexNormal(input.texcoords, input.normal, input.tangent);
	float4 roughness = getVertexRoughness(input.texcoords);
	color *= accumulateLight(
		vertexColor,
		normal,
		input.c_world,
		view,
		roughness.x,
		1.6f
	);
	color.w = sW;
	return color;
}

[RootSignature(OrbitDefaultRS)]
vsout1 vs_solid_color(vsin1_inst input)
{
	vsout1 output = (vsout1)0;
	float4 pos = float4(input.pos, 1.f);
	matrix worldMatrix = instancedWorldMatrix(input);
#ifdef ORBIT_DIRECTX_11
	matrix transform = mul(mul(worldMatrix, viewMatrix), projectionMatrix);
#else
	matrix transform = mul(mul(worldMatrix, PerFrameBuffer.viewMatrix), PerFrameBuffer.projectionMatrix);
#endif
	float3x3 nTransform = calculateNormalTransformation(worldMatrix);
	output.c_world   = mul(pos, worldMatrix);
	output.c_screen  = mul(pos, transform);
	output.color = input.color;
	return output;
}

[RootSignature(OrbitDefaultRS)]
float4 ps_solid_color(psin1 input) : SV_TARGET
{		
	return input.color;
}
