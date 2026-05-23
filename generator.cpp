#include "geometry.hpp"
#include "perlin.hpp"
#include <ctime>
#include <iostream>
#include <vector>

enum Feature {
    FEATURE_WALL_GLITCH = 0,
    FEATURE_AMBIENT_SOUND = 1,
    FEATURE_ROTATION = 2,
    FEATURE_TELEPORTS = 3,
    FEATURE_MONSTERS = 4,
    FEATURE_FLOOD = 5,
    FEATURE_LIGHT_FLICKER = 6,
    FEATURE_COUNT = 7
};

void generateMap(std::string name, float chaos, float strangeness, std::string seedString) {
    std::ofstream map(name + ".map");
    if (!map.is_open()) return;

    srand(time(0)); 
    
    map << "{\n\"classname\" \"worldspawn\"\n\"mapversion\" \"220\"\n\"MaxRange\" \"4096\"\n";
    map << "\"wad\" \"halflife.wad;cstrike.wad\"\n"; // Локальные WAD

    float worldSize = 2500.0f;
    makeSkybox(map, worldSize);

    std::vector<bool> activeFeatures(FEATURE_COUNT, false);
    
    std::vector<std::string> textures = {"CONCRETE", "LAB_FLR1", "LAB_WALL01", "OUT_W3", "SKY"};
    std::string wallTexture = "CONCRETE";

    int offset = (int)strangeness;
    bool forceWeapons = false;

    for (char c : seedString) {
        int baseIndex = -1;
        if (c == 'L' || c == 'l') baseIndex = 0;
        else if (c == 'H' || c == 'h') baseIndex = 1;
        else if (c == 'R' || c == 'r') baseIndex = 2;
        else if (c == 'T' || c == 't') baseIndex = 3;
        else if (c == 'M' || c == 'm') baseIndex = 4;
        else if (c == 'W' || c == 'w') {
            baseIndex = 5;
            forceWeapons = true;
        }
        else if (c == 'F' || c == 'f') baseIndex = 6;

        if (baseIndex != -1) {
            int mutatedIndex = (baseIndex + offset) % FEATURE_COUNT;
            activeFeatures[mutatedIndex] = true;
        }

        if (c >= '1' && c <= '3') {
            int texBase = c - '1'; 
            int mutatedTex = (texBase + offset) % textures.size();
            wallTexture = textures[mutatedTex];
        }
    }

    float roomSize = 256.0f;
    float wallThick = 16.0f;  
    float roomHeight = 144.0f; 

    for (int gx = -4; gx <= 4; gx++) {
        for (int gy = -4; gy <= 4; gy++) {
            float x1 = gx * roomSize;
            float y1 = gy * roomSize;
            float x2 = x1 + roomSize;
            float y2 = y1 + roomSize;

            makeBrick(map, {x1, y1, -16}, {x2, y2, 0}, "LAB_FLR1");
            makeBrick(map, {x1, y1, roomHeight}, {x2, y2, roomHeight + wallThick}, "LAB_WALL01");

            bool skipNorth = activeFeatures[FEATURE_WALL_GLITCH] && (rand() % 100 < (30 + chaos * 10));
            bool skipWest  = activeFeatures[FEATURE_WALL_GLITCH] && (rand() % 100 < (30 + chaos * 10));

            if (!skipNorth && gy < 4) makeBrick(map, {x1, y2 - wallThick, 0}, {x2, y2, roomHeight}, wallTexture);
            if (!skipWest && gx < 4)  makeBrick(map, {x2 - wallThick, y1, 0}, {x2, y2, roomHeight}, wallTexture);

            // Статичные колонны
            if (!activeFeatures[FEATURE_ROTATION] && (rand() % 100 < (15 * chaos))) {
                float cx = x1 + roomSize / 2.0f - 16.0f;
                float cy = y1 + roomSize / 2.0f - 16.0f;
                makeBrick(map, {cx, cy, 0}, {cx + 32, cy + 32, roomHeight}, wallTexture);
            }
        }
    }

    map << "}\n";

    if (activeFeatures[FEATURE_FLOOD]) {
        float waterHeight = 12.0f + (strangeness * 10.0f);
        makeBrushEntity(map, "func_water", {-1100, -1100, 0}, {1100, 1100, waterHeight}, "!WATERRED", {
            {"skin", "-3"},
            {"wave", "3"}
        });
    }

    if (activeFeatures[FEATURE_ROTATION]) {
        for (int gx = -3; gx <= 3; gx++) {
            for (int gy = -3; gy <= 3; gy++) {
                if (rand() % 100 < 20) {
                    float cx = gx * roomSize + roomSize / 2.0f - 16.0f;
                    float cy = gy * roomSize + roomSize / 2.0f - 16.0f;
                    makeBrushEntity(map, "func_rotating", {cx, cy, 0}, {cx + 32, cy + 32, roomHeight}, wallTexture, {
                        {"speed", std::to_string((int)(20 + (strangeness * 10)))},
                        {"spawnflags", "4"} 
                    });
                }
            }
        }
    }

    std::string lightStyle = "0";
    if (activeFeatures[FEATURE_LIGHT_FLICKER]) {
        lightStyle = "10"; 
    }

    // Расставляем лампы по сетке комнат
    for(int i = -2; i <= 2; i += 2) {
        for(int j = -2; j <= 2; j += 2) {
            makePointEntity(map, "light", {i * roomSize, j * roomSize, 100}, {
                {"_light", "240 240 150 45"},
                {"style", lightStyle}
            });
        }
    }

    if (chaos > 3.5f) {
        makePointEntity(map, "env_shake", {0, 0, 64}, {
            {"amplitude", std::to_string((int)(chaos * 2))},
            {"duration", "5"},
            {"frequency", "2.5"},
            {"radius", "2000"},
            {"spawnflags", "1"}
        });
    }

    if (activeFeatures[FEATURE_TELEPORTS]) {
        int tCount = (int)chaos + 2;
        for (int i = 0; i < tCount; i++) {
            float tx = (rand() % 1600) - 800; float ty = (rand() % 1600) - 800;
            std::string tName = "trap_" + std::to_string(i);
            makePointEntity(map, "info_teleport_destination", {0, 0, 64}, {{"targetname", tName}});
            makeBrushEntity(map, "trigger_teleport", {tx, ty, 0}, {tx + 64, ty + 64, 48}, "AAATRIGGER", {{"target", tName}});
        }
    }

    if (activeFeatures[FEATURE_MONSTERS]) {
        int mCount = (int)(chaos * 2) + 1;
        for(int i = 0; i < mCount; i++) {
            float mx = (rand() % 1600) - 800; float my = (rand() % 1600) - 800;
            makePointEntity(map, "monster_alien_slave", {mx, my, 16}); 
        }
    }

    makePointEntity(map, "info_player_start", {0, 0, 32});

    std::vector<std::string> kit;
    if (forceWeapons || chaos > 3.0f) {
        kit.push_back("weapon_9mmAR"); kit.push_back("ammo_9mmAR"); kit.push_back("weapon_shotgun");
        kit.push_back("ammo_buckshot"); kit.push_back("weapon_rpg"); kit.push_back("ammo_rpgclip");
        kit.push_back("item_battery"); kit.push_back("item_suit");
    } else if (chaos > 1.5f) {
        kit.push_back("weapon_crowbar"); kit.push_back("weapon_9mmhandgun"); kit.push_back("ammo_9mmclip"); kit.push_back("item_suit");
    }
    if (!kit.empty()) makePlayerEquipment(map, kit);

    if (activeFeatures[FEATURE_AMBIENT_SOUND]) {
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                makeAmbientSound(map, {i * roomSize * 2, j * roomSize * 2, 100}, "ambience/industrial4.wav", 0.6f);
            }
        }
    }
    
    map.close();
    std::cout << "DeltaBox Engine: 7-Feature matrix grid compiled successfully.\n";
}
