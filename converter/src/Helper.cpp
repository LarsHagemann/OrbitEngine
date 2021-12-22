#include "Helper.hpp"
#include "orb/OrbFile.hpp"
#include "implementation/misc/Logger.hpp"
#include "wavefront/MaterialReader.hpp"
#include "wavefront/ObjectReader.hpp"
#include "fbx/FbxReader.hpp"
#include "alembic/DaeReader.hpp"
#include "raw/RawReader.hpp"

namespace orbtool
{

    void Do_Analyze(const char* input, const char* item)
    {
        OrbFile file;
		if (!file.ParseFile(input))
			return;

		if (!item)
		{
			file.PrintIndex();
		}
		else
		{
			uint32_t itemId = strtoul(item, nullptr, 10);
			if (itemId == 0)
			{
				ORBIT_ERROR("Item-ID invalid: %s", item);
				return;
			}
			file.PrintItemDetails(itemId);
		}
    }

    void Do_ReadFile(const fs::path& file, OrbIntermediate* intermediate, bool triangulateMeshes)
    {
        ORBIT_LOG("Reading file %s", file.generic_string().c_str());
        if (file.extension() == ".mtl")
        {
            WFMaterialReader reader;
            if (!reader.ReadFile(file, intermediate))
            {
                ORBIT_ERROR("Failed to read file '%s'", file.generic_string().c_str());
                return;
            }
        }
        else if (file.extension() == ".obj")
        {
            WFObjectReader reader;
            if (triangulateMeshes)
                reader.WarnOnQuads(false);

            if (!reader.ReadFile(file, intermediate))
            {
                ORBIT_ERROR("Failed to read file '%s'", file.generic_string().c_str());
                return;
            }
        }
        else if (file.extension() == ".fbx")
        {
            FbxReader reader;
            if (triangulateMeshes)
                reader.WarnOnQuads(false);
            if (!reader.ReadFile(file, intermediate))
            {
                ORBIT_ERROR("Failed to read file '%s'", file.generic_string().c_str());
                return;
            }
        }
        else if (file.extension() == ".dae")
        {
            DaeReader reader;
            if (!reader.ReadFile(file, intermediate))
            {
                ORBIT_ERROR("Failed to read file '%s'", file.generic_string().c_str());
                return;
            }
        }
        else if (file.extension() == ".rorb")
        {
            RawReader reader;
            if (!reader.ReadFile(file, intermediate))
            {
                ORBIT_ERROR("Failed to read file '%s'", file.generic_string().c_str());
                return;
            }
        }
        else
        {
            ORBIT_ERROR("Unsupported file extension: %s", file.extension().generic_string().c_str());
            return;
        }
    }   

    void Do_WriteAppend(const char* const* files, uint32_t numFiles, const char* output, bool append, bool triangulateMeshes)
    {
        if (append)
		{
			ORBIT_ERROR("Appending does not work yet.");
			ORBIT_ERROR("For some reason the filestream decides to not overwrite data :'(");
			return;
		}

        OrbIntermediate intermediate;
		for (auto i = 0u; i < numFiles; ++i)
		{
			fs::path file = files[i];
			Do_ReadFile(file, &intermediate, triangulateMeshes);
		}
		OrbFile file;
		
		if (append)
			file.ParseFile(output);
		else if (fs::exists(output))
			fs::remove(output);

		file.WriteIntermediate(intermediate, output);
    }

}