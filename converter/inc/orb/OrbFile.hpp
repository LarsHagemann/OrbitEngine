#pragma once
#include "implementation/misc/Version.hpp"
#include "implementation/engine/ResourceType.hpp"

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <string>

namespace orbtool
{

    using ResourceId = uint64_t;
    namespace fs = std::filesystem;

    class OrbIntermediate;

    using ResourceType = orbit::ResourceType;

    static const char* ResourceTypeToString(ResourceType type);

    class OrbFile
    {
    private:
        static constexpr orbit::Version sVersion = { 0, 0, 1 };
        struct Index
        {
            size_t       offset;
            ResourceType type;
        };
        struct ResourceHeader
        {
            ResourceId   id;
            ResourceType type;
            uint32_t     payloadSize;
            std::string  name;
        };
        std::unordered_map<ResourceId, Index> m_indices;
        fs::path m_filepath;
        uint64_t NextIndex() const;
    public:
        bool ParseFile(const fs::path& filepath);
        void PrintIndex() const;
        void PrintItemDetails(ResourceId itemId) const;
        void WriteIntermediate(const OrbIntermediate& orb, const fs::path& target) const;
        void UpdateFile(const fs::path& target) const;
    };

}