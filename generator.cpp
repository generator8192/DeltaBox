#include "geometry.hpp"
#include <sstream>
#include <vector>
#include <iostream>

void generateMapFromAIInstructions(std::string name, const std::string& aiInstructions) {
    std::ofstream map(name + ".map");
    if (!map.is_open()) return;

    map << "{\n\"classname\" \"worldspawn\"\n\"mapversion\" \"220\"\n\"MaxRange\" \"4096\"\n";
    map << "\"wad\" \"halflife.wad\"\n";

    makeSkybox(map, 2000.0f);

    std::stringstream ss(aiInstructions);
    std::string line;

    // Вектора для энтити (запись после закрытия worldspawn)
    std::vector<std::string> PointEntitiesList;
    std::vector<std::string> BrushEntitiesList;
    std::vector<std::string> playerKit;

    // --- ФАЗА 1: СБОРКА СТРУКТУРНОЙ ГЕОМЕТРИИ (worldspawn) ---
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        std::stringstream lineStream(line);
        std::string command;
        lineStream >> command;

        if (command == "ROOM") {
            float x, y, z, w, l, h; std::string tex; int matType;
            lineStream >> x >> y >> z >> w >> l >> h >> tex >> matType;

            float t = 16.0f;
            makeBrick(map, {x - w/2, y - l/2, z - t}, {x + w/2, y + l/2, z}, tex);
            makeBrick(map, {x - w/2, y - l/2, z + h}, {x + w/2, y + l/2, z + h + t}, tex);

            if (matType == -1) {
                makeBrick(map, {x - w/2 - t, y - l/2, z}, {x - w/2, y + l/2, z + h}, tex); // Лево
                makeBrick(map, {x + w/2, y - l/2, z}, {x + w/2 + t, y + l/2, z + h}, tex); // Право
                makeBrick(map, {x - w/2, y - l/2 - t, z}, {x + w/2, y - l/2, z + h}, tex); // Перед
                makeBrick(map, {x - w/2, y + l/2, z}, {x + w/2, y + l/2 + t, z + h}, tex); // Зад
            } else {
                BrushEntitiesList.push_back(line);
            }
        }
        else if (command == "BOX") {
            float x, y, z, w, l, h; std::string tex;
            lineStream >> x >> y >> z >> w >> l >> h >> tex;
            makeBrick(map, {x - w/2, y - l/2, z}, {x + w/2, y + l/2, z + h}, tex);
        }
        else if (command == "ROTATING_BOX" || command == "LIGHT" || command == "MONSTER" || command == "EQUIP" || command == "WALL_ITEM") {
            continue;
        }
    }

    map << "}\n";


    makePointEntity(map, "info_player_start", {0, 0, 32});

    std::stringstream ss2(aiInstructions);
    while (std::getline(ss2, line)) {
        if (line.empty()) continue;
        std::stringstream lineStream(line);
        std::string command;
        lineStream >> command;

        if (command == "ROOM") {
            float x, y, z, w, l, h; std::string tex; int matType;
            lineStream >> x >> y >> z >> w >> l >> h >> tex >> matType;

            if (matType != -1) {
                float t = 16.0f;
                // Генерируем 4 брейкабла вокруг комнаты
                makeBrushEntity(map, "func_breakable", {x - w/2 - t, y - l/2, z}, {x - w/2, y + l/2, z + h}, tex, {{"health", "50"}, {"material", std::to_string(matType)}});
                makeBrushEntity(map, "func_breakable", {x + w/2, y - l/2, z}, {x + w/2 + t, y + l/2, z + h}, tex, {{"health", "50"}, {"material", std::to_string(matType)}});
                makeBrushEntity(map, "func_breakable", {x - w/2, y - l/2 - t, z}, {x + w/2, y - l/2, z + h}, tex, {{"health", "50"}, {"material", std::to_string(matType)}});
                makeBrushEntity(map, "func_breakable", {x - w/2, y + l/2, z}, {x + w/2, y + l/2 + t, z + h}, tex, {{"health", "50"}, {"material", std::to_string(matType)}});
            }
        }
        else if (command == "WALL_ITEM") {
            float x, y, z, w, l, h; std::string tex; int matType, spawnObj;
            lineStream >> x >> y >> z >> w >> l >> h >> tex >> matType >> spawnObj;

            makeBrushEntity(map, "func_breakable", {x - w/2, y - l/2, z}, {x + w/2, y + l/2, z + h}, tex, {
                {"health", "20"},
                {"material", std::to_string(matType)},
                {"spawnobject", std::to_string(spawnObj)}
            });
        }
        else if (command == "LIGHT") {
            float x, y, z, r, g, b, intensity; int style;
            lineStream >> x >> y >> z >> r >> g >> b >> intensity >> style;
            std::string colorStr = std::to_string((int)r) + " " + std::to_string((int)g) + " " + std::to_string((int)b) + " " + std::to_string((int)intensity);

            makePointEntity(map, "light", {x, y, z}, {{"_light", colorStr}, {"style", std::to_string(style)}});
        }
        else if (command == "ROTATING_BOX") {
            float x, y, z, w, l, h, speed; std::string tex;
            lineStream >> x >> y >> z >> w >> l >> h >> speed >> tex;
            makeBrushEntity(map, "func_rotating", {x - w/2, y - l/2, z}, {x + w/2, y + l/2, z + h}, tex, {
                {"speed", std::to_string((int)speed)}, {"spawnflags", "4"}
            });
        }
        else if (command == "MONSTER") {
            std::string type; float x, y, z;
            lineStream >> type >> x >> y >> z;
            makePointEntity(map, "monster_" + type, {x, y, z});
        }
        else if (command == "EQUIP") {
            std::string item;
            while (lineStream >> item) { playerKit.push_back(item); }
        }
    }

    if (!playerKit.empty()) {
        makePlayerEquipment(map, playerKit);
    }

    map.close();
}
