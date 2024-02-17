#pragma once

#include <entity.h>
#include <vector>

inline std::vector<Entity*> entities;

template <typename T>
T* CreateEntity() {
    T* ent = nullptr;

    // Look for a dead entity so we can potentially reuse it
    for (Entity* entity : entities) {
        if (entity->IsAlive) {
            continue;
        }

        T* t = dynamic_cast<T*>(entity);

        if (t == nullptr) {
            continue;
        }

        ent = t;
        break;
    }

    // If we didn't find a dead entity, create a new one
    if (ent == nullptr) {
        ent = new T();
        ent->id.index = entities.size();

        entities.push_back(ent);
    }


    ent->id.version++; // Bump the version number so we don't have to deal with irrelevant entities
    ent->IsAlive = true;

    return ent;
};
Entity* FindEntity(EntityId id);
void DestroyEntity(Entity* ent);
