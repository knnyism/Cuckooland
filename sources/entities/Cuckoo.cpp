#include <entities/Cuckoo.h>

void Cuckoo::Load(Vec3 position, Quat rotation, bool playLongAnimation) {
    this->playLongAnimation = playLongAnimation;

    prop = CreateEntity<Prop>();
    prop->Load("cuckoo", position, rotation, -1.0f, 0.5f, Layers::NO_GRAB);

    if (playLongAnimation)
        animationModel = GetModel("cuckoo");
}

void Cuckoo::HandlePlayer(Player* player) {
    if ((player->moveHelper.position - body_interface->GetPosition(prop->model.bodyId)).Length() < 6.0f) {
        acquired = true;
        acquireTime = GetTime();

        cuckooGetSound.Play();

        cameraRelativeMatrix = game::GetCameraMatrix().Inversed() * body_interface->GetWorldTransform(prop->model.bodyId);

        DestroyEntity(prop);
    }
}

void Cuckoo::Tick() {
    if (prop->IsAlive) {
        for (auto& bucket : entities.buckets) {
            for (Entity* entity : bucket->data) {
                Player* player = dynamic_cast<Player*>(entity);

                if (player) {
                    HandlePlayer(player);
                }
            }
        }
    }
}

void Cuckoo::Render() {
    if (acquired) {

        Mat44 worldMatrix = game::GetCameraMatrix() * Mat44::sRotationTranslation(cameraRelativeMatrix.GetRotation().GetQuaternion(), cameraRelativeMatrix.GetTranslation() * 0.1f);
        Mat44 targetMatrix = game::GetCameraMatrix() * Mat44::sTranslation(Vec3(0, -0.05, 0.6));

        f32 t = GetTime() - acquireTime;
        f32 alpha = tween::sineinout(std::clamp(t / 1.0f, 0.0f, 1.0f));

        Quat resultRotation = worldMatrix.GetRotation().GetQuaternion().SLERP(targetMatrix.GetRotation().GetQuaternion(), alpha);
        Vec3 resultPosition = worldMatrix.GetTranslation() + (targetMatrix.GetTranslation() - worldMatrix.GetTranslation()) * alpha;

        Mat44 resultMatrix = Mat44::sRotationTranslation(resultRotation.Normalized(), resultPosition);

        Vec3 position = resultMatrix.GetTranslation();
        Vec3 rotationAxis;
        f32 rotationAngle;
        resultMatrix.GetRotation().GetQuaternion().GetAxisAngle(rotationAxis, rotationAngle);

        animationModel->Draw(
            { position.GetX(), position.GetY(), position.GetZ() },
            { rotationAxis.GetX(), rotationAxis.GetY(), rotationAxis.GetZ() },
            rotationAngle * RAD2DEG, Vector3{ 0.1f, 0.1f, 0.1f }
        );
    }
}