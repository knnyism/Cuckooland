#include <entities/Map.h>

void Map::Load(const char* map_path) {
    model = raylib::Model(map_path);

    /*
    for (int i = 0; i < model.materialCount; i++) {
        model.materials[i].shader = *shader;

        model.materials[i].maps[MATERIAL_MAP_ALBEDO].color = raylib::Color(255, 255, 255, 255);
        model.materials[i].maps[MATERIAL_MAP_METALNESS].value = 0.0f;
        model.materials[i].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
        model.materials[i].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
        model.materials[i].maps[MATERIAL_MAP_EMISSION].color = raylib::Color(255, 162, 0, 255);
    }
    */

    // Now we need to load the map into the physics system
    // TODO: Assimp is great, although maybe we could use raylib here? It would let us avoid parsing the model twice

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(map_path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
        return;
    }

    JPH::BodyInterface& body_interface = physics_system->GetBodyInterface();

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

    BodyCreationSettings creationSettings(BodyCreationSettings(new MeshShapeSettings(triangles), RVec3::sZero(), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING));
    body_interface.CreateAndAddBody(creationSettings, JPH::EActivation::Activate);
}

void Map::Tick() {

}

void Map::Render() {
    model.Draw({ 0.0f, 0.0f, 0.0f });
}