#include <entities/Prop.h>

void Prop::Load(const std::string path, Vec3 position, Quat rotation, f32 overrideMass, f32 overrideFriction, ObjectLayer layer, PropMaterial material, EMotionType motionType) {
    model.Load(path, position, rotation, overrideMass, overrideFriction, motionType, layer);

    modelPath = path;
    hitSound = new game::Sound3D((material == MATERIAL_WOOD) ? "hit_wood" : "hit_metal", Vec3::sZero(), 6, 3);

    spawnMatrix = Mat44::sRotationTranslation(rotation, position);
    savedMatrix = Mat44::sRotationTranslation(rotation, position);
}

void Prop::SaveState() {
    savedMatrix = Mat44::sRotationTranslation(model.body->GetRotation(), model.body->GetPosition());
}

void Prop::LoadSavedState() {
    body_interface->SetLinearAndAngularVelocity(model.bodyId, Vec3::sZero(), Vec3::sZero());
    body_interface->SetPositionAndRotation(model.bodyId, savedMatrix.GetTranslation(), savedMatrix.GetRotation().GetQuaternion(), EActivation::Activate);
}

void Prop::ResetState() {
    body_interface->SetLinearAndAngularVelocity(model.bodyId, Vec3::sZero(), Vec3::sZero());
    body_interface->SetPositionAndRotation(model.bodyId, spawnMatrix.GetTranslation(), spawnMatrix.GetRotation().GetQuaternion(), EActivation::Activate);
}

void Prop::Destroy() {
    if (trigger != nullptr) { DestroyEntity(trigger); }

    body_interface->RemoveBody(model.bodyId);
    body_interface->DestroyBody(model.bodyId);
}

void Prop::Tick() {
    if (emitSound && lastHitBody != hitBody) {
        hitSound->volume = hitVolume;
        hitSound->Play();

        emitSound = false;
        lastHitBody = hitBody;
    }

    if (lastHitTime > 1.0f) {
        hitBody = BodyID();
    }

    model.Tick();

    if (trigger != nullptr) {
        body_interface->SetPosition(trigger->bodyId, model.body->GetPosition(), EActivation::DontActivate);
        body_interface->SetRotation(trigger->bodyId, model.body->GetRotation(), EActivation::DontActivate);
    }
}

void Prop::SetTriggerFunc(std::function<void(BodyID)> func) {
    if (trigger == nullptr) {
        BodyCreationSettings* bcs = new BodyCreationSettings(new ConvexHullShapeSettings(*GetShape(modelPath)), model.body->GetPosition(), model.body->GetRotation(), EMotionType::Dynamic, Layers::TRIGGER);

        trigger = CreateEntity<Trigger>();
        trigger->Load(bcs, func);
    }
};

void Prop::Render() {
    hitSound->position = body_interface->GetPosition(model.bodyId);

    model.Render();
}

void Prop::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) {
    f32 currentTime = GetTime();

    if (currentTime - lastHitTime > 0.2f) {
        if (inBody1.GetID() == model.bodyId || inBody2.GetID() == model.bodyId) {
            emitSound = true;
            hitVolume = min(inManifold.mPenetrationDepth * 10.0f, 0.5f);
            hitBody = inBody1.GetID() == model.bodyId ? inBody2.GetID() : inBody1.GetID();
            lastHitTime = currentTime;
        }
    }
}