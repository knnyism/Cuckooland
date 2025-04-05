/*
* game.h
*
* This file contains methods used for entity management, and game specific classes: Sound and Model and UIObject.
*/

#pragma once

#include <physics.h>
#include <interpolation.h>
#include <resources.h>
#include <tween.h>

#include <raylib-cpp.hpp>

#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include <filesystem>
#include <string>
#include <vector>

constexpr u32 ENTITY_BUCKET_SIZE = 32;
constexpr u8 MAX_SOUNDS = 255;

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
    virtual void BeforeCamera() {};
    virtual void AfterCamera() {};
    virtual void Render() {};

    virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) {};
    virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) {};
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
// spawn an entity during a tick. Bucket arrays do not have this problem as each bucket has a fixed size.
class EntityBucket {
private:
    u64 count = 0;

    void addBucket() {
        Bucket* bucket = new Bucket();

        for (u32 i = 0; i < ENTITY_BUCKET_SIZE; i++) {
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
        if (buckets[count]->count >= ENTITY_BUCKET_SIZE) { // If we don't have any space left in the current bucket, create a new one
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
    // Helper function to set camera matrix, raylib doesn't have this built in
    // NOTE: This method is also used for sound listener position!
    void SetCameraMatrix(Mat44 matrix);
    Mat44 GetCameraMatrix();

    // Simple class to hold a raylib model and its physics body
    // Uses the raylib-cpp model class for rendering, and the Jolt physics body for physics
    // Of course, we also have to interpolate the model's position and rotation
    class Model {
    public:
        JPH::Body* body;
        JPH::BodyID bodyId;

        void Model::Load( std::string path, Vec3 position = Vec3::sZero(), Quat rotation = Quat::sIdentity(), f32 overrideMass = -1.0f, f32 overrideFriction = 0.5f, EMotionType motionType = EMotionType::Dynamic, ObjectLayer layer = Layers::MOVING);
        void Model::Tick();
        void Model::Render();

        raylib::Model* raylibModel;
    private:
        InterpState<Vec3> positionState;
        InterpState<Quat> rotationState;
    };

    // Simple class to hold a sound and its state
    // It will behave as if it is attached to the camera
    // Used for music, UI Sounds, ambience etc.
    class Sound {
    public:
        raylib::Sound* currentSound;

        bool isLooped = false;
        bool isPlaying = false;
        bool isStopped = false;

        f32 volume = 0.5f;
        f32 pitch = 1.0f;

        f32 timePosition = 0.0f;
        u8 maxInstances;
        u8 maxVariations;

        std::string path;

        Sound( std::string soundPath, u8 maxInstances = 8, u8 maxVariations = 1, bool isLooped = false) : maxInstances(maxInstances), maxVariations(maxVariations), isLooped(isLooped) {
            path = soundPath;
            if (maxVariations > 1) {
                for (u8 i = 0; i < maxInstances; i++) {
                    u8 variation = i % maxVariations;
                    soundArray[i] = GetSound(soundPath + std::to_string(variation + 1));
                }
            }
            else {
                for (u8 i = 0; i < maxInstances; i++) {
                    soundArray[i] = GetSound(soundPath);
                }
            }

            currentSound = soundArray[0];
        }

        virtual void Play();
        virtual void Stop();
    protected:
        raylib::Sound* soundArray[MAX_SOUNDS];
        u8 soundIndex = 0;
    };

    // Same as Sound, but with a 3D position
    // Used for sounds that are attached to entities
    class Sound3D : public Sound {
    public:
        Vec3 position;
        f32 maxDistance = 50.0f;

        Sound3D() = default;

        Sound3D( std::string soundPath, Vec3 position = Vec3::sZero(), u8 maxInstances = 8, u8 maxVariations = 1, bool isLooped = false) : Sound(soundPath, maxInstances, maxVariations, isLooped), position(position) {};

        void Play() override;
        void Stop() override;
        void Update();
    };

    // This data type represents a one-dimensional value with two components, a relative scale and an absolute offset
    // The intended use-cases for UDims are usually related to UI code 
    class UDim {
    public:
        f32 scale;
        f32 offset;

        UDim(f32 scale = 0.0f, f32 offset = 0.0f) : scale(scale), offset(offset) {}
    };

    class UDim2 {
    public:
        UDim x;
        UDim y;

        UDim2(f32 xScale = 0.0f, f32 xOffset = 0.0f, f32 yScale = 0.0f, f32 yOffset = 0.0f) : x(xScale, xOffset), y(yScale, yOffset) {}
    };

    // Wrapper class for raylib objects that can work with (2d) position and size values
    template <typename T>
    class UIObject {
    public:
        T object;

        UDim2 position;
        UDim2 size;

        Vector2 pivot;

        template<typename... Args>
        UIObject(UDim2 position = UDim2(), UDim2 size = UDim2(), Vector2 pivot = { 0.0f, 0.0f }, Args&&... args) : position(position), size(size), pivot(pivot), object(std::forward<Args>(args)...) {}

        Vector2 GetAbsoluteSize() {
            return {
                GetScreenWidth() * size.x.scale + size.x.offset,
                GetScreenHeight() * size.y.scale + size.y.offset
            };
        }

        Vector2 GetAbsolutePosition() {
            Vector2 absoluteSize = GetAbsoluteSize();

            return {
                GetScreenWidth() * position.x.scale + position.x.offset - absoluteSize.x * pivot.x,
                GetScreenHeight() * position.y.scale + position.y.offset - absoluteSize.y * pivot.y
            };
        }
    };

    void UpdateSound3Ds();
}