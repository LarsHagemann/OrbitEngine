#pragma once
#include "orb/OrbIntermediate.hpp"
#include "Reader.hpp"

#include <filesystem>

namespace orbtool
{

    namespace fs = std::filesystem;

    class WFMaterialReader : public Reader
    {
    private:
        void ParseMaterial();
    public:
        bool ReadFile(const fs::path& filepath, OrbIntermediate* intermediate) override;
    };

}