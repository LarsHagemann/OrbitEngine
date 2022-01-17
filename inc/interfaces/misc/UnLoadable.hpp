#pragma once
#include "implementation/Common.hpp"
#include "implementation/misc/ResourceHeader.hpp"

#include <fstream>

namespace orbit
{

    // Interface for resources that can be loaded and unloaded
    class UnLoadable
    {
    private:
        // @member: true if the object has been loaded
        bool m_isLoaded = false;
        ResourceId m_id;
    protected:
        virtual bool LoadImpl(std::ifstream* stream) = 0;
        virtual void UnloadImpl() = 0;
        ResourceId ReadReferenceId(std::ifstream* stream);
    public:
        // @method: Loads a resource (from file, from memory, ...)
        virtual bool Load();
        // @method: Unloads a resource (frees buffers, ...)
        virtual void Unload();

        // @member: returns true if the object is loaded
        bool IsLoaded() const { return m_isLoaded; }
        // @member: returns the object's id
        ResourceId GetId() const { return m_id; }
        // @member: sets the object's id
        void SetId(ResourceId id) { m_id = id; }
    };

}