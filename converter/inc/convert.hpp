#pragma once
#include <iostream>
#include <vector>
#include <filesystem>

#include "fbx/fbx_read.hpp"
#include "fbx/fbx_convert.hpp"

namespace fs = std::filesystem;

void ShowUsage()
{
    std::cout << R"(! Usage

  orbtool -fbx <fbx_files> -o <output_orb>
  orbtool -obj <obj_files> -o <output_orb>
  orbtool -a <input_orb>
  orbtool -version

Switches
  -fbx <fbx_files> -o <output_orb>
    tells the converter to convert the following .fbx files
    into an orb file
    Example: >orbtool -fbx assets_0.fbx assets_1.fbx -o assets.orb

  -dae <dae_files> -o <output_orb>
    tells the converter to convert the following .dae files
    into an orb file
    Example: >orbtool -dae assets_0.dae assets_1.dae -o assets.orb

  -obj <obj_files> -o <output_orb>
    tells the converter to convert the following .obj files
    into an orb file
    Example: >orbtool -obj assets_0.obj assets_1.obj -o assets.orb

  -a <input_orb>
    prints information about the objects stored in the orb file
    helpful for debugging
    Example: >orbtool -a assets.orb

  -version
    prints information about the current orbtool version
)" << '\n';
}

bool MakeArgumentList(std::vector<fs::path>* inputs, fs::path* output, int argc, const char* argv[])
{
    if (!strcmp(argv[0], "-o"))
    {
        std::cout << "Expected at least one input file...\n";
        ShowUsage();
        return false;
    }

    auto i = 0;
    for (; i < argc && (strcmp(argv[i], "-o") != 0); ++i)
        inputs->emplace_back(argv[i]);

    if (i == argc || i + 1 == argc)
    {
        std::cout << "Expected -o <output_orb>...\n";
        ShowUsage();
        return false;
    }

    *output = argv[i + 1];
    return true;
}

void ConvertFBX(int argc, const char* argv[])
{
    if (argc == 0)
    {
        std::cout << "Expected fbx file(s) as argument(s)...\n";
        return ShowUsage();
    }

    std::vector<fs::path> fbx_files;
    fs::path output;
    if (!MakeArgumentList(&fbx_files, &output, argc, argv))
        return;

    orbit::Orb orb;
    for (const auto& file : fbx_files)
    {
        fbx::FBXNode root;
        root.name = "Root";
        fbx::GenerateFBXTree(&root, file);
        fbx::FBXToOrb(&root, &orb);
    }
    orb.ToFile(output);
}

void ConvertDAE(int argc, const char* argv[])
{
    if (argc == 0)
    {
        std::cout << "Expected dae file(s) as argument(s)...\n";
        return ShowUsage();
    }

    std::vector<fs::path> dae_files;
    fs::path output;
    if (!MakeArgumentList(&dae_files, &output, argc, argv))
        return;

    orbit::Orb orb;
    for (const auto& file : dae_files)
    {

    }
    orb.ToFile(output);
}

void ConvertOBJ(int argc, const char* argv[])
{
    if (argc == 0)
    {
        std::cout << "Expected obj file(s) as argument(s)...\n";
        return ShowUsage();
    }

    std::vector<fs::path> obj_files;
    fs::path output;
    if (!MakeArgumentList(&obj_files, &output, argc, argv))
        return;

    orbit::Orb orb;
    for (const auto& file : obj_files)
    {

    }
    orb.ToFile(output);
}

void AnalyzeOrb(int argc, const char* argv[])
{
    if (argc > 1)
    {
        std::cout << "Too many arguments...\n";
        ShowUsage();
    }
    else if (argc < 1)
    {
        std::cout << "Expected orb file as argument...\n";
        return ShowUsage();
    }

    // argc == 1
    auto input_orb = argv[0];

    try {
        // open orb file and show debug information
        orbit::OrbCompiler compiler;
        compiler.AnalyzeFile(input_orb);
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << '\n';
    }
}