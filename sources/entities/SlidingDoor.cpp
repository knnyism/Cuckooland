#include <entities/SlidingDoor.h>

void SlidingDoor::Spawn(Vec3 closedPosition, Vec3 openPosition, Quat rotation) {
    this->closedPosition = closedPosition;
    this->openPosition = openPosition;

    position = closedPosition;

    model.Load("tech_door", position, rotation, {}, EMotionType::Static);
}

// TODO: Do better.
void SlidingDoor::Tick() {
    bool shouldMove = false;

    // Our sensor is active, so we should check if the player is near
    if (sensorActive) {
        for (auto& bucket : entities.buckets)
            for (Entity* entity : bucket->data) {
                Player* plr = dynamic_cast<Player*>(entity);

                if (plr) {
                    f32 distance = (plr->moveHelper.position - closedPosition).Length();

                    if (distance < 6.0f) // Player is near and door is closed
                    {
                        lastCheckTime = GetTime();

                        if (!isOpen)
                            shouldMove = true;
                    }
                    else if (distance > 6.0f && isOpen && (GetTime() - lastCheckTime) > idleTime) // Otherwise...
                    {
                        shouldMove = true;
                    }
                }
            }
    }

    if (shouldMove) {
        isMoving = true;
        sensorActive = false;
    }

    if (isMoving) {
        if (isOpen) {
            position += (closedPosition - openPosition).Normalized() * 0.05f;

            if ((position - closedPosition).Length() < 0.1f) {
                isMoving = false;
                isOpen = false;
                sensorActive = true;
            }
        }
        else if (!isOpen) {
            position += (openPosition - closedPosition).Normalized() * 0.05f;

            if ((position - openPosition).Length() < 0.1f) {
                isMoving = false;
                isOpen = true;
                sensorActive = true;
            }
        }
    }

    body_interface->SetPosition(model.bodyId, position, EActivation::Activate);

    model.Tick();
}

void SlidingDoor::Render() {
    model.Render();
}