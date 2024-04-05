#include <entities/SlidingDoor.h>

void SlidingDoor::Spawn(Vec3 closedPosition, Vec3 openPosition, Quat rotation) {
    this->closedPosition = closedPosition;
    this->openPosition = openPosition;

    position = closedPosition;
    this->rotation = rotation;

    openSound.position = closedPosition;
    closeSound.position = closedPosition;

    model.Load("tech_door", position, rotation, -1.0f, {}, EMotionType::Kinematic);

    trigger = CreateEntity<Trigger>();
    trigger->Load(new BodyCreationSettings(new BoxShape(Vec3(sensorRange, 6.0f, 5.0f)), position, rotation, EMotionType::Static, Layers::TRIGGER),
        [this](BodyID bodyId) { OnTriggerTouched(bodyId); });
}

void SlidingDoor::SetState(bool value) {
    if (value) { openSound.Play(); }
    else { closeSound.Play(); }

    isOpen = value;
    isMoving = true;
    moveStartTime = GetTime();
}

void SlidingDoor::OnTriggerTouched(BodyID bodyId) {
    if (bodyId == model.bodyId)
        return;

    sensorHit = true;
}

void SlidingDoor::HandleState() {
    if (!automatic) { return; }

    if (sensorHit) {
        if (!isMoving && !isOpen) { SetState(true); }
        lastCheckTime = GetTime();
    }
    else {
        if (!isMoving && isOpen && GetTime() - lastCheckTime > idleTime) { SetState(false); }
    }

}

void SlidingDoor::Animate() {
    f32 alpha = std::clamp((GetTime() - moveStartTime) / moveTime, 0.0, 1.0);

    Vec3 nextState = isOpen ? openPosition : closedPosition;
    Vec3 lastState = isOpen ? closedPosition : openPosition;
    f32 tweened = isOpen ? tween::sineinout(alpha) : tween::sinein(alpha);

    position = lastState + (nextState - lastState) * tweened;

    if (alpha == 1.0) {
        isMoving = false;
    }
}

void SlidingDoor::Tick() {
    HandleState();
    Animate();

    body_interface->MoveKinematic(model.bodyId, position, rotation, TICK_DURATION);

    model.Tick();

    sensorHit = false;
}

void SlidingDoor::Render() {
    model.Render();
}