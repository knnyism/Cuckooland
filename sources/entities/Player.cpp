#include <entities/Player.h>

#include <tween.h>
#include <iostream>

static Vec3 CalculateMoveDirection(Vec3 targetDirection, f32 camAngleX) {
    Quat baseDir = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(camAngleX));
    Vec3 result = baseDir * targetDirection;
    f32 length = result.Length();

    if (length > 0) {
        return result / length;
    }
    else {
        return Vec3(0, 0, 0);
    }
}

static Vec3 Accelerate(Vec3 moveDirection, Vec3 velocity, f32 acceleration, f32 maxSpeed) {
    f32 projectedSpeed = velocity.Dot(moveDirection);
    f32 accelerationSpeed = acceleration * TICK_DURATION;

    if (projectedSpeed + accelerationSpeed > maxSpeed) {
        accelerationSpeed = std::max(maxSpeed - projectedSpeed, 0.0f);
    }

    return velocity + (moveDirection * accelerationSpeed);
}

static Vec3 MoveGround(Vec3 moveDirection, Vec3 velocity) {
    Vec3 horizontal = velocity * Vec3(1, 0, 1);
    f32 speed = horizontal.Length();

    if (speed > 0) {
        f32 drop = speed * PLAYER_FRICTION * TICK_DURATION;
        horizontal *= std::max(speed - drop, 0.0f) / speed;
    }

    return Accelerate(moveDirection, horizontal + Vec3(0, velocity.GetY(), 0), PLAYER_ACCELERATE, PLAYER_MAX_SPEED);
}

static Vec3 MoveAir(Vec3 moveDirection, Vec3 velocity) {
    Vec3 horizontal = velocity * Vec3(1, 0, 1);
    Vec3 vertical = velocity - horizontal;

    f32 speed = horizontal.Length();

    if (speed > PLAYER_MAX_SPEED_AIR) {
        f32 drop = speed * PLAYER_FRICTION_AIR * TICK_DURATION;
        horizontal *= std::max(speed - drop, 0.0f) / speed;
    }

    return Accelerate(moveDirection, horizontal + vertical, PLAYER_ACCELERATE_AIR, PLAYER_GAIN_AIR) + physics_system->GetGravity() * TICK_DURATION;
}

static Vec3 MoveLadder(Vec3 moveDirection, Vec3 ladderNormal) {
    float dot = -moveDirection.Dot(ladderNormal);
    Vec3 accelerationDirection = Vec3(moveDirection.GetX(), dot, moveDirection.GetZ());

    Vec3 vel = (accelerationDirection - (accelerationDirection * Vec3(abs(ladderNormal.GetX()), abs(ladderNormal.GetY()), abs(ladderNormal.GetZ())))) * PLAYER_MAX_SPEED;

    if (vel.LengthSq() > 0) {
        vel -= ladderNormal * 5;
        return vel.Normalized() * min((f32)PLAYER_MAX_SPEED, vel.Length()) * Vec3(0.7, 1, 0.7);
    }

    return Vec3();
}

class LadderObjectLayerFilter : public ObjectLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inObject) const override
    {
        if (inObject == Layers::LADDER)
            return true;

        return false;
    }
};

void Player::Load() {
    body = body_interface->CreateBody(BodyCreationSettings(new CapsuleShape(0.5f * characterHeightStanding - 0.2f, characterRadiusStanding + 0.1f), moveHelper.position, Quat::sIdentity(), EMotionType::Dynamic, Layers::PLAYER));
    moveHelper = MoveHelper(Vec3::sZero(), Vec3::sZero(), body);

    body_interface->AddBody(body->GetID(), EActivation::Activate);
}

void Player::Kill() {
    std::cout << "dead!" << std::endl;
    IsPlayerAlive = false;
}

void Player::Spawn(Vec3 atPos) {
    IsPlayerAlive = true;
    moveHelper.position = atPos;
}

void Player::TraceToLadder(Ref<const Shape> shape, BodyFilter& filter) {
    TraceResult ladderTrace = TraceShape(
        shape,
        moveHelper.position,
        (body_interface->GetPosition(currentLadder) - moveHelper.position).Normalized(),
        filter,
        LadderObjectLayerFilter()
    );

    ladderNormal = ladderTrace.normal;

    if (!ladderTrace.hit) {
        currentLadder = BodyID();
    }
}

void Player::CheckForLadder(Ref<const Shape> shape, BodyFilter& filter) {
    if (!currentLadder.IsInvalid()) {
        TraceToLadder(shape, filter);
    }
    else {
        TraceResult ladderTrace = TraceShape(
            shape,
            moveHelper.position,
            moveHelper.velocity.Normalized(),
            filter,
            LadderObjectLayerFilter()
        );

        if (ladderTrace.hit) {
            currentLadder = ladderTrace.bodyId;
            TraceToLadder(shape, filter);
        }
    }
}

void Player::Tick() {
    Vec3 inputDirection = Vec3(IsKeyDown(KEY_A) - IsKeyDown(KEY_D), 0, IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    Vec3 moveDirection = CalculateMoveDirection(inputDirection, lookAngleX);

    Ref<const Shape> shape = standingShape;

    IgnoreMultipleBodiesFilter filter;
    filter.IgnoreBody(body->GetID());
    filter.IgnoreBody(holdProp); // Early playtesters (me) were using props to boost themselves up to unreachable places, this fixes that!

    TraceResult groundTrace = TraceShape(
        shape,
        moveHelper.position,
        -Vec3::sAxisY() * 0.1f,
        filter
    );

    bool wasCrouching = isCrouching;
    bool wasGrounded = isGrounded;

    bool isOnLadder = !currentLadder.IsInvalid();
    isGrounded = groundTrace.hit && groundTrace.normal.GetY() > 0.8f;

    CheckForLadder(shape, filter);

    if (isGrounded)
    {
        isCrouching = IsKeyDown(KEY_LEFT_CONTROL);

        if (IsKeyDown(KEY_SPACE))
            moveHelper.velocity = Vec3(moveHelper.velocity.GetX(), PLAYER_IMPULSE_JUMP, moveHelper.velocity.GetZ());
        else
            moveHelper.velocity = MoveGround(moveDirection, moveHelper.velocity);

        Vec3 groundPosition = body_interface->GetPosition(groundTrace.bodyId);

        if (wasGrounded && lastGroundBody == groundTrace.bodyId && holdProp != groundTrace.bodyId) {
            Vec3 delta = groundPosition - lastGroundPosition;
            moveHelper.position += delta;
        }

        lastGroundBody = groundTrace.bodyId;
        lastGroundPosition = groundPosition;
    }
    else if (isOnLadder)
    {
        /*if (IsKeyDown(KEY_SPACE))
        {
            moveHelper.velocity = ladderNormal * PLAYER_IMPULSE_JUMP;
        }
        else*/
        moveHelper.velocity = MoveLadder(moveDirection, ladderNormal);
    }
    else
    {
        moveHelper.velocity = MoveAir(moveDirection, moveHelper.velocity);
    }

    if (!wasGrounded && isGrounded) {
        f32 fallSpeed = velocityState.lastState.GetY();

        if (velocityState.lastState.GetY() < -16.0f) {
            landTime = GetTime();
            landDirection = (rand() % 2 == 0) ? 1 : -1;
            landPower = (min(abs(velocityState.lastState.GetY()), 25.0f) / 25.0f) * 5 * DEG2RAD;

            landSound.Play();
        }
    }

    moveHelper.MoveAndSlide(shape);

    body_interface->SetLinearVelocity(body->GetID(), moveHelper.velocity);
    body_interface->SetPosition(body->GetID(), moveHelper.position, EActivation::Activate);
    body_interface->SetRotation(body->GetID(), Quat::sIdentity(), EActivation::Activate);

    positionState.Set(moveHelper.position);
    velocityState.Set(moveHelper.velocity);

    if (isGrounded || isOnLadder) {
        if ((moveHelper.position - lastStepPosition).Length() > 3.5f && moveHelper.velocity.Length() > 0.1f) {
            if (isOnLadder)
                ladderSound.Play();
            else
                stepSound.Play();

            lastStepPosition = moveHelper.position;
        }
    }
}

void Player::BeforeCamera() {
    // TODO: Whoa.. clean this up
    Vector2 mouseDelta = GetMouseDelta();
    Vec3 velocity = velocityState.Get();

    if (!lookLocked) {
        lookAngleX = fmod(lookAngleX - mouseDelta.x * 0.2f, 360);
        lookAngleY = std::clamp(lookAngleY + mouseDelta.y * 0.2f, CAM_LOOK_DOWN, CAM_LOOK_UP);
    }

    Vec3 viewBob = Vec3(0, cos(GetTime() * 8) * 0.2f * (velocity * Vec3(1, 0, 1)).Length() / PLAYER_MAX_SPEED, 0);
    cameraMatrix = Mat44::sRotationTranslation(
        Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(lookAngleX)) * Quat::sRotation(Vec3::sAxisX(), DegreesToRadians(lookAngleY)),
        positionState.Get() + viewBob + Vec3::sAxisY() * cameraHeightStanding
    );

    // Effects
    f32 landAlpha = std::clamp((GetTime() - landTime) / 1.0, 0.0, 1.0);
    Quat roll = Quat::sRotation(-Vec3::sAxisZ(), ((cameraMatrix.GetRotation().GetQuaternion().Inversed() * velocity).GetX() / (PLAYER_MAX_SPEED * 25)));
    Quat landBob = Quat::sRotation(Vec3(1, landDirection, landDirection).Normalized(), (1 - tween::cubicout(landAlpha)) * landPower);

    Mat44 matrixWithEffects = cameraMatrix
        * Mat44::sRotation(roll)
        * Mat44::sRotation(landBob);

    game::SetCameraMatrix(matrixWithEffects);
}