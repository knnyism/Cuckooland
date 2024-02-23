#include <game.h>

Entity* FindEntity(EntityId id)
{
    Entity* ent = entities.buckets[id.bucket]->data[id.index];

    if (ent->IsAlive && ent->id == id) {
        return ent;
    }

    return nullptr;
}

void DestroyEntity(Entity* ent)
{
    std::cout << "ENT: DESTROY: at bucket " << ent->id.bucket << " with id " << ent->id.index << std::endl;
    ent->IsAlive = false;
    ent->Destroy();
}

namespace game {
    void Model::Load(string path, Vec3 position, Quat rotation, f32 overrideMass, EMotionType motionType) {
        string assetsPath = "C:/Users/canis/Repositories/Cuckooland/assets/";
        Assimp::Importer importer;

        string modelPath = assetsPath + "models/" + path + ".glb";
        string physicsPath = assetsPath + "models/" + path + ".p.glb";

        raylibModel = raylib::Model(modelPath);

        if (!std::filesystem::exists(physicsPath))
            physicsPath = modelPath;

        const aiScene* scene = importer.ReadFile(physicsPath, aiProcess_Triangulate);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
            return;
        }

        Array<Vec3> propShape;

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];

            for (int j = 0; j < mesh->mNumVertices; j++) {
                aiVector3D vertex = mesh->mVertices[j];
                propShape.push_back(Vec3(vertex.x, vertex.y, vertex.z));
            }
        }

        BodyCreationSettings bcs = BodyCreationSettings(new ConvexHullShapeSettings(propShape), position, rotation, motionType, Layers::MOVING);

        if (overrideMass != -1.0f) {
            std::cout << "Overriding mass: " << overrideMass << std::endl;
            bcs.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
            bcs.mMassPropertiesOverride.mMass = overrideMass;
        }

        bodyId = body_interface->CreateAndAddBody(bcs, EActivation::Activate);
    }

    void Model::Tick() {
        Vec3 position = body_interface->GetPosition(bodyId);
        Quat rotation = body_interface->GetRotation(bodyId);

        Vec3 axis;
        f32 angle;
        rotation.GetAxisAngle(axis, angle);

        interpState.Update(GetTime(), position, axis, angle);
    }

    void Model::Render() {
        Vec3 position = interpState.GetPosition();
        Vec3 rotationAxis = interpState.GetRotationAxis();
        f32 rotationAngle = interpState.GetRotationAngle();

        raylibModel.Draw(
            { position.GetX(), position.GetY(), position.GetZ() },
            { rotationAxis.GetX(), rotationAxis.GetY(), rotationAxis.GetZ() },
            rotationAngle * RAD2DEG
        );
    }
}