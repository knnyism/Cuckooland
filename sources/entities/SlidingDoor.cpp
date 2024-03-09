#include <entities/SlidingDoor.h>

void SlidingDoor::Spawn(Vec3 closedPosition, Vec3 openPosition, Quat rotation) {
    this->closedPosition = closedPosition;
    this->openPosition = openPosition;

    position = closedPosition;
    this->rotation = rotation;

    openSound.position = closedPosition;
    closeSound.position = closedPosition;

    model.Load("tech_door", position, rotation, {}, {}, EMotionType::Static);
}

void SlidingDoor::HandlePlayer(Player* player) {
    if ((player->moveHelper.position - closedPosition).Length() < sensorRange) {
        sensorHit = true;
    }
}

void SlidingDoor::HandleState() {
    if (sensorHit) {
        if (!isMoving && !isOpen) {
            openSound.Play();
            isOpen = true;
            isMoving = true;
            moveStartTime = GetTime();
        }
        lastCheckTime = GetTime();
    }
    else {
        if (!isMoving && isOpen && GetTime() - lastCheckTime > idleTime) {
            closeSound.Play();
            isOpen = false;
            isMoving = true;
            moveStartTime = GetTime();
        }
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
    sensorHit = false;

    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            Player* player = dynamic_cast<Player*>(entity);

            if (player) {
                HandlePlayer(player);
            }
        }
    }

    HandleState();
    Animate();

    body_interface->SetPosition(model.bodyId, position, EActivation::Activate);
    model.Tick();
}

void SlidingDoor::Render() {
    model.Render();
}