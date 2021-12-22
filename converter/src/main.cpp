#include "implementation/misc/Logger.hpp"
#include "orb/OrbFile.hpp"
#include "ArgumentParser.hpp"
#include "Helper.hpp"

using namespace orbtool;

void Run(const ArgumentParser& parser);

static constexpr auto CMD_ANALYZE = 0;
static constexpr auto CMD_APPEND  = 1;
static constexpr auto CMD_WRITE   = 2;
static constexpr auto CMD_UPDATE  = 3;

int main(int argc, const char** argv)
{
	ArgumentParser parser;
	parser.RegisterArgument("Output file to be written.", "output", "o");
	parser.RegisterArgument("Input file to be modified or viewed.", "input", "i");
	parser.RegisterFlag("Analyze an input file. Incompatible with -output.", "analyze", "a");
	parser.RegisterArgument("Specify an item with its Resource-ID to show its details. Can only be used with -a.", "item", "I");
	parser.RegisterFlag("Appends data from the input file to the output file.", "append", "A");
	parser.RegisterArray("External data input file", "external", "e");
	parser.RegisterFlag("Writes a new file from an input file", "write", "w");
	parser.RegisterFlag("Update a file to the most recent parser version", "update", "u");
	parser.RegisterFlag("Automatically triangulate quads (naiv triangulation).", "triangulate", "t");
	parser.RegisterValidConfigurations(
		{ 
			"011X00000", // Analyzing a file, CMD_ANALYZE
			"10001100X", // Append a file, CMD_APPEND
			"10000110X", // Write a new file, CMD_WRITE
			"010000010", // Update an orb file to the newest version, CMD_UPDATE
		}
	);
	parser.WarnOnInvalid(true);
	parser.WarnOnUnknownSwitch(true);
	parser.AllowFreeArguments(false);
	parser.SetExecutableName("orbtool");
	if (parser.ParseArguments(argc, argv))
	{
#ifndef _DEBUG
		try {
#endif
			Run(parser);
#ifndef _DEBUG
		}
		catch (std::exception& e) {
			ORBIT_LOG(e.what());
			return -1;
		}
#endif
		return 0;
	}
}

void Run(const ArgumentParser& parser)
{
	auto config = parser.GetConfiguration();
	if (config == CMD_ANALYZE)
	{
		// Analyze the input file
		auto input = *parser.GetSwitch("input");
		auto itemStr = parser.GetSwitch("item");

		Do_Analyze(input, itemStr ? *itemStr : nullptr);
	}
	else if (config == CMD_WRITE || config == CMD_APPEND)
	{
		auto orbfile = *parser.GetSwitch("output");
		uint32_t numFiles = 0u;
		auto externalFiles = parser.GetSwitch("external", &numFiles);

		Do_WriteAppend(externalFiles, numFiles, orbfile, config == CMD_APPEND, parser.GetSwitch("triangulate") != nullptr);
	}
	else if (config == CMD_UPDATE)
	{
		auto input = *parser.GetSwitch("input");
		OrbFile file;
		file.UpdateFile(input);
	}
	else
	{
		ORBIT_ERROR("Unknown command line argument combination");
		return;
	}

	ORBIT_LOG("Done.");
	return;
}