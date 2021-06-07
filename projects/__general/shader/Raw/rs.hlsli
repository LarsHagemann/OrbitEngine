#ifndef _HLSLI_ORBIT_ROOT_SIGNATURE
#define _HLSLI_ORBIT_ROOT_SIGNATURE

#define OrbitDefaultRS  \
			"RootFlags( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | DENY_HULL_SHADER_ROOT_ACCESS | DENY_DOMAIN_SHADER_ROOT_ACCESS | DENY_GEOMETRY_SHADER_ROOT_ACCESS), " \
			"CBV(b0), " /* Per Frame Buffer */ \
			"CBV(b1), " /* Per Mesh Buffer */ \
			"DescriptorTable(" \
				"SRV(t0), " /* Color Texture */ \
				"SRV(t1), " /* Normal Texture */ \
				"SRV(t2), " /* Roughness Texture */ \
				"SRV(t3)), " /* Occlusion Texture */ \
			"StaticSampler(s0, " \
                             "addressU = TEXTURE_ADDRESS_CLAMP, " \
							 "addressV = TEXTURE_ADDRESS_CLAMP, " \
							 "addressW = TEXTURE_ADDRESS_CLAMP, " \
                             "filter = FILTER_MIN_MAG_MIP_LINEAR), " \
			"StaticSampler(s1, " \
                             "addressU = TEXTURE_ADDRESS_CLAMP, " \
							 "addressV = TEXTURE_ADDRESS_CLAMP, " \
							 "addressW = TEXTURE_ADDRESS_CLAMP, " \
                             "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT), " \
			"StaticSampler(s2, " \
                             "addressU = TEXTURE_ADDRESS_CLAMP, " \
							 "addressV = TEXTURE_ADDRESS_CLAMP, " \
							 "addressW = TEXTURE_ADDRESS_CLAMP, " \
                             "filter = FILTER_MIN_MAG_POINT_MIP_LINEAR), " \
			"StaticSampler(s3, " \
                             "addressU = TEXTURE_ADDRESS_CLAMP, " \
							 "addressV = TEXTURE_ADDRESS_CLAMP, " \
							 "addressW = TEXTURE_ADDRESS_CLAMP, " \
                             "filter = FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT), " \
			"StaticSampler(s4, " \
                             "addressU = TEXTURE_ADDRESS_CLAMP, " \
							 "addressV = TEXTURE_ADDRESS_CLAMP, " \
							 "addressW = TEXTURE_ADDRESS_CLAMP, " \
                             "filter = FILTER_ANISOTROPIC), " \
			"StaticSampler(s5, " \
                             "addressU = TEXTURE_ADDRESS_WRAP, " \
							 "addressV = TEXTURE_ADDRESS_WRAP, " \
							 "addressW = TEXTURE_ADDRESS_WRAP, " \
                             "filter = FILTER_MIN_MAG_MIP_LINEAR), " \
			"StaticSampler(s6, " \
                             "addressU = TEXTURE_ADDRESS_WRAP, " \
							 "addressV = TEXTURE_ADDRESS_WRAP, " \
							 "addressW = TEXTURE_ADDRESS_WRAP, " \
                             "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT), " \
			"StaticSampler(s7, " \
                             "addressU = TEXTURE_ADDRESS_WRAP, " \
							 "addressV = TEXTURE_ADDRESS_WRAP, " \
							 "addressW = TEXTURE_ADDRESS_WRAP, " \
                             "filter = FILTER_MIN_MAG_POINT_MIP_LINEAR), " \
			"StaticSampler(s8, " \
                             "addressU = TEXTURE_ADDRESS_WRAP, " \
							 "addressV = TEXTURE_ADDRESS_WRAP, " \
							 "addressW = TEXTURE_ADDRESS_WRAP, " \
                             "filter = FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT), " \
			"StaticSampler(s9, " \
                             "addressU = TEXTURE_ADDRESS_WRAP, " \
							 "addressV = TEXTURE_ADDRESS_WRAP, " \
							 "addressW = TEXTURE_ADDRESS_WRAP, " \
                             "filter = FILTER_ANISOTROPIC)"

#endif