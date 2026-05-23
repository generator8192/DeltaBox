#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <fstream>
#include <string>
#include <vector>

struct Vector { float x, y, z; };

void writeFace(std::ofstream& f, Vector p1, Vector p2, Vector p3, std::string tex);
void makeBrick(std::ofstream& f, Vector min, Vector max, std::string tex);
void makeSkybox(std::ofstream& f, float size);
void makePointEntity(std::ofstream& f, std::string classname, Vector origin, std::vector<std::pair<std::string, std::string>> kv = {});
void makeBrushEntity(std::ofstream& f, std::string classname, Vector min, Vector max, std::string tex, std::vector<std::pair<std::string, std::string>> kv = {});
void makeAmbientSound(std::ofstream& f, Vector origin, std::string wavPath, float volume = 1.0);
void makePlayerEquipment(std::ofstream& f, std::vector<std::string> weapons);

#endif
