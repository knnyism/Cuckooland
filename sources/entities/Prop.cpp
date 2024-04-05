#include <entities/Prop.h>

void Prop::Load(const string path, Vec3 position, Quat rotation, f32 overrideMass, f32 overrideFriction, ObjectLayer layer, PropMaterial material) {
    model.Load(path, position, rotation, overrideMass, overrideFriction, EMotionType::Dynamic, layer);

    hitSound = new game::Sound3D((material == MATERIAL_WOOD) ? "hit_wood" : "hit_metal", Vec3::sZero(), 6, 3);
}

void Prop::Destroy() {
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
}

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