#include <entities/Map.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Geometry/Triangle.h>

#include <iostream>

void Map::Load(const char* map_path) {
    model = raylib::Model(map_path);

    // Now we need to load the map into the physics system
    // TODO: Assimp is great, although maybe we could use raylib here? It would let us avoid parsing the model twice

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(map_path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
        return;
    }

    BodyInterface& body_interface = physics_system->GetBodyInterface();
    TriangleList triangles;

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        for (int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];

            triangles.push_back(
                Triangle(
                    Float3(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z),
                    Float3(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z),
                    Float3(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z)
                )
            );
        }
    }

    BodyCreationSettings creationSettings(BodyCreationSettings(new MeshShapeSettings(triangles), RVec3::sZero(), Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI), EMotionType::Static, Layers::NON_MOVING));
    Body* meshBody = body_interface.CreateBody(creationSettings);
    body_interface.AddBody(meshBody->GetID(), EActivation::DontActivate);

    bodyId = meshBody->GetID();
}

void Map::Tick() {

}

void Map::Render() {
    model.Draw({ 0.0f, 0.0f, 0.0f });
}