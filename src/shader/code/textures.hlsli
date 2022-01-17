#ifndef _HLSLI_ORBIT_TEXTURES
#define _HLSLI_ORBIT_TEXTURES

// textures used by the orbit engine:
//  - depth texture
// 	- BaseColor
// 	- Normal map
// 	- Roughness
// 	- Ambient Occlusion

Texture2D depthTexture : register(t0);
Texture2D colorMap : register(t1);
Texture2D normalMap : register(t2);
Texture2D occlusionMap : register(t3);
Texture2D roughnessMap : register(t4);

SamplerState MinMagMipLinearClamp : register(s0)      { AddressU = Clamp; AddressV = Clamp; AddressW = Clamp; Filter = MIN_MAG_MIP_LINEAR; };
SamplerState MinMagLinearMipPointClamp : register(s1) { AddressU = Clamp; AddressV = Clamp; AddressW = Clamp; Filter = MIN_MAG_LINEAR_MIP_POINT; };
SamplerState MinMagPointMipLinearClamp : register(s2) { AddressU = Clamp; AddressV = Clamp; AddressW = Clamp; Filter = MIN_MAG_POINT_MIP_LINEAR; };
SamplerState MinMagMipPointClamp : register(s3)       { AddressU = Clamp; AddressV = Clamp; AddressW = Clamp; Filter = MIN_MAG_MIP_POINT; };
SamplerState MinMagMipAnisotropicClamp : register(s4) { AddressU = Clamp; AddressV = Clamp; AddressW = Clamp; Filter = ANISOTROPIC; };

SamplerState MinMagMipLinearWrap : register(s5)       { AddressU = Wrap ; AddressV = Wrap ; AddressW = Wrap ; Filter = MIN_MAG_MIP_LINEAR; };
SamplerState MinMagLinearMipPointWrap : register(s6)  { AddressU = Wrap ; AddressV = Wrap ; AddressW = Wrap ; Filter = MIN_MAG_LINEAR_MIP_POINT; };
SamplerState MinMagPointMipLinearWrap : register(s7)  { AddressU = Wrap ; AddressV = Wrap ; AddressW = Wrap ; Filter = MIN_MAG_POINT_MIP_LINEAR; };
SamplerState MinMagMipPointWrap : register(s8)        { AddressU = Wrap ; AddressV = Wrap ; AddressW = Wrap ; Filter = MIN_MAG_MIP_POINT; };
SamplerState MinMagMipAnisotropicWrap : register(s9)  { AddressU = Wrap ; AddressV = Wrap ; AddressW = Wrap ; Filter = ANISOTROPIC; };

#endif
