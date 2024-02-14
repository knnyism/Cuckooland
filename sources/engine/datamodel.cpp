#include <engine/datamodel.h>

Entity* FindEntity(EntityId id)
{
    Entity* ent = entities[id.index];

    if (ent->IsAlive && ent->id == id) {
        return ent;
    }

    return nullptr;
}

void DestroyEntity(Entity* ent)
{
    ent->IsAlive = false;
}