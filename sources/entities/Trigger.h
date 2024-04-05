#pragma once 

#include <game.h>
#include <entities/Player.h>

class Trigger : public Entity {
public:
    BodyID bodyId;
    bool IsActive = true;

    Mutex triggerMutex;

    void Load(BodyCreationSettings* bcs, std::function<void(BodyID)> onTouch);

    void Tick() override;
    void Destroy() override;
    void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;
    void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;
private:
    std::function<void(BodyID)> onTouchFunc;
    Array<BodyID> bodiesInSensor;
};