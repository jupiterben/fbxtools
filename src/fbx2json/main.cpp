#include "./fbx2json.h"
#include <iostream>
#include <cxxopts.hpp>
#include <fstream>
#include <string>
typedef cxxopts::Options CmdOptions;

int main(int argc, char **argv)
{
    CmdOptions options(argv[0], " - FBX to JSON converter");
    options.add_options()
        ("help,h", "Print help")
        ("input,i", "Input FBX file", cxxopts::value<std::string>())
        ("output,o", "Output JSON file", cxxopts::value<std::string>())
        ("verbose,v", "Print verbose output");

    auto result = options.parse(argc, argv);
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }
    if (result.count("input") == 0)
    {
        std::cout << "Input file is required" << std::endl;
        return 1;
    }
    if (result.count("output") == 0)
    {
        std::cout << "Output file is required" << std::endl;
        return 1;
    }
    if (result.count("verbose"))
    {
        std::cout << "Verbose output" << std::endl;
    }
    std::string input = result["input"].as<std::string>();
    std::string output = result["output"].as<std::string>();

    FbxManager *pManager = nullptr;
    FbxScene *pScene = nullptr;
    InitializeSdkObjects(pManager, pScene);
    int fbxFileVersion = -1;
    if (LoadScene(pManager, pScene, input.c_str(), fbxFileVersion))
    {
        json j = exportScene(pScene);
        std::ofstream out(output);
        out << j.dump(4);
    }
    return 0;
}