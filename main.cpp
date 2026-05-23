#include <iostream>
#include <string>
#include <cstdlib>
#include "geometry.hpp"
#include "generator.cpp"

int main() {
    std::string mapName = "procedural_map";
    // Укажи точный путь к своей папке valve/maps!

    float chaos, strangeness;
    std::string textSeed;
    
    std::cout << "  /\\ \n";
    std::cout << " /  \\     DeltaBox Engine v1.0\n";
    std::cout << "/_@@_\\    BSP GoldSource Map Generator\n\n";
    
    std::cout << "Enter DNA Seed (e.g. LH1): "; std::cin >> textSeed;
    std::cout << "Enter Chaos level (0.0 - 5.0): "; std::cin >> chaos;
    std::cout << "Enter Strangeness level (0.0 - 5.0): "; std::cin >> strangeness;

    std::cout << "\n[1/3] Triggering seed mutations..." << std::endl;
    generateMap(mapName, chaos, strangeness, textSeed);

    std::cout << "[2/3] Executing ZHLT compilers..." << std::endl;
    std::string compileCmd = "wine hlcsg.exe -nowadtextures " + mapName + " && " +
                         "wine hlbsp.exe " + mapName + " && " +
                         "wine hlvis.exe " + mapName + " && " +
                         "wine hlrad.exe " + mapName;
    
    int res = system(compileCmd.c_str());

    if (res != -1 && (res >> 8) == 0) {
        std::cout << "[3/3] Deployment successful! Syncing map..." << std::endl;
        std::cout << "\n--- DELTABOX GENERATION COMPLETE ---\n";
        std::cout << "Put " << mapName << ".bsp in half-life map foulder.\n";
    } else {
        std::cerr << "\n[!] Critical: ZHLT compilation failed!" << std::endl;
    }

    return 0;
}
