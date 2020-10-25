#ifndef _HLSLI_ORBIT_SCENE
#define _HLSLI_ORBIT_SCENE

struct Scene
{
	float4 ambientLight;			// Ambient light			PACK(0)
	float4 camera;					// camera position			PACK(1)
	uint numLights;					// number of scene lights 	PACK(2)
	float gameTime;					// the current game time	PACK(2)
};

#endif