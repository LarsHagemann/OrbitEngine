#include "FileParser.hpp"
#include "implementation/engine/ResourceManager.hpp"
#include "implementation/misc/Version.hpp"

#include <fstream>

using ResourceId = orbit::ResourceId;

bool FileParser::ParseFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::in);
    if (!file.is_open() || file.eof())
        return false;

    uint32_t numObjects  = 0;
    orbit::Version fileVersion = 0;

    file.read((char*)&fileVersion, sizeof(orbit::Version));
    if (orbit::ResourceManager::RMGetVersion() < fileVersion) {
        ORBIT_ERROR("File version %d is more recent than parser version (%d) in %s", 
            fileVersion.m_version, 
            orbit::ResourceManager::RMGetVersion().m_version, 
            path.generic_string().c_str()
        );
        return false;
    }

    file.read((char*)&numObjects, sizeof(uint32_t));
    orbit::ResourceHeader header;
    uint32_t nameLen = 0u;
    for (auto i = 0u; i < numObjects; ++i)
    {
        nameLen = 0u;
        file.read((char*)&header.id, sizeof(ResourceId));
        file.read((char*)&header.type, sizeof(orbit::ResourceType));
        file.read((char*)&header.payloadSize, sizeof(uint32_t));
        file.read((char*)&nameLen, sizeof(uint32_t));
        header.name.resize(nameLen);
        file.read(header.name.data(), nameLen);
        file.seekg(header.payloadSize, std::ios::cur);

        m_entities.emplace_back(std::move(header));
        
        if (file.bad())
            return false;
    }

    return true;
}