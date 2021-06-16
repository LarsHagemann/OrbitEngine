#pragma once

#define PX_RELEASE(x) if(x) { x->release(); x = nullptr; }


// Nvidia PhysX matrices are column-major
// Eigen matrices are by default column-major
// HLSL expects column-major matrices
// GLSL expects column-major matrices
// DirectXMath uses row-major matrices
// 