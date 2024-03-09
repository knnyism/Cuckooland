/*
* resources.h
*
* The game got quite large so I decided to make a quick resource manager
* This file is used to load resources from the disk and store them in memory
* It also provides a way to access these resources
*/

#pragma once

#include <physics.h>
#include <lighting.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <filesystem>
#include <string>

#include <map>

raylib::Model* GetModel(string path);
Array<Vec3>* GetShape(string path);
raylib::Sound* GetSound(string path);

string GetAssetPath(string localDirectory);