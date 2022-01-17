#ifndef _HLSLI_ORBIT_IO
#define _HLSLI_ORBIT_IO

// INPUT LAYOUT FOR THE INSTANCED DEFAULT SHADER
struct vsin0_inst
{
	float3 pos       : POSITION;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float2 texcoords : TEXCOORD;
	float4 vRowX     : WORLDROW0;
	float4 vRowY     : WORLDROW1;
	float4 vRowZ     : WORLDROW2;
	float4 vRowW     : WORLDROW3;
};

struct psin0
{
	float4 c_screen  : SV_POSITION;
	float4 c_world	 : POSITION;
	float4 normal    : NORMAL;
	float2 texcoords : TEXCOORD;
	float4 tangent   : TANGENT;
};

typedef psin0 vsout0;

struct vsin1_inst
{
	float3 pos       : POSITION;
	float4 color     : COLOR;
	float4 vRowX     : WORLDROW0;
	float4 vRowY     : WORLDROW1;
	float4 vRowZ     : WORLDROW2;
	float4 vRowW     : WORLDROW3;
};

struct psin1
{
	float4 c_screen  : SV_POSITION;
	float4 c_world	 : POSITION;
	float4 color     : COLOR;
};

typedef psin1 vsout1;

#endif