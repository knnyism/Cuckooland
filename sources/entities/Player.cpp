#include <entities/Player.h>

// Projects the target direction to the camera's look vector
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

static Vec3 MoveLadder(Player* player, Vec3 inputDirection, Vec3 ladderNormal) {
    Vec3 lookDir = player->cameraMatrix.GetRotation() * Mat44::sRotationX(-PI / 4) * inputDirection;
    const Vec3 velocityNormal = lookDir.Normalized();

    const Vec3 undesiredMotion = -ladderNormal * velocityNormal.Dot(-ladderNormal);
    const Vec3 desiredMotion = (velocityNormal - undesiredMotion) * PLAYER_MAX_SPEED;

    Vec3 velocity = -ladderNormal * 2;

    if (desiredMotion.Normalized().IsNaN())
    {
        return velocity;
    }

    return velocity + desiredMotion.Normalized() * std::min(desiredMotion.Length(), (float)PLAYER_MAX_SPEED);
}

// Object layer filter for ladder magnetization
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
    BodyCreationSettings bcs = BodyCreationSettings(new CapsuleShape(0.5f * characterHeightStanding - 0.2f, characterRadiusStanding + 0.1f), moveHelper.position, Quat::sIdentity(), EMotionType::Dynamic, Layers::PLAYER);

    body = body_interface->CreateBody(bcs);
    body_interface->AddBody(body->GetID(), EActivation::Activate);

    moveHelper = MoveHelper(Vec3::sZero(), Vec3::sZero(), body);

    fov.stiffness = 500;
    fov.dampening = 40;
}

void Player::Kill() {
    deathSound.Play();
    deathTime = GetTime();

    deathCount++;

    IsPlayerAlive = false;
}

void Player::Spawn() {
    spawnTime = GetTime();

    moveHelper.velocity = Vec3::sZero();
    moveHelper.position = spawnPoint;

    IsPlayerAlive = true;
}

// Are we currently colliding with the ladder we are being magnetized to?
void Player::TraceToLadder(Ref<const Shape> shape, BodyFilter& filter) {
    TraceResult ladderTrace = TraceShape(
        shape,
        moveHelper.position,
        (body_interface->GetPosition(currentLadder) - moveHelper.position).Normalized(),
        filter,
        LadderObjectLayerFilter()
    );

    ladderNormal = ladderTrace.normal.Normalized();

    if (!ladderTrace.hit) {
        currentLadder = BodyID();
    }
}

