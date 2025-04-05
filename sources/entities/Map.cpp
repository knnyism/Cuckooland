#include <entities/Map.h>

void Map::Load( std::string map_path) {
    // TODO: Clean this up
    model = GetModel(map_path);

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(GetAssetPath("/models/" + map_path + ".glb"), aiProcess_Triangulate);

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
    model->Draw({ 0.0f, 0.0f, 0.0f });
}