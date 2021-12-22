#pragma once
#include "orb/OrbIntermediate.hpp"
#include "Reader.hpp"

#include <filesystem>

namespace orbtool
{

    namespace fs = std::filesystem;

    class DaeReader : public Reader
    {
    private:
    public:
        bool ReadFile(const fs::path& filepath, OrbIntermediate* intermediate) override;
    };

}