#include <entities/Intro.h>

void Intro::Play(Player* player, Vec3 pos)
{
    this->player = player;

    animationTime = GetTime();
    startPos = pos;
}

void Intro::BeforeCamera() {
    f32 t = GetTime() - animationTime - 7.0f;

    if (!lightsOn && t > -1.0f) {
        lightsOnSound.Play();
        CreateLight(LIGHT_SPOT, { 0.0f, 7.934f, 0.0f }, { -0.0f, 0.0f, -0.0f }, { 255, 0, 0 }, 40.0f);
        lightsOn = true;
    }

    f32 alpha = tween::cubicout(tween::cubicin(std::clamp(t / 3.3f, 0.0f, 1.0f)));
    f32 alpha2 = tween::backout(tween::cubicin(std::clamp(t / 3.0f, 0.0f, 1.0f)));
    f32 alpha3 = tween::cubicout(tween::cubicin(std::clamp(t / 3.2f, 0.0f, 1.0f)));

    Vec3 position = startPos + (player->moveHelper.position + Vec3::sAxisY() * player->cameraHeightStanding - startPos) * alpha;
    Quat rotation = Quat::sEulerAngles(Vec3(0, 90.0f * (1.0f - alpha3), 90.0f * (1.0f - alpha2)) * DEG2RAD);

    if (t > 3.2f) {
        player->lookLocked = false;
        player->movementLocked = false;
        DestroyEntity(this);
    }

    game::SetCameraMatrix(Mat44::sRotationTranslation(rotation, position));
}

void Intro::AfterCamera() {
    f32 t = GetTime() - animationTime - 2.0f;

    f32 alpha = tween::sineout(std::clamp(t / 3.0f, 0.0f, 1.0f));

    fadeIn.SetSize(Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() });
    fadeIn.Draw(raylib::Color(0, 0, 0, 255 * (1.0f - alpha)));
}