// Look for ladders that are in our trajectory
// and magnetize if possible
void Player::CheckForLadder(Ref<const Shape> shape, BodyFilter& filter) {
    if (!currentLadder.IsInvalid()) {
        TraceToLadder(shape, filter);
    }
    else {
        if (moveHelper.velocity.LengthSq() > 0.0f) {
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
}

void Player::Tick() {
    if (!IsPlayerAlive) {
        f32 t = GetTime() - deathTime;

        if (t > 4.0f) {
            spawnSound.Play();
            Spawn();
        }

        return;
    }

    Vec3 inputDirection = Vec3::sZero();
    if (!movementLocked) { inputDirection = Vec3(IsKeyDown(KEY_A) - IsKeyDown(KEY_D), 0, IsKeyDown(KEY_W) - IsKeyDown(KEY_S)); }
    Vec3 moveDirection = CalculateMoveDirection(inputDirection, lookAngleX);

    Ref<const Shape> shape = standingShape;

    IgnoreMultipleBodiesFilter filter;
    filter.IgnoreBody(body->GetID());
    filter.IgnoreBody(holdProp); // Early playtesters (me) were using props to boost themselves up to unreachable places, this fixes that!

    // Trace directly down to check if we're currently grounded
    TraceResult groundTrace = TraceShape(
        shape,
        moveHelper.position,
        -Vec3::sAxisY() * 0.1f,
        filter
    );

    bool wasGrounded = isGrounded;

    bool isOnLadder = !currentLadder.IsInvalid();
    isGrounded = groundTrace.hit && groundTrace.normal.GetY() > 0.8f;

    if (isOnLadder && ladderNormal.GetY() < 0.8f) // 
    {
        if (IsKeyDown(KEY_SPACE) && !movementLocked)
        {
            moveHelper.velocity = ladderNormal * PLAYER_IMPULSE_JUMP;
        }
        else
        {
            moveHelper.velocity = MoveLadder(this, inputDirection, ladderNormal);
        }
    }
    else if (isGrounded)
    {
        if (IsKeyDown(KEY_SPACE) && !movementLocked)
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
    else
    {
        moveHelper.velocity = MoveAir(moveDirection, moveHelper.velocity);
    }

    if (!wasGrounded && isGrounded) {
        f32 fallSpeed = velocityState.lastState.GetY();

        if (velocityState.lastState.GetY() < -16.0f) {
            // Play landing animation since we hit the ground hard enough
            landTime = GetTime();
            landDirection = (rand() % 2 == 0) ? 1 : -1;
            landPower = (min(abs(velocityState.lastState.GetY()), 25.0f) / 25.0f) * 5 * DEG2RAD;

            landSound.Play();
        }
    }

    // Move the player capsule and simulate the collisions
    moveHelper.MoveAndSlide(shape);
    CheckForLadder(shape, filter);

    body_interface->SetPositionAndRotation(body->GetID(), moveHelper.position, Quat::sIdentity(), EActivation::DontActivate);
    body_interface->SetLinearAndAngularVelocity(body->GetID(), Vec3::sZero(), Vec3::sZero());

    // Set the interpolation states
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
    Vector2 mouseDelta = GetMouseDelta();
    Vec3 velocity = velocityState.Get();

    f32 fovGoal = FOV_DEFAULT;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !lookLocked) {
        fovGoal = FOV_ZOOMED;
    }

    fov.UpdateWithGoal(fovGoal, GetFrameTime());
    camera->fovy = fov.position;

    if (!lookLocked) {
        // Move the camera with mouse input
        f32 mouseSensitivity = 0.2f * camera->fovy / FOV_DEFAULT;

        lookAngleX = fmod(lookAngleX - mouseDelta.x * mouseSensitivity, 360);
        lookAngleY = std::clamp(lookAngleY + mouseDelta.y * mouseSensitivity, CAM_LOOK_DOWN, CAM_LOOK_UP);
    }

    Vec3 viewBob = Vec3(0, cos(GetTime() * 8) * 0.2f * (velocity * Vec3(1, 0, 1)).Length() / PLAYER_MAX_SPEED, 0);

    cameraMatrix = Mat44::sRotationTranslation(
        Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(lookAngleX)) * Quat::sRotation(Vec3::sAxisX(), DegreesToRadians(lookAngleY)),
        positionState.Get() + viewBob + Vec3::sAxisY() * cameraHeightStanding
    );

    f32 t = GetTime() - spawnTime;

    if (t < 2.5f) {
        cameraMatrix = cameraMatrix * Mat44::sTranslation(
            Vec3(
                (float)GetRandomValue(-100, 100) / 100.0f,
                (float)GetRandomValue(-100, 100) / 100.0f,
                (float)GetRandomValue(-100, 100) / 100.0f
            ) * (2.5f - t) * 0.1f
        );
    }

    // Effects

    f32 landAlpha = std::clamp((GetTime() - landTime) / 1.0, 0.0, 1.0);
    Quat roll = Quat::sRotation(-Vec3::sAxisZ(), ((cameraMatrix.GetRotation().GetQuaternion().Inversed() * velocity).GetX() / (PLAYER_MAX_SPEED * 25)));
    Quat landBob = Quat::sRotation(Vec3(1, landDirection, landDirection).Normalized(), (1 - tween::cubicout(landAlpha)) * landPower);

    Mat44 matrixWithEffects = cameraMatrix
        * Mat44::sRotation(roll)
        * Mat44::sRotation(landBob);

    game::SetCameraMatrix(matrixWithEffects);
}

void Player::AfterCamera() {
    if (!IsPlayerAlive) {
        // Draw the death screen

        f32 t = GetTime() - deathTime;

        deathRectangle.SetSize(Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() });
        deathRectangle.Draw(raylib::Color(0, 0, 0, 255));

        if (t > 1.0f) {
            deathLabel1.object.SetSpacing(4.0f);
            deathLabel1.object.SetFontSize(deathLabel1.GetAbsoluteSize().y);
            deathLabel1.size = game::UDim2(0, deathLabel1.object.MeasureEx().x, deathLabel1.size.y.scale, 0);
            deathLabel1.object.Draw(deathLabel1.GetAbsolutePosition());
        }

        if (t > 3.0f) {
            deathLabel2.object.SetSpacing(4.0f);
            deathLabel2.object.SetFontSize(deathLabel2.GetAbsoluteSize().y);
            deathLabel2.size = game::UDim2(0, deathLabel2.object.MeasureEx().x, deathLabel2.size.y.scale, 0);
            deathLabel2.object.Draw(deathLabel2.GetAbsolutePosition());
        }

        return;
    }

    f32 t = GetTime() - spawnTime;
    if (t < 2.0f) {
        // Draw the spawn effects

        f32 alpha = 1.0f - tween::sineout(std::clamp(t / 2.0f, 0.0f, 1.0f));

        deathRectangle.Draw(raylib::Color(255, 255, 255, GetRandomValue(210, 255) * alpha));
    }
}