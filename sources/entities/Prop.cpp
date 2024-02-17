#include <entities/Prop.h>

void Prop::Load(const char* path) {
    model = raylib::Model(path);
    // Now we need to load the map into the physics system
    // TODO: Assimp is great, although maybe we could use raylib here? It would let us avoid parsing the model twice

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
        return;
    }

    JPH::BodyInterface& body_interface = physics_system->GetBodyInterface();

    JPH::Array<JPH::Vec3> propShape;

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        for (int j = 0; j < mesh->mNumVertices; j++) {
            aiVector3D vertex = mesh->mVertices[j];
            propShape.push_back(JPH::Vec3(vertex.x, vertex.y, vertex.z));
        }
    }

    bodyId = body_interface.CreateAndAddBody(JPH::BodyCreationSettings(new JPH::ConvexHullShapeSettings(propShape), JPH::RVec3(0, 0, 5), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING), JPH::EActivation::Activate);
}

void Prop::Tick() {
    JPH::BodyInterface& body_interface = physics_system->GetBodyInterface();

    JPH::RVec3 position = body_interface.GetPosition(bodyId);
    JPH::Quat rotation = body_interface.GetRotation(bodyId);
    auto normalized = rotation.Normalized(); // Normalize the quaternion

    float angle = 2.0f * acos(normalized.GetW()); // Calculate the angle of rotation
    JPH::Vec3 axis = JPH::Vec3(normalized.GetX(), normalized.GetY(), normalized.GetZ()).Normalized(); // Calculate the axis of rotation

    raylib_position = { position.GetX(), position.GetY(), position.GetZ() };
    raylib_rotationAxis = { axis.GetX(), axis.GetY(), axis.GetZ() };
    raylib_rotationAngle = angle * RAD2DEG; // Raylib uses degrees... GROSS- just kidding :3
}

void Prop::Render() {
    model.Draw(raylib_position, raylib_rotationAxis, raylib_rotationAngle);
}