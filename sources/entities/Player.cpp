#include <entities/Player.h>

#include <iostream>

const f32 COLLISION_TOLERANCE = 0.05f; // Collision distance from the ground

const u32 PLAYER_ACCELERATE = 150;
const u32 PLAYER_MAX_SPEED = 15;
const u32 PLAYER_FRICTION = 13;

const u32 PLAYER_ACCELERATE_AIR = 200;
const f32 PLAYER_GAIN_AIR = 1;
const u32 PLAYER_MAX_SPEED_AIR = 20;
const f32 PLAYER_FRICTION_AIR = 0.2;

const u32 PLAYER_IMPULSE_JUMP = 17;

static JPH::Vec3 CalculateMoveDirection(JPH::Vec3 targetDirection, f32 camAngleX) {
    JPH::Quat baseDir = JPH::Quat::sRotation(JPH::Vec3::sAxisY(), JPH::DegreesToRadians(camAngleX));
    JPH::Vec3 result = baseDir * targetDirection;
    f32 length = result.Length();

    if (length > 0) {
        return result / length;
    }
    else {
        return JPH::Vec3(0, 0, 0);
    }
}

static JPH::Vec3 Accelerate(JPH::Vec3 moveDirection, JPH::Vec3 velocity, f32 acceleration, f32 maxSpeed) {
    f32 projectedSpeed = velocity.Dot(moveDirection);
    f32 accelerationSpeed = acceleration * TICK_DURATION;

    if (projectedSpeed + accelerationSpeed > maxSpeed) {
        accelerationSpeed = std::max(maxSpeed - projectedSpeed, 0.0f);
    }

    return velocity + (moveDirection * accelerationSpeed);
}

JPH::Vec3 Player::MoveGround(JPH::Vec3 moveDirection, JPH::Vec3 velocity) {
    JPH::Vec3 horizontal = velocity * JPH::Vec3(1, 0, 1);
    f32 speed = horizontal.Length();

    if (speed > 0) {
        f32 drop = speed * PLAYER_FRICTION * TICK_DURATION;
        horizontal *= std::max(speed - drop, 0.0f) / speed;
    }

    return Accelerate(moveDirection, horizontal + JPH::Vec3(0, velocity.GetY(), 0), PLAYER_ACCELERATE, PLAYER_MAX_SPEED);
}

JPH::Vec3 Player::MoveAir(JPH::Vec3 moveDirection, JPH::Vec3 velocity) {
    JPH::Vec3 gravity = JPH::Vec3(0, -56.0f, 0) * TICK_DURATION;

    JPH::Vec3 horizontal = velocity * JPH::Vec3(1, 0, 1);
    JPH::Vec3 vertical = velocity - horizontal;

    f32 speed = horizontal.Length();

    if (speed > PLAYER_MAX_SPEED_AIR) {
        f32 drop = speed * PLAYER_FRICTION_AIR * TICK_DURATION;
        horizontal *= std::max(speed - drop, 0.0f) / speed;
    }

    return Accelerate(moveDirection, horizontal + vertical, PLAYER_ACCELERATE_AIR, PLAYER_GAIN_AIR) + physics_system->GetGravity() * TICK_DURATION;
}

void Player::Spawn() {
    JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
    settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
    settings->mMaxStrength = 100.0f;
    settings->mShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, 0.5f * characterHeightStanding + characterRadiusStanding, 0), JPH::Quat::sIdentity(), new JPH::BoxShape(JPH::Vec3(characterRadiusStanding, 0.5f * characterHeightStanding + characterRadiusStanding, characterRadiusStanding))).Create().Get();
    settings->mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;
    settings->mCharacterPadding = 0.01f;
    settings->mPenetrationRecoverySpeed = 1.0f;
    settings->mPredictiveContactDistance = 0.1f;
    settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -characterRadiusStanding); // Accept contacts that touch the lower sphere of the capsule

    character = new JPH::CharacterVirtual(settings, JPH::RVec3::sZero(), JPH::Quat::sIdentity(), physics_system);
}

void Player::Tick() {
    JPH::Vec3 inputDirection = JPH::Vec3(IsKeyDown(KEY_A) - IsKeyDown(KEY_D), 0, IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    JPH::Vec3 moveDirection = CalculateMoveDirection(inputDirection, lookAngleX);

    character->UpdateGroundVelocity();

    // TODO: Handle SteepGround and NotSupported ground states
    JPH::Vec3 velocity = character->GetLinearVelocity();
    JPH::Vec3 current_vertical_velocity = velocity.Dot(character->GetUp()) * character->GetUp();
    JPH::Vec3 ground_velocity = character->GetGroundVelocity();

    if (character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround)
    {
        if (IsKeyDown(KEY_SPACE))
            velocity = JPH::Vec3(velocity.GetX(), PLAYER_IMPULSE_JUMP, velocity.GetZ());
        else
            velocity = MoveGround(moveDirection, velocity * JPH::Vec3(1, 0, 1));
    }
    else
        velocity = MoveAir(moveDirection, velocity);

    character->SetLinearVelocity(velocity);

    JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
    update_settings.mStickToFloorStepDown = -character->GetUp() * update_settings.mStickToFloorStepDown.Length();
    update_settings.mWalkStairsStepUp = character->GetUp() * update_settings.mStickToFloorStepDown.Length();

    character->ExtendedUpdate(TICK_DURATION,
        -character->GetUp() * physics_system->GetGravity().Length(),
        update_settings,
        physics_system->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
        physics_system->GetDefaultLayerFilter(Layers::MOVING),
        { },
        { },
        *temp_allocator);

    for (auto& contact : character->GetActiveContacts()) {
        if (contact.mHadCollision) {
            velocity -= velocity.Dot(contact.mContactNormal) * contact.mContactNormal;
        }
    }

    character->SetLinearVelocity(velocity);

    interpState.lastTime = interpState.nextTime;
    interpState.nextTime = GetTime();

    interpState.lastPosition = interpState.nextPosition;
    interpState.nextPosition = character->GetPosition();
}

const f32 CAM_LOOK_UP = 89.9f;
const f32 CAM_LOOK_DOWN = -89.9f;

void Player::Render() {
    Vector2 mouseDelta = GetMouseDelta();
    lookAngleX = fmod(lookAngleX - mouseDelta.x * 0.2f, 360);
    lookAngleY = std::clamp(lookAngleY + mouseDelta.y * 0.2f, CAM_LOOK_DOWN, CAM_LOOK_UP);

    // Set up interpolation
    f64 nowTime = GetTime() - TICK_DURATION; // We want to render the player one tick behind
    f64 alpha = std::clamp((nowTime - interpState.lastTime) / (interpState.nextTime - interpState.lastTime), 0.0, 1.0);
    JPH::Vec3 nowPosition = interpState.lastPosition + (interpState.nextPosition - interpState.lastPosition) * alpha;

    // TODO: Function that sets the camera position and target using the look angles lol, might need this elsewhere
    JPH::Vec3 cameraPosition = nowPosition + JPH::Vec3::sAxisY() * cameraHeightStanding;
    JPH::Quat rotation = JPH::Quat::sRotation(JPH::Vec3::sAxisY(), JPH::DegreesToRadians(lookAngleX)) * JPH::Quat::sRotation(JPH::Vec3::sAxisX(), JPH::DegreesToRadians(lookAngleY));
    JPH::Vec3 target = cameraPosition + rotation * JPH::Vec3::sAxisZ();

    camera->SetPosition({ cameraPosition.GetX(), cameraPosition.GetY(), cameraPosition.GetZ() });
    camera->SetTarget({ target.GetX() , target.GetY(), target.GetZ() });
}

void Player::OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
}