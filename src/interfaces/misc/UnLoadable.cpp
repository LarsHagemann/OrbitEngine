#include "interfaces/misc/UnLoadable.hpp"
#include "implementation/engine/Engine.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbit
{

    bool UnLoadable::Load()
    {
        if (m_isLoaded)
            return true;

        if (m_id == 0)
        {
            if (LoadImpl(nullptr))
            {
                m_isLoaded = true;
                return true;
            }
            return false;
        }
        std::ifstream stream;
        if (!ENGINE->RMGetStream(m_id, &stream) ||
            !LoadImpl(&stream))
            return false;

        m_isLoaded = true;
        return true;
    }

    void UnLoadable::Unload()
    {
        UnloadImpl();
        m_isLoaded = false;
    }

    ResourceId UnLoadable::ReadReferenceId(std::ifstream* stream)
    {
        int64_t reference = 0u;
        stream->read((char*)&reference, sizeof(ResourceId));
        return GetId() + reference;
    }

}