#pragma once

struct EntityId {
    size_t index = 0;
    int version = -1;

    bool operator==(const EntityId& other) const {
        return index == other.index && version == other.version;
    }
};

class Entity {
public:
    EntityId id;
    bool IsAlive = true;

    virtual ~Entity() = default;

    virtual void Tick() {};
    virtual void Render() {};
private:
};