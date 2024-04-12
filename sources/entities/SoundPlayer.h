#pragma once

#include <game.h>

// Wrapper entity for play-once sounds
class SoundPlayer : public Entity {
public:
    game::Sound3D* sound;

    bool autoRemove;

    void SoundPlayer::Load(game::Sound3D* sound, bool autoRemove) {
        this->sound = sound;
        this->autoRemove = autoRemove;
    };

    void SoundPlayer::Play() {
        sound->Play();
    }

    void SoundPlayer::Stop() {
        sound->Stop();
    }

    void SoundPlayer::Tick() override {};

    void SoundPlayer::Render() override {
        if (!sound->isLooped && autoRemove) {
            DestroyEntity(this);
        }
    };
};