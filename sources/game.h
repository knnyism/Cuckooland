#pragma once

#include <physics.h>
#include <interpolation.h>

#include <raylib-cpp.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

#include <filesystem>
#include <string>
#include <vector>

constexpr u32 ENTITY_BUCKET_SIZE = 10;

struct EntityId {
    size_t bucket = 0;
    size_t index = 0;
    int version = -1;

    bool operator==(const EntityId& other) const {
        return
            bucket == other.bucket &&
            index == other.index &&
            version == other.version;
    }
};

class Entity {
public:
    EntityId id;
    bool IsAlive = false;

    virtual ~Entity() = default;

    virtual void Tick() {};
    virtual void Render() {};
    virtual void Destroy() {};
private:
};

class Bucket {
public:
    u64 count = 0;
    Entity* data[ENTITY_BUCKET_SIZE] = {};
};

// Before I came up with a solution using bucket arrays, I was using a vector to keep pointers to entities.
// This was a bad idea because it would invalidate the pointers when the vector resized, which means I can't
// spawn an entity during a tick. Bucket arrays do not have this problem as they have a fixed size.
class EntityBucket {
private:
    u64 count = 0;

    void addBucket() {
        Bucket* bucket = new Bucket();

        for (int i = 0; i < ENTITY_BUCKET_SIZE; i++) {
            bucket->data[i] = new Entity();
        }

        buckets.push_back(bucket);
    }
public:
    std::vector<Bucket*> buckets;

    EntityBucket() {
        addBucket(); // Create the first bucket or else our game will crash!
    }

    void insert(Entity* entity) {

        // If we don't have any space left in the current bucket, create a new one
        if (buckets[count]->count >= ENTITY_BUCKET_SIZE) {
            addBucket();
            count++;
        }

        Bucket* bucket = buckets[count];
        entity->id.bucket = count;
        entity->id.index = bucket->count;

        std::cout << "ENT: CREATE: at bucket " << count << " with id " << bucket->count << std::endl;

        bucket->data[bucket->count] = entity;
        bucket->count++;
    }
};

inline EntityBucket entities;

template <typename T>
T* CreateEntity() {
    T* ent = nullptr;

    // Look for a dead entity so we can potentially reuse it
    for (auto& bucket : entities.buckets)
        for (Entity* entity : bucket->data) {
            if (entity->IsAlive) {
                continue;
            }

            T* t = dynamic_cast<T*>(entity);

            if (t == nullptr) {
                continue;
            }

            std::cout << "ENT: REUSE: at bucket " << entity->id.bucket << " with id " << entity->id.index << std::endl;

            ent = t;
            break;
        }

    // If we didn't find a dead entity, create a new one
    if (ent == nullptr) {
        ent = new T();
        entities.insert(ent);
    }

    ent->id.version++; // Bump the version number so we don't have to deal with irrelevant entities
    ent->IsAlive = true;

    return ent;
};

Entity* FindEntity(EntityId id);
void DestroyEntity(Entity* ent);

namespace game {
    // Simple class to hold a model and its physics body
    // Uses the raylib-cpp model class for rendering, and the Jolt physics body for physics
    // Of course, we also have to interpolate the model's position and rotation
    class Model {
    public:
        JPH::BodyID bodyId;

        void Model::Load(string path, Vec3 position = Vec3::sZero(), Quat rotation = Quat::sIdentity(), f32 overrideMass = -1.0f, EMotionType motionType = EMotionType::Dynamic);
        void Model::Tick();
        void Model::Render();
    private:
        raylib::Model raylibModel;
        InterpState interpState;
    };
}