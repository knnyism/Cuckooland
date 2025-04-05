/*
* physics.h
*
* This file is responsible for initalizing the Jolt Physics library; it also includes MoveHelper and TraceShape
* MoveHelper is a class that casts a shape to where it is headed, used for precise locomotion
* TraceShape/Ray is a function that casts a shape to a point and returns the result
*/

#pragma once

#include <globals.h>

#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

JPH_SUPPRESS_WARNINGS

using namespace JPH;

namespace Layers
{
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING = 1;
    static constexpr ObjectLayer PLAYER = 2;
    static constexpr ObjectLayer NO_GRAB = 3;
    static constexpr ObjectLayer LADDER = 4;
    static constexpr ObjectLayer TRIGGER = 5;
    static constexpr ObjectLayer NUM_LAYERS = 6;
};

namespace BroadPhaseLayers
{
    static constexpr BroadPhaseLayer NON_MOVING(0);
    static constexpr BroadPhaseLayer MOVING(1);
    static constexpr BroadPhaseLayer PLAYER(2);
    static constexpr BroadPhaseLayer NO_GRAB(3);
    static constexpr BroadPhaseLayer LADDER(4);
    static constexpr BroadPhaseLayer TRIGGER(5);
    static constexpr uint NUM_LAYERS(6);
};


/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING; // Non moving only collides with moving
        case (Layers::MOVING):
            return true;
        case (Layers::PLAYER):
            return true;
        case (Layers::NO_GRAB):
            return true;
        case (Layers::LADDER):
            return true;
        case (Layers::TRIGGER):
            return inObject2 == Layers::PLAYER;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
        mObjectToBroadPhase[Layers::PLAYER] = BroadPhaseLayers::PLAYER;
        mObjectToBroadPhase[Layers::NO_GRAB] = BroadPhaseLayers::NO_GRAB;
        mObjectToBroadPhase[Layers::LADDER] = BroadPhaseLayers::LADDER;
        mObjectToBroadPhase[Layers::TRIGGER] = BroadPhaseLayers::TRIGGER;
    }

    virtual uint GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
    {
        switch ((BroadPhaseLayer::Type)inLayer)
        {
        case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::PLAYER:		return "PLAYER";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::NO_GRAB:		return "NO_GRAB";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::LADDER:		return "LADDER";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::TRIGGER:		return "TRIGGER";
        default:													JPH_ASSERT(false); return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    BroadPhaseLayer	mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case (Layers::MOVING):
            return true;
        case (Layers::PLAYER):
            return true;
        case (Layers::NO_GRAB):
            return true;
        case (Layers::LADDER):
            return true;
        case (Layers::TRIGGER):
            return inLayer2 == BroadPhaseLayers::PLAYER;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

void TraceImpl(const char* inFMT, ...);
bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine);

struct TraceResult {
    bool hit = false;
    bool startedStuck = false;
    f32 fraction = 1.0f;
    f32 depth;
    BodyID bodyId;
    Vec3 point;
    Vec3 planeNormal;
    Vec3 normal;
};

// MoveHelper is basically a virtual physics body that casts a shape to where it is headed
// We can use it to have great precision, prevent ghost collisions and overall smoother locomotion
// Currently only supports MoveAndSlide because that's all I need for now
class MoveHelper {
public:
    Vec3 position;
    Vec3 velocity;
    Body* body;

    MoveHelper(Vec3 position = Vec3(0, 0, 0), Vec3 velocity = Vec3::sZero(), Body* body = nullptr) : position(position), velocity(velocity), body(body) {};

    void MoveAndSlide(Ref<const Shape> shape);
private:
};


class TraceObjectLayerFilter : public ObjectLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inObject) const override
    {
        if (inObject == Layers::TRIGGER)
            return false;

        return true;
    }
};

// TraceShape is pretty much just MoveHelper without a body, used for quick collision checks e.g. ground checks
TraceResult TraceShape(Ref<const Shape> shape, Vec3 origin, Vec3 motion, BodyFilter& bodyFilter = BodyFilter(), ObjectLayerFilter& objectLayerFilter = TraceObjectLayerFilter());
TraceResult TraceShape(Ref<const Shape> shape, Mat44 matrix, Vec3 motion, BodyFilter& bodyFilter = BodyFilter(), ObjectLayerFilter& objectLayerFilter = TraceObjectLayerFilter());

TraceResult TraceRay(Vec3 from, Vec3 to, BodyFilter& bodyFilter = BodyFilter(), ObjectLayerFilter& objectLayerFilter = TraceObjectLayerFilter());

inline PhysicsSystem* physics_system;
inline BodyInterface* body_interface;
inline TempAllocatorImpl* temp_allocator;
inline JobSystemThreadPool* job_system;