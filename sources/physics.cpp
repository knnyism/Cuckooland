#include <physics.h>

void TraceImpl(const char* inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    std::cout << buffer << std::endl;
}
bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
{
    // Print to the TTY
    std::cout << "JOLT: ERROR: " << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;

    // Breakpoint
    return false;
};

TraceResult TraceShape(Ref<const Shape> shape, Mat44 matrix, Vec3 motion, BodyFilter& bodyFilter, ObjectLayerFilter& objectLayerFilter) {
    TraceResult traceResult;
    ClosestHitCollisionCollector<CastShapeCollector> cast_shape_collector;
    physics_system->GetNarrowPhaseQuery().CastShape(
        RShapeCast(shape, Vec3::sReplicate(1), matrix, motion),
        ShapeCastSettings(),
        RVec3::sZero(),
        cast_shape_collector,
        {},
        objectLayerFilter,
        bodyFilter
    );

    if (cast_shape_collector.HadHit())
    {
        traceResult.hit = true;

        traceResult.normal = -cast_shape_collector.mHit.mPenetrationAxis.Normalized();
        traceResult.bodyId = cast_shape_collector.mHit.mBodyID2;
        traceResult.point = cast_shape_collector.mHit.mContactPointOn2;
        traceResult.depth = cast_shape_collector.mHit.mPenetrationDepth;
        traceResult.fraction = cast_shape_collector.mHit.mFraction;
        traceResult.startedStuck = false;
    }

    return traceResult;
};

TraceResult TraceShape(Ref<const Shape> shape, Vec3 origin, Vec3 motion, BodyFilter& bodyFilter, ObjectLayerFilter& objectLayerFilter) {
    return TraceShape(shape, Mat44::sTranslation(origin), motion, bodyFilter, objectLayerFilter);
}

TraceResult TraceRay(Vec3 from, Vec3 direction, BodyFilter& bodyFilter, ObjectLayerFilter& objectLayerFilter) {
    TraceResult traceResult;

    RayCastResult rayCastResult;
    bool hit = physics_system->GetNarrowPhaseQuery().CastRay(
        RRayCast(from, direction),
        rayCastResult,
        {},
        objectLayerFilter,
        bodyFilter
    );

    if (hit) {
        traceResult.hit = true;
        traceResult.bodyId = rayCastResult.mBodyID;
        traceResult.point = from + direction * rayCastResult.mFraction;
        traceResult.fraction = rayCastResult.mFraction;
    }

    return traceResult;
}


void MoveHelper::MoveAndSlide(Ref<const Shape> shape) {
    f32 travelLeft = 1.0f;
    int iterations = 10; // TODO: why does BoxShape cause a crash here
    BodyInterface& bodyInterface = physics_system->GetBodyInterface();

    while (travelLeft > 0 && iterations > 0) {
        const Vec3 rayDirection = velocity * TICK_DURATION * travelLeft;
        const TraceResult traceResult = TraceShape(shape, position, rayDirection, IgnoreSingleBodyFilter(body->GetID()));

        if (!traceResult.hit) {
            position = position + rayDirection;
            break;
        }

        const f32 velocityLength = velocity.Length();
        const Vec3 velocityNormal = velocity.Normalized();

        const Vec3 undesiredMotion = -traceResult.normal * velocityNormal.Dot(-traceResult.normal);
        const Vec3 desiredMotion = velocityNormal - undesiredMotion;

        position = position + rayDirection * traceResult.fraction + traceResult.normal * 0.001f;
        velocity = desiredMotion * velocityLength;

        travelLeft -= traceResult.fraction;
        iterations--;
    }
}