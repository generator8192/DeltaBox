#include "geometry.hpp"
#include <cmath>

//void writeFace(std::ofstream& f, Vector p1, Vector p2, Vector p3, std::string tex) {
    //f << "( " << p1.x << " " << p1.y << " " << p1.z << " ) "
      //<< "( " << p2.x << " " << p2.y << " " << p2.z << " ) "
      //<< "( " << p3.x << " " << p3.y << " " << p3.z << " ) "
      //<< tex << " [ 1 0 0 0 ] [ 0 -1 0 0 ] 0 1.0 1.0 \n";
//}

void writeFace(std::ofstream& f, Vector p1, Vector p2, Vector p3, std::string tex) {
    float v1x = p2.x - p1.x; float v1y = p2.y - p1.y; float v1z = p2.z - p1.z;
    float v2x = p3.x - p1.x; float v2y = p3.y - p1.y; float v2z = p3.z - p1.z;

    float nx = (v1y * v2z) - (v1z * v2y);
    float ny = (v1z * v2x) - (v1x * v2z);
    float nz = (v1x * v2y) - (v1y * v2x);

    std::string s_axis = "[ 1 0 0 0 ]";
    std::string t_axis = "[ 0 -1 0 0 ]";

    if (std::abs(nx) > std::abs(ny) && std::abs(nx) > std::abs(nz)) {
        s_axis = "[ 0 1 0 0 ]";
        t_axis = "[ 0 0 -1 0 ]";
    }
    else if (std::abs(ny) > std::abs(nx) && std::abs(ny) > std::abs(nz)) {
        s_axis = "[ 1 0 0 0 ]";
        t_axis = "[ 0 0 -1 0 ]";
    }
    else {
        s_axis = "[ 1 0 0 0 ]";
        t_axis = "[ 0 -1 0 0 ]";
    }

    f << "( " << p1.x << " " << p1.y << " " << p1.z << " ) "
      << "( " << p2.x << " " << p2.y << " " << p2.z << " ) "
      << "( " << p3.x << " " << p3.y << " " << p3.z << " ) "
      << tex << " " << s_axis << " " << t_axis << " 0 1.0 1.0 \n";
}

//void makeBrick(std::ofstream& f, Vector min, Vector max, std::string tex) {
    //f << "{\n";
    //writeFace(f, {min.x, max.y, max.z}, {max.x, max.y, max.z}, {max.x, min.y, max.z}, tex); // Верх
   // writeFace(f, {min.x, min.y, min.z}, {max.x, min.y, min.z}, {max.x, max.y, min.z}, tex); // Низ
    //writeFace(f, {min.x, max.y, max.z}, {min.x, min.y, max.z}, {min.x, min.y, min.z}, tex); // Лево
    //writeFace(f, {max.x, min.y, max.z}, {max.x, max.y, max.z}, {max.x, max.y, min.z}, tex); // Право
    //writeFace(f, {min.x, min.y, max.z}, {max.x, min.y, max.z}, {max.x, min.y, min.z}, tex); // Перед
    //writeFace(f, {max.x, max.y, max.z}, {min.x, max.y, max.z}, {min.x, max.y, min.z}, tex); // Зад
    //f << "}\n";
//}

void makeBrick(std::ofstream& f, Vector min, Vector max, std::string tex) {
    f << "{\n";
    writeFace(f, {min.x, max.y, max.z}, {max.x, max.y, max.z}, {max.x, min.y, max.z}, tex);
    writeFace(f, {min.x, min.y, min.z}, {max.x, min.y, min.z}, {max.x, max.y, min.z}, tex);
    writeFace(f, {min.x, max.y, max.z}, {min.x, min.y, max.z}, {min.x, min.y, min.z}, tex);
    writeFace(f, {max.x, min.y, max.z}, {max.x, max.y, max.z}, {max.x, max.y, min.z}, tex);
    writeFace(f, {min.x, min.y, max.z}, {max.x, min.y, max.z}, {max.x, min.y, min.z}, tex);
    writeFace(f, {max.x, max.y, max.z}, {min.x, max.y, max.z}, {min.x, max.y, min.z}, tex);
    f << "}\n";
}

void makeSkybox(std::ofstream& f, float size) {
    float t = 64.0f;
    makeBrick(f, {-size, -size, -t}, {size, size, 0}, "SKY");
    makeBrick(f, {-size, -size, size}, {size, size, size + t}, "SKY");
    makeBrick(f, {-size - t, -size, 0}, {-size, size, size}, "SKY");
    makeBrick(f, {size, -size, 0}, {size + t, size, size}, "SKY");
    makeBrick(f, {-size, -size - t, 0}, {size, -size, size}, "SKY");
    makeBrick(f, {-size, size, 0}, {size, size + t, size}, "SKY");
}

void makePointEntity(std::ofstream& f, std::string classname, Vector origin, std::vector<std::pair<std::string, std::string>> kv) {
    f << "{\n\"classname\" \"" << classname << "\"\n\"origin\" \"" 
      << origin.x << " " << origin.y << " " << origin.z << "\"\n";
    for (auto& p : kv) f << "\"" << p.first << "\" \"" << p.second << "\"\n";
    f << "}\n";
}

void makeBrushEntity(std::ofstream& f, std::string classname, Vector min, Vector max, std::string tex, std::vector<std::pair<std::string, std::string>> kv) {
    f << "{\n\"classname\" \"" << classname << "\"\n";
    for (auto& p : kv) f << "\"" << p.first << "\" \"" << p.second << "\"\n";
    makeBrick(f, min, max, tex);
    f << "}\n";
}

void makeAmbientSound(std::ofstream& f, Vector origin, std::string wavPath, float volume) {
    int healthValue = (int)(volume * 10.0f);
    if (healthValue > 10) healthValue = 10;
    if (healthValue < 1) healthValue = 1;

    makePointEntity(f, "ambient_generic", origin, {
        {"message", wavPath},
        {"health", std::to_string(healthValue)},
        {"spawnflags", "0"}
    });
}

void makePlayerEquipment(std::ofstream& f, std::vector<std::string> weapons) {
    f << "{\n\"classname\" \"game_player_equip\"\n";
    for (const auto& w : weapons) {
        f << "\"" << w << "\" \"1\"\n"; 
    }
    f << "}\n";
}

