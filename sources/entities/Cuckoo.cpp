#include <entities/Cuckoo.h>

void Cuckoo::Load(Vec3 position, Quat rotation, bool playLongAnimation) {
    this->playLongAnimation = playLongAnimation;

    prop = CreateEntity<Prop>();
    prop->Load("cuckoo", position, rotation, -1.0f, 0.5f, Layers::NO_GRAB);

    trigger = CreateEntity<Trigger>();
    trigger->Load(new BodyCreationSettings(new BoxShape(Vec3::sReplicate(2.0f)), position, rotation, EMotionType::Dynamic, Layers::TRIGGER),
        [this](BodyID bodyId) {
            if (triggerFunc != nullptr)
                triggerFunc(bodyId);

            OnTriggerTouched(bodyId);
        });

    animationModel = GetModel("cuckoo");

    totalCuckoos++;
}

void Cuckoo::SetTriggerFunc(std::function<void(BodyID)> func) {
    triggerFunc = func;
}

void Cuckoo::OnTriggerTouched(BodyID bodyId) {
    if (acquired)
        return;

    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            if (!entity->IsAlive)
                continue;

            Player* player = dynamic_cast<Player*>(entity);

            if (player) {
                if (bodyId == player->body->GetID()) {
                    acquired = true;
                    acquireTime = GetTime();

                    cuckooCount++;

                    if (playLongAnimation)
                        cuckooGetSound.Play();

                    cameraRelativeMatrix = game::GetCameraMatrix().Inversed() * body_interface->GetWorldTransform(prop->model.bodyId);
                    lastTransform = body_interface->GetWorldTransform(prop->model.bodyId);

                    DestroyEntity(prop);
                    DestroyEntity(trigger);
                }
            }
        }
    }
}

void Cuckoo::Tick() {
    if (!acquired) {
        body_interface->SetPosition(trigger->bodyId, body_interface->GetPosition(prop->model.bodyId), EActivation::DontActivate);
        body_interface->SetRotation(trigger->bodyId, body_interface->GetRotation(prop->model.bodyId), EActivation::DontActivate);
    }
}

void Cuckoo::Render() {
    if (acquired && playLongAnimation) {
        Mat44 worldMatrix = game::GetCameraMatrix() * Mat44::sRotationTranslation(cameraRelativeMatrix.GetRotation().GetQuaternion(), cameraRelativeMatrix.GetTranslation() * 0.1f);
        Mat44 targetMatrix = game::GetCameraMatrix() * Mat44::sTranslation(Vec3(0.0f, 0.0f, 0.6f));

        f32 t = GetTime() - acquireTime;
        f32 alpha = tween::sineinout(std::clamp(t / 1.0f, 0.0f, 1.0f));

        Quat resultRotation = worldMatrix.GetRotation().GetQuaternion().SLERP(targetMatrix.GetRotation().GetQuaternion(), alpha);
        Vec3 resultPosition = worldMatrix.GetTranslation() + (targetMatrix.GetTranslation() - worldMatrix.GetTranslation()) * alpha;

        Mat44 resultMatrix = Mat44::sRotationTranslation(resultRotation.Normalized(), resultPosition);

        Vec3 position = resultMatrix.GetTranslation();
        Vec3 rotationAxis;
        f32 rotationAngle;
        resultMatrix.GetRotation().GetQuaternion().GetAxisAngle(rotationAxis, rotationAngle);

        f32 alpha2 = 1.0f - tween::sineinout(std::clamp((t - 7.0f) / 1.0f, 0.0f, 1.0f));
        f32 alpha3 = 1.0f - tween::sineinout(std::clamp((t - 7.2f) / 1.2f, 0.0f, 1.0f));

        if (t > 7.0f && !added) {
            added = true;
            cuckooAddSound.Play();
        }

        animationModel->Draw(
            { position.GetX(), position.GetY(), position.GetZ() },
            { rotationAxis.GetX(), rotationAxis.GetY(), rotationAxis.GetZ() },
            rotationAngle * RAD2DEG, Vector3{ 0.1f * alpha3, 0.1f * alpha2, 0.1f * alpha3 }
        );

        if (t > 8.5f) { DestroyEntity(this); }
    }
    else if (acquired) {
        f32 t = GetTime() - acquireTime;

        f32 alpha2 = 1.0f - tween::sineinout(std::clamp(t / 1.0f, 0.0f, 1.0f));
        f32 alpha3 = 1.0f - tween::sineinout(std::clamp(t / 1.2f, 0.0f, 1.0f));

        Mat44 targetMatrix = game::GetCameraMatrix() * Mat44::sTranslation(Vec3(0, -0.1, 0));

        Quat resultRotation = lastTransform.GetRotation().GetQuaternion().SLERP(targetMatrix.GetRotation().GetQuaternion(), 1.0f - alpha2);
        Vec3 resultPosition = lastTransform.GetTranslation() + (targetMatrix.GetTranslation() - lastTransform.GetTranslation()) * (1.0f - alpha2);

        Vec3 rotationAxis;
        f32 rotationAngle;
        resultRotation.GetAxisAngle(rotationAxis, rotationAngle);

        if (!added) {
            added = true;
            cuckooAddSound.Play();
        }

        animationModel->Draw(
            { resultPosition.GetX(), resultPosition.GetY(), resultPosition.GetZ() },
            { rotationAxis.GetX(), rotationAxis.GetY(), rotationAxis.GetZ() },
            rotationAngle * RAD2DEG, Vector3{ alpha3, alpha2, alpha3 }
        );

        if (t > 1.2f) { DestroyEntity(this); }
    }
}

void Cuckoo::AfterCamera() {
    if (acquired && playLongAnimation) {
        f32 t = GetTime() - acquireTime;

        if (t > 1.0f && t < 7.0f) {
            game::UIObject<raylib::Text> topText(game::UDim2(0.5, 0, 0.4, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f,0.5f }, "You got a cuckoo!");
            topText.object.SetFontSize(topText.GetAbsoluteSize().y);
            topText.object.SetSpacing(2.0f);
            topText.size = game::UDim2(0, topText.object.MeasureEx().x, topText.size.y.scale, 0);

            game::UIObject<raylib::Text> bottomText(game::UDim2(0.5, 0, 0.6, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f,0.5f }, "Find and collect as many cuckoos as you can!");
            bottomText.object.SetFontSize(bottomText.GetAbsoluteSize().y);
            bottomText.object.SetSpacing(2.0f);
            bottomText.size = game::UDim2(0, bottomText.object.MeasureEx().x, bottomText.size.y.scale, 0);

            topText.object.Draw(topText.GetAbsolutePosition());
            bottomText.object.Draw(bottomText.GetAbsolutePosition());
        }
    }
}
