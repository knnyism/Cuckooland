#include <entities/Player.h>

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

void Player::Spawn() {

}

void Player::Tick() {
    Vec3 inputDirection = Vec3(IsKeyDown(KEY_A) - IsKeyDown(KEY_D), 0, IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    Vec3 moveDirection = CalculateMoveDirection(inputDirection, lookAngleX);

    Ref<Shape> shape = isCrouching ? crouchingShape : standingShape;

    moveHelper.MoveAndSlide(shape);

    TraceResult groundTrace = TraceShape(shape, moveHelper.position, -Vec3::sAxisY() * 0.1f);
    isGrounded = groundTrace.hit && groundTrace.normal.GetY() > 0.8f;

    wasCrouching = isCrouching;

    if (isGrounded)
    {
        isCrouching = IsKeyDown(KEY_LEFT_CONTROL);

        if (IsKeyDown(KEY_SPACE))
            moveHelper.velocity = Vec3(moveHelper.velocity.GetX(), PLAYER_IMPULSE_JUMP, moveHelper.velocity.GetZ());
        else
            moveHelper.velocity = MoveGround(moveDirection, moveHelper.velocity * Vec3(1, 0, 1));
    }
    else
    {
        if (IsKeyDown(KEY_LEFT_CONTROL))
            isCrouching = true;

        moveHelper.velocity = MoveAir(moveDirection, moveHelper.velocity);
    }

    if (isCrouching != wasCrouching) {
        moveHelper.position = moveHelper.position + Vec3(0, isCrouching ? 0 : (characterHeightStanding - characterHeightCrouching), 0);
    }

    interpState.UpdatePosition(GetTime(), moveHelper.position);
}

const f32 CAM_LOOK_UP = 89.99f;
const f32 CAM_LOOK_DOWN = -89.99f;

void Player::Render() {
    Vector2 mouseDelta = GetMouseDelta();
    lookAngleX = fmod(lookAngleX - mouseDelta.x * 0.2f, 360);
    lookAngleY = std::clamp(lookAngleY + mouseDelta.y * 0.2f, CAM_LOOK_DOWN, CAM_LOOK_UP);

    Vec3 viewBob = Vec3(0, std::cos(GetTime() * 8) * 0.2f * (moveHelper.velocity - Vec3::sAxisY() * moveHelper.velocity.GetY()).Length() / PLAYER_MAX_SPEED, 0);

    f32 cameraHeight = isCrouching ? cameraHeightCrouching : cameraHeightStanding;

    // TODO: Function that sets the camera position and target using the look angles
    Vec3 cameraPosition = interpState.GetPosition() + viewBob + Vec3::sAxisY() * cameraHeight;
    Quat rotation = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(lookAngleX)) * Quat::sRotation(Vec3::sAxisX(), DegreesToRadians(lookAngleY));
    Vec3 target = cameraPosition + rotation * Vec3::sAxisZ();

    camera->SetPosition({ cameraPosition.GetX(), cameraPosition.GetY(), cameraPosition.GetZ() });
    camera->SetTarget({ target.GetX() , target.GetY(), target.GetZ() });
}