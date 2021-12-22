#pragma once
#include "orb/OrbIntermediate.hpp"

#include <cstdint>
#include <filesystem>

namespace orbtool
{

    namespace fs = std::filesystem;

    extern void Do_Analyze(const char* file, const char* item);
    extern void Do_ReadFile(const fs::path& file, OrbIntermediate* intermediate, bool triangulateMeshes = false);
    extern void Do_WriteAppend(const char*const* files, uint32_t numFiles, const char* output, bool append, bool triangulateMeshes = false);

}