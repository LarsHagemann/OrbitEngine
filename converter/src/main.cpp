#include <string>
#include <iostream>
#include "convert.hpp"
#include "orb/orb_type.hpp"

/*
TODO:
    Implement obj (wavefront) converting
    Implement dae (collada) converting

*/

void PrintVersion()
{
    std::cout 
        << "orbtool " << orbit::tool_version
        <<  " - (c) 2020 Lars Hagemann\n"
            "This program converts model files (fbx, obj, dae) into the Orbit game engine file format\n"
            "See https://github.com/LarsHagemann/OrbitFileFormat for the latest specification\n"
            "Use '>orbtool ?' for help\n\n";
}

void ShowAbout()
{

}

int main(int argc, const char* argv[])
{
    PrintVersion();
	auto show_help = argc <= 1 || !strcmp(argv[1], "?") || !strcmp(argv[1], "help");
    if (show_help)
    {
        ShowUsage();
        return 0;
    }
	else
	{
		auto cmd = argv[1];
        try {
            if (!strcmp(cmd, "-fbx"))
                ConvertFBX(argc - 2, argv + 2);
            else if (!strcmp(cmd, "-obj"))
                ConvertOBJ(argc - 2, argv + 2);
            else if (!strcmp(cmd, "-dae"))
                ConvertDAE(argc - 2, argv + 2);
            else if (!strcmp(cmd, "-a"))
                AnalyzeOrb(argc - 2, argv + 2);
            else if (!strcmp(cmd, "-version"))
                ShowAbout();
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
        }
	}
}
