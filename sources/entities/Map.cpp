#include <entities/Map.h>

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

    JPH::BodyInterface& body_interface = physics_system->GetBodyInterface();
    JPH::TriangleList triangles;

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        for (int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];

            triangles.push_back(
                JPH::Triangle(
                    JPH::Float3(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z),
                    JPH::Float3(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z),
                    JPH::Float3(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z)
                )
            );
        }
    }

    JPH::BodyCreationSettings creationSettings(JPH::BodyCreationSettings(new JPH::MeshShapeSettings(triangles), JPH::RVec3::sZero(), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING));
    bodyId = body_interface.CreateAndAddBody(creationSettings, JPH::EActivation::Activate);
}

void Map::Tick() {

}

void Map::Render() {
    model.Draw({ 0.0f, 0.0f, 0.0f });
}