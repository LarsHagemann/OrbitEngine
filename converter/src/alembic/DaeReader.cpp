#include "alembic/DaeReader.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbtool
{

    bool DaeReader::ReadFile(const fs::path& filepath, OrbIntermediate* orb)
    {
        if (!OpenFile(filepath))
            return false;

        ORBIT_THROW("Reader for this file type not implemented yet: '%s'", filepath.generic_string().c_str());
        
        return true;
    }

}