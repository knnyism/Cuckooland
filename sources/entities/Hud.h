#pragma once

#include <game.h>

// Hud entity that draws subtitles, cuckoo count, save icon and end game screen
class Hud : public Entity {
public:
    bool showCuckoos;
    void SetSubtitle(string subtitle);

    void ShowEndGameScreen();
    void ShowSaveIcon();

    void Tick() override;
    void AfterCamera() override;
private:
    raylib::Rectangle subBackdrop;

    s16 points;
    f32 endingTime;

    bool endSoundPlayed = false;
    game::Sound endSound = game::Sound("end_chime");

    bool isShowingEnding;

    game::UIObject<raylib::Texture> saveLabel = game::UIObject<raylib::Texture>(game::UDim2(0.95, 0, 0.95, 0), game::UDim2(0, 0, 0.1, 0), Vector2{ 1.0f, 1.0f }, GetAssetPath("textures/save.png"));

    game::UIObject<raylib::Text> subLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.95, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f });

    raylib::Rectangle endBackdrop;
    game::UIObject<raylib::Text> endTitleLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.3, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "ASSESSMENT COMPLETE");

    game::UIObject<raylib::Text> endTimeLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.4, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "TIME: ?");
    game::UIObject<raylib::Text> endCuckoosLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.45, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "CUCKOOS: ?");
    game::UIObject<raylib::Text> endRecombLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.5, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "RECOMBOBULATIONS: ?");

    game::UIObject<raylib::Text> endRatingLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.6, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "FINAL RATING: ?");

    game::UIObject<raylib::Text> endCommentLabel = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.8, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "Subject did extraordinarily well.");

    f32 saveIconAppearTime;

    string currentSub;
    f32 subDissapearTime;
};