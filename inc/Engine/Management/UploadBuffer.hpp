#ifdef ORBIT_DX12
#include "Engine/Bindings/DX12/UploadBuffer.hpp"
#elif defined ORBIT_DX11
#include DX11_ERROR_INCLUDE
#elif defined ORBIT_OPENGL
#include OPENGL_ERROR_INCLUDE
#endif
