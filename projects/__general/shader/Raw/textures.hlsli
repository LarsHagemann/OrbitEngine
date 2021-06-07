#ifndef _HLSLI_ORBIT_TEXTURES
#define _HLSLI_ORBIT_TEXTURES

// textures used by the orbit engine:
// 	- BaseColor
// 	- Normal map
// 	- Roughness
// 	- Ambient Occlusion

// In the future texture t0 will be the depth texture

Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D occlusionMap : register(t2);
Texture2D roughnessMap : register(t3);

SamplerState MinMagMipLinearClamp : register(s0);
SamplerState MinMagLinearMipPointClamp : register(s1);
SamplerState MinMagPointMipLinearClamp : register(s2);
SamplerState MinMagMipPointClamp : register(s3);
SamplerState MinMagMipAnisotropicClamp : register(s4);

SamplerState MinMagMipLinearWrap : register(s5);
SamplerState MinMagLinearMipPointWrap : register(s6);
SamplerState MinMagPointMipLinearWrap : register(s7);
SamplerState MinMagMipPointWrap : register(s8);
SamplerState MinMagMipAnisotropicWrap : register(s9);

#endif