#ifndef _HLSLI_ORBIT_LIGHT
#define _HLSLI_ORBIT_LIGHT

//<--------LIGHT TYPES ----------->//
#define DIRECTIONALLIGHT 0
#define POINTLIGHT 1
#define SPOTLIGHT 2
//<--------LIGHT TYPES ----------->//

#define MAX_LIGHTS 100

struct Light
{
	float4 color;					// light intensity and color
	float4 pos;						// light position  (only spot and point light)
	float4 dir;						// light direction (only directional and spot)
	float angle; 					// angle of the spotlight (only spot light)
	float falloffBegin;				// Distance fall-off begin
	float falloffEnd;				// Distance fall-off end
	uint type;						// light type
};

#endif