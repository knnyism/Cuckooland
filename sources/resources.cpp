#include <resources.h>

Assimp::Importer importer;
const string assetsPath = string(GetWorkingDirectory()) + "/assets/";

std::map<string, raylib::Model*> models;
std::map<string, Array<Vec3>*> shapes;
std::map<string, raylib::Sound*> sounds;

raylib::Model* GetModel(string path) {
    if (models[path] == nullptr) {
        raylib::Model* model = new raylib::Model(assetsPath + "models/" + path + ".glb");

        for (int i = 0; i < model->materialCount; i++) {
            model->materials[i] = LoadPbrMaterial(
                model->materials[i].maps[MATERIAL_MAP_ALBEDO].texture,
                model->materials[i].maps[MATERIAL_MAP_NORMAL].texture,
                model->materials[i].maps[MATERIAL_MAP_ROUGHNESS].texture
            );
        }

        models[path] = model;
    }

    return models[path];
}

Array<Vec3>* GetShape(string path) {
    if (shapes[path] == nullptr) {
        string extension = ".p.glb";
        if (!std::filesystem::exists(assetsPath + "models/" + path + extension))
            extension = ".glb";

        const aiScene* scene = importer.ReadFile(assetsPath + "models/" + path + extension, aiProcess_Triangulate | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
        }

        Array<Vec3>* shape = new Array<Vec3>();

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];

            for (int j = 0; j < mesh->mNumVertices; j++) {
                aiVector3D vertex = mesh->mVertices[j];
                Vec3 jphVertex = Vec3(vertex.x, vertex.y, vertex.z);

                bool found = false;
                for (const Vec3& otherVertex : *shape) {
                    if (jphVertex.IsClose(otherVertex)) {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    shape->push_back(jphVertex);
            }
        }

        for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
            shape->push_back(Vec3(scene->mMeshes[0]->mVertices[i].x, scene->mMeshes[0]->mVertices[i].y, scene->mMeshes[0]->mVertices[i].z));
        }

        shapes[path] = shape;
    }

    return shapes[path];
}

raylib::Sound* GetSound(string path) {
    if (sounds[path] == nullptr) {
        sounds[path] = new raylib::Sound(assetsPath + "sounds/" + path + ".wav");
    }

    return sounds[path];
}

string GetAssetPath(string localDirectory) {
    return assetsPath + localDirectory;
}