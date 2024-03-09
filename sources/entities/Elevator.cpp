#include <entities/Elevator.h>

void Elevator::Load(Vec3 pos, Quat rot, f32 distance) {
    this->rot = rot;

    shaftPos = pos - Vec3(0, 5, 0);
    basePos = pos - Vec3(distance, 0, 0);

    base.Load("elevator_base", basePos, rot, 62000.0f, 1.0f, EMotionType::Dynamic, Layers::NO_GRAB);
    shaft.Load("elevator_shaft", shaftPos, rot, 5000.0f, 1.0f, EMotionType::Dynamic, Layers::NO_GRAB);

    PulleyConstraintSettings settings;
    settings.mBodyPoint1 = basePos; // Connect at the top of the block
    settings.mBodyPoint2 = shaftPos;
    settings.mFixedPoint1 = settings.mBodyPoint1 + Vec3(0, 9, 0);
    settings.mFixedPoint2 = settings.mBodyPoint2 + Vec3(0, 0, 0);
    settings.mRatio = 0.5f;
    settings.mMinLength = settings.mMaxLength = -1;

    MassProperties baseMass = shaft.body->GetShape()->GetMassProperties();
    baseMass.mMass = 62000.0f;

    MassProperties shaftMass = shaft.body->GetShape()->GetMassProperties();
    shaftMass.mMass = 5000.0f;

    base.body->GetMotionProperties()->SetMassProperties(EAllowedDOFs::TranslationY, baseMass);
    shaft.body->GetMotionProperties()->SetMassProperties(EAllowedDOFs::TranslationY, shaftMass);

    physics_system->AddConstraint(body_interface->CreateConstraint(&settings, base.bodyId, shaft.bodyId));
}

void Elevator::Destroy() {
    body_interface->RemoveBody(base.bodyId);
    body_interface->RemoveBody(shaft.bodyId);
}

void Elevator::Tick() {
    base.Tick();
    shaft.Tick();
}

void Elevator::Render() {
    base.Render();
    shaft.Render();
}