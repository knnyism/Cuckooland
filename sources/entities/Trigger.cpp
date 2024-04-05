#include <entities/Trigger.h>

void Trigger::Load(BodyCreationSettings* bcs, std::function<void(BodyID)> onTouch) {
    bcs->mIsSensor = true;
    bodyId = body_interface->CreateAndAddBody(*bcs, EActivation::DontActivate);
    onTouchFunc = onTouch;
}

void Trigger::Tick() {
    if (!IsActive)
        return;

    for (const BodyID& otherBodyId : bodiesInSensor) {
        onTouchFunc(otherBodyId);
    }
}

void Trigger::Destroy() {
    body_interface->RemoveBody(bodyId);
}

void Trigger::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) {
    // Check which body is the sensor
    BodyID otherBodyId;
    if (inBody1.GetID() == bodyId)
        otherBodyId = inBody2.GetID();
    else if (inBody2.GetID() == bodyId)
        otherBodyId = inBody1.GetID();
    else
        return;

    lock_guard lock(triggerMutex);
    bodiesInSensor.insert(std::lower_bound(bodiesInSensor.begin(), bodiesInSensor.end(), otherBodyId), otherBodyId);
}

void Trigger::OnContactRemoved(const SubShapeIDPair& inSubShapePair) {
    BodyID otherBodyId;
    if (inSubShapePair.GetBody1ID() == bodyId)
        otherBodyId = inSubShapePair.GetBody2ID();
    else if (inSubShapePair.GetBody2ID() == bodyId)
        otherBodyId = inSubShapePair.GetBody1ID();
    else
        return;

    lock_guard lock(triggerMutex);
    bodiesInSensor.erase(std::lower_bound(bodiesInSensor.begin(), bodiesInSensor.end(), otherBodyId));
}