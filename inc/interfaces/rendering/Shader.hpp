#pragma once
#include "implementation/Common.hpp"
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

#include <d3dcompiler.h>

namespace orbit
{

    class IShaderBase : public IBindable<>, public UnLoadable
    {
    public:
        // @brief: Compiles a file into the backends format
        // @param shaderpath: path to the shader code
        // @param entry_point: entry point function name. always "main" for OpenGL backends
        // @return: false on error
        virtual bool CompileFile(const fs::path& shaderpath, const std::string& entry_point) = 0;
        // @brief: Loads a precompiled shader binary
        // @param shaderpath: path to the binary
        // @return: false on error
        virtual bool LoadBinary(const fs::path& shaderpath) = 0;
        // @brief: Loads a precompiled shader binary
        // @param binary: the binary code
        // @param binaryLength: length of the code
        // @return: false on error
        virtual bool LoadBinary(const char* binary, uint32_t binaryLength) = 0;
            
        bool LoadImpl(std::ifstream* stream) override;
    };


}