#include <entities/Door.h>

constexpr f32 HINGE_SIZE = 0.5f;

void Door::Load(const std::string path, Vec3 pos, Quat rot, f32 mass, f32 hingeLimitsMin, f32 hingeLimitsMax) {
    model.Load(path, pos, rot, mass);

    Mat44 doorMatrix = Mat44::sRotationTranslation(rot, pos);
    Mat44 hingeMatrix = doorMatrix * Mat44::sTranslation(Vec3(0, 0, -2.6f));

    body1 = body_interface->CreateBody(BodyCreationSettings(new BoxShape(Vec3::sReplicate(HINGE_SIZE)), hingeMatrix.GetTranslation(), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING));
    body_interface->AddBody(body1->GetID(), EActivation::DontActivate);

    HingeConstraintSettings hinge;
    hinge.mPoint1 = hinge.mPoint2 = (hingeMatrix * Mat44::sTranslation(Vec3(0, 0, HINGE_SIZE))).GetTranslation();
    hinge.mHingeAxis1 = hinge.mHingeAxis2 = Vec3::sAxisY();
    hinge.mNormalAxis1 = hinge.mNormalAxis2 = Vec3::sAxisZ();
    hinge.mLimitsMin = DegreesToRadians(hingeLimitsMin);
    hinge.mLimitsMax = DegreesToRadians(hingeLimitsMax);

    physics_system->AddConstraint(hinge.Create(*body1, *model.body));
}

void Door::Tick() {
    model.Tick();
}

void Door::Render() {
    model.Render();
}