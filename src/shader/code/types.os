#if defined(ORBIT_DIRECTX_11) || defined(ORBIT_DIRECTX_12)

typedef ORBIT_FLOAT4 float4;
typedef ORBIT_FLOAT3 float3;
typedef ORBIT_FLOAT2 float2;
typedef ORBIT_FLOAT1 float1;
typedef ORBIT_FLOAT ORBIT_FLOAT1;

#else if defined(ORBIT_OPENGL)

#define ORBIT_FLOAT4 vec4;
#define ORBIT_FLOAT3 vec3;
#define ORBIT_FLOAT2 vec2;
#define ORBIT_FLOAT1 vec1;
#define ORBIT_FLOAT float;

#endif