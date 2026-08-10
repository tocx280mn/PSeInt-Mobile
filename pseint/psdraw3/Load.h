#ifndef LOAD_H
#define LOAD_H
#include <cstddef>
#include <string>
class Entity;
bool Save(std::string filename = "");
bool Load(std::string filename = "");
void SetProc(Entity *proc);
void CreateEmptyProc(std::string type);
void New();
#endif

