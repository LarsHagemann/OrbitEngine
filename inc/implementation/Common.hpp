#pragma once
#include <cstdint>
#include <ostream>
#include <filesystem>
#include <memory>
#include <Eigen/Dense>

#ifndef ORBTOOL_CONV
#include <foundation/PxVec3.h>
#include <characterkinematic/PxExtended.h>
#include <foundation/PxQuat.h>
#endif

#if defined _DEBUG

extern const char* monitor_file;
extern unsigned    monitor_line;

struct AllocEntry
{
	const void* alloc;
	const char* file;
	unsigned line;
	unsigned size;
};

struct AllocDictionary {
	AllocEntry* allocs;
	unsigned size;
	unsigned capacity;
};

static AllocDictionary alloc_dict;

extern bool prepare_monitoring();

static const auto monitoring_prepared = prepare_monitoring();

extern void grow_dictionary();

extern void shutdown_monitoring();

extern void log_allocations();

extern void monitor_allocation(const void* alloc, unsigned size, const char* file, unsigned line);

extern void monitor_deallocation(const void* alloc);

extern bool prepare_alloc(const char* file, unsigned line);

inline void* operator new(std::size_t size)
{
	auto alloc = malloc(size);
    auto fLen = strlen(monitor_file);
    char* file = (char*)malloc(fLen);
    memcpy(file, monitor_file, fLen);
	monitor_allocation(alloc, size, file, monitor_line);
	return alloc;
}

inline void operator delete(void* alloc)
{
	monitor_deallocation(alloc);
	free(alloc);
}

#define new prepare_alloc(__FILE__, __LINE__) ? 0 : new

#else

static void prepare_monitoring()
{
}

static void shutdown_monitoring()
{
}

#endif


#if defined (ORBIT_DIRECTX_11) || (ORBIT_DIRECTX_12)

#include <Windows.h>
#include <wrl/client.h> 

namespace orbit
{
	namespace wrl = Microsoft::WRL;

	template<typename COM>
	using ComPtr = wrl::ComPtr<COM>;
}

#endif

#include <DirectXMath.h>

namespace orbit
{
#if defined (ORBIT_DIRECTX_11) || (ORBIT_DIRECTX_12)
    using namespace DirectX;
#endif
  
    using namespace Eigen;
    namespace fs = std::filesystem;

#ifndef ORBTOOL_CONV

    using namespace physx;

#endif

    // Objects in Orbit are identified by their unique-ID. 
    using ResourceId = uint64_t;

    // A Slot can have context-dependant meaning. I.e. a constant buffer slot for a 
    // shader resource. Or a Slot in the input layout.
    using Slot = uint32_t;

    class OpenGL_4_0 {};
    class DirectX_11 {};
    class DirectX_12 {};

    using Index = size_t;
    using Offset = size_t;

    using ShaderIndex = uint32_t;
    using MaterialFlags = uint32_t;

    using Byte = uint8_t;

    using Roughness = float;

    template<typename T>
    using SPtr = std::shared_ptr<T>;
    template<typename T>
    using UPtr = std::unique_ptr<T>;

    template<typename T>
    class Math
    {
    public:
        static constexpr T PI     = static_cast<T>(3.141592653589793238462643383279);
        static constexpr T PIDIV4 = static_cast<T>(PI * .25);
        static constexpr T PIDIV2 = static_cast<T>(PI * .5);
        static constexpr T TWO_PI = static_cast<T>(PI * 2.);
        
        static constexpr T AlignUp(const T& in, const T& alignment)
        {
            static_assert(std::is_integral<T>::value && "Math<T>::AlignUp() T must be integral type.");
            return (in + alignment - 1) & ~(alignment - 1);
        }
        static constexpr T AlignDown(const T& in, const T& alignment)
        {
            static_assert(std::is_integral<T>::value && "Math<T>::AlignDown() T must be integral type.");
            return in & ~(alignment - 1);
        }
        static constexpr Matrix<T, 4, 4> LookAt(
            const Matrix<T, 3, 1>& eye,
            const Matrix<T, 3, 1>& target,
            const Matrix<T, 3, 1>& up)
        {            
            auto zaxis = (target - eye).normalized();
            auto xaxis = up.cross(zaxis).normalized();
            auto yaxis = zaxis.cross(xaxis);
            
            Matrix<T,4,4> matrix = Matrix<T, 4, 4>::Zero();
            matrix.block<3, 1>(0, 0) = xaxis;
            matrix.block<3, 1>(0, 1) = yaxis;
            matrix.block<3, 1>(0, 2) = zaxis;
            matrix(3, 0) = -xaxis.dot(eye);
            matrix(3, 1) = -yaxis.dot(eye);
            matrix(3, 2) = -zaxis.dot(eye);
            matrix(3, 3) = 1;

            return matrix;
        }
        static constexpr Matrix<T, 4, 4> Perspective(
            T vFOV,
            T aspectRatio,
            T nearZ,
            T farZ)
        {
            const auto d = 1.f / (nearZ - farZ);
            const auto a = 1.f / tan(.5f * vFOV);

            Matrix4f m = Matrix4f::Zero();
            m(0, 0) = a / aspectRatio;
            m(1, 1) = a;
            m(2, 2) = -(nearZ + farZ) * d;
            m(3, 2) = nearZ * farZ * d;
            m(2, 3) = 1;
            return m;
        }

        static constexpr Matrix<T, 4, 4> Orthogonal(T nearZ, T farZ, Vector4f normal)
        {
            const auto d = 1.f / (nearZ - farZ);
            Matrix4f m = Matrix4f::Identity() - normal * normal.transpose();
            m(2, 2) = -(nearZ + farZ) * d;
            m(3, 2) = nearZ * farZ * d;
            m(2, 3) = 1;
            return m;
        }

#ifndef ORBTOOL_CONV

        static constexpr PxVec3 EigenToPx3(const Vector3<T>& in)
        {
            return PxVec3(
                static_cast<float>(in.x()),
                static_cast<float>(in.y()),
                static_cast<float>(in.z())
            );
        }

        static constexpr Vector3<T> PxToEigen(const PxVec3& in)
        {
            return Vector3<T>{
                static_cast<T>(in.x),
                static_cast<T>(in.y),
                static_cast<T>(in.z)
            };
        }

        static constexpr Quaternion<T> PxToEigen(const PxQuat& in)
        {
            return Quaternion<T>{
                static_cast<T>(in.x),
                static_cast<T>(in.y),
                static_cast<T>(in.z),
                static_cast<T>(in.w)
            };
        }

        static constexpr Vector3<T> PxToEigen(const PxExtendedVec3& in)
        {
            return Vector3<T>{
                static_cast<T>(in.x),
                static_cast<T>(in.y),
                static_cast<T>(in.z)
            };
        }

        static constexpr PxQuat EigenToPx(const Quaternion<T>& in)
        {
            return PxQuat(
                static_cast<float>(in.x()),
                static_cast<float>(in.x()),
                static_cast<float>(in.z()),
                static_cast<float>(in.w())
            );
        }
#endif

    };

}

#ifdef ORBIT_OPENGL
  #ifndef GL_GLEXT_PROTOTYPES
    #define GL_GLEXT_PROTOTYPES 1
  #endif

  #include <GL/glut.h>
  #include <GL/gl.h>
#elif defined(ORBIT_DIRECTX_11)
  #include <d3d11.h>
  //#include <d3dx11.h>
#elif defined(ORBIT_DIRECTX_11)
  #include <d3d12.h>
  #include <d3dx12.h>
#endif