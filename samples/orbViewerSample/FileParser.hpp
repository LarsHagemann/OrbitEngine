#pragma once
#include <filesystem>
#include <vector>

#include "implementation/misc/ResourceHeader.hpp"

class FileParser
{
private:
    std::vector<orbit::ResourceHeader> m_entities;
public:
    bool ParseFile(const std::filesystem::path& path);
    const std::vector<orbit::ResourceHeader>& GetEntities() const { return m_entities; }
};
