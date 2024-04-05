#include <entities/Hud.h>

std::map<string, string> subtitleMap = {
    {"wake_up", "Hey you, the one trapped in that cell! WAKE UP!"},
    {"glad_wokeup", "Ah, you're not dead yet, great!"},
    {"time_to_test", "We've got a few experiments to conduct regarding our new invention."},
    {"will_you", "Now, step out of that room."},

    {"trick", "There's something behind that door, you've got to see this..."},
    {"tricked", "Got you."},
    {"no_regrets", "Alright fine! Stay in there. No regrets."},

    {"this_is", "This is the Transform Cannon [Patent Pending]."},
    {"ignore_fancy", "Ignore the fancy name, it just lets you move objects."},
    {"practice", "Let's practice using those boxes in the corner."},
    {"whats_that", "What's that? A cuckoo...?"},
    {"theyre_everywhere", "Oh right, you should collect those! They're everywhere around the facility."},

    {"goto_door", "Alright, now go through that door to advance to the next experiment."},
    {"door_stuck", "Oh great, this place really is falling apart. Just use the fire exit, I guess."},

    {"pulley", "This experiment involves a seesaw lift. Here's a clue on how it works..."},
    {"shaft", "Enough effort lifts the load."},

    {"nice", "Nicely done!"},

    {"what_is", "What is with this place and doors getting stuck?"},
    {"i_wonder", "Oh, I wonder who put that there..."},

    {"next_test", "We ensured this next experiment to be OSHA compliant."},
    {"ladder", "Also, in case you fall, we left a ladder in there for you."},
    {"good_luck", "Good luck."},
};

std::map<string, string> tutorialMap = {
    {"wasd", "WASD / Move around"},
    {"space", "SPACE / Jump"},
    {"ladder", "WASD / Navigate on ladder"},
    {"mouse1", "LEFT MOUSE / Hold object"},
    {"rotate", "R / Rotate held object"},
    {"scroll", "SCROLL / Bring object closer or further away"}
};

void Hud::SetSubtitle(string subtitle) {
    currentSub = subtitleMap[subtitle];
    subDissapearTime = GetTime() + currentSub.length() * 0.1 + 0.5;
}

void Hud::ShowEndGameScreen() {
    endingTime = GetTime();

    isShowingEnding = true;
    showCuckoos = false;

    points += cuckooCount * 2;
    points -= deathCount;

    f32 timePassed = endingTime - startTime;

    string mins = std::to_string((u8)std::floor(timePassed / 60)); // The voices are telling me...
    string secs = std::to_string((u8)std::fmod(std::floor(timePassed), 60)); // "There has to be a better way!"

    if (secs.length() == 1) {
        secs = "0" + secs;
    }

    string timeFormatted = mins + ":" + secs;

    string comment = TextFormat("Subject failed successfully. %i cuckoos were left uncollected.", totalCuckoos - cuckooCount);

    if (cuckooCount >= totalCuckoos) {
        comment = "Subject did extraordinarily well.";
    }
    else if (cuckooCount >= totalCuckoos / 2) {
        comment = TextFormat("Subject has completed all tasks. However, %i cuckoos still remain.", totalCuckoos - cuckooCount);
    }

    if (deathCount >= 5) {
        comment = comment + "\n\nThe subject recombobulated way too many times.";
    }

    endTitleLabel.object.SetSpacing(4.0f);
    endTitleLabel.size = game::UDim2(0, endTitleLabel.object.MeasureEx().x, endTitleLabel.size.y.scale, 0);

    endTimeLabel.object.SetSpacing(4.0f);
    endTimeLabel.size = game::UDim2(0, endTimeLabel.object.MeasureEx().x, endTimeLabel.size.y.scale, 0);
    endTimeLabel.object.SetText("TIME: " + timeFormatted);

    endCuckoosLabel.object.SetSpacing(4.0f);
    endCuckoosLabel.size = game::UDim2(0, endCuckoosLabel.object.MeasureEx().x, endCuckoosLabel.size.y.scale, 0);
    endCuckoosLabel.object.SetText("CUCKOOS: " + std::to_string(cuckooCount));

    endRecombLabel.object.SetSpacing(4.0f);
    endRecombLabel.size = game::UDim2(0, endRecombLabel.object.MeasureEx().x, endRecombLabel.size.y.scale, 0);
    endRecombLabel.object.SetText("RECOMBOBULATIONS: " + std::to_string(deathCount));

    endRatingLabel.object.SetSpacing(4.0f);
    endRatingLabel.size = game::UDim2(0, endRatingLabel.object.MeasureEx().x, endRatingLabel.size.y.scale, 0);
    endRatingLabel.object.SetText("FINAL RATING: " + std::to_string(points));

    endCommentLabel.object.SetSpacing(4.0f);
    endCommentLabel.size = game::UDim2(0, endCommentLabel.object.MeasureEx().x, endCommentLabel.size.y.scale, 0);
    endCommentLabel.object.SetText("ABSTRACT:\n\n" + comment);
}

void Hud::ShowSaveIcon() {
    saveIconAppearTime = GetTime();
}

void Hud::Tick() {
    if (subDissapearTime - GetTime() <= 0.0f)
        currentSub = "";
}

void Hud::AfterCamera() {
    if (GetTime() - saveIconAppearTime < 5.0f) {
        saveLabel.size = game::UDim2(0, saveLabel.GetAbsoluteSize().y, 0.1, 0);

        saveLabel.object.SetWidth(saveLabel.GetAbsoluteSize().x);
        saveLabel.object.SetHeight(saveLabel.GetAbsoluteSize().y);

        saveLabel.object.Draw(saveLabel.GetAbsolutePosition());
    }

    if (showCuckoos)
        raylib::DrawText("Cuckoos: " + std::to_string(cuckooCount), 10, 10, 24, raylib::WHITE);

    if (isShowingEnding) {
        f32 t = GetTime() - endingTime;

        f32 alpha = tween::sineout(std::clamp(t / 3.0f, 0.0f, 1.0f));
        f32 alpha2 = tween::sineout(std::clamp((t - 1.0f) / 3.0f, 0.0f, 1.0f));
        f32 alpha3 = tween::sineout(std::clamp((t - 1.5f) / 3.0f, 0.0f, 1.0f));
        f32 alpha4 = tween::sineout(std::clamp((t - 2.0f) / 3.0f, 0.0f, 1.0f));
        f32 alpha5 = tween::sineout(std::clamp((t - 4.0f) / 3.0f, 0.0f, 1.0f));
        f32 alpha6 = tween::sineout(std::clamp((t - 4.5f) / 3.0f, 0.0f, 1.0f));

        endBackdrop.SetSize(Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() });
        endBackdrop.Draw(raylib::Color(0, 0, 0, 255 * alpha));

        endTitleLabel.object.SetColor(raylib::Color(255, 255, 255, 255 * alpha));
        endTitleLabel.object.SetFontSize(endTitleLabel.GetAbsoluteSize().y);
        endTitleLabel.size = game::UDim2(0, endTitleLabel.object.MeasureEx().x, endTitleLabel.size.y.scale, 0);
        endTitleLabel.object.Draw(endTitleLabel.GetAbsolutePosition());

        endTimeLabel.object.SetColor(raylib::Color(255, 255, 255, 255 * alpha2));
        endTimeLabel.object.SetFontSize(endTimeLabel.GetAbsoluteSize().y);
        endTimeLabel.size = game::UDim2(0, endTimeLabel.object.MeasureEx().x, endTimeLabel.size.y.scale, 0);
        endTimeLabel.object.Draw(endTimeLabel.GetAbsolutePosition());

        endCuckoosLabel.object.SetColor(raylib::Color(255, 255, 255, 255 * alpha3));
        endCuckoosLabel.object.SetFontSize(endCuckoosLabel.GetAbsoluteSize().y);
        endCuckoosLabel.size = game::UDim2(0, endCuckoosLabel.object.MeasureEx().x, endCuckoosLabel.size.y.scale, 0);
        endCuckoosLabel.object.Draw(endCuckoosLabel.GetAbsolutePosition());

        endRecombLabel.object.SetColor(raylib::Color(255, 255, 255, 255 * alpha4));
        endRecombLabel.object.SetFontSize(endRecombLabel.GetAbsoluteSize().y);
        endRecombLabel.size = game::UDim2(0, endRecombLabel.object.MeasureEx().x, endRecombLabel.size.y.scale, 0);
        endRecombLabel.object.Draw(endRecombLabel.GetAbsolutePosition());

        endRatingLabel.object.SetColor(raylib::Color(255, 255, 255, 255 * alpha5));
        endRatingLabel.object.SetFontSize(endRatingLabel.GetAbsoluteSize().y);
        endRatingLabel.size = game::UDim2(0, endRatingLabel.object.MeasureEx().x, endRatingLabel.size.y.scale, 0);
        endRatingLabel.object.Draw(endRatingLabel.GetAbsolutePosition());

        endCommentLabel.object.SetColor(raylib::Color(255, 255, 255, 255 * alpha6));
        endCommentLabel.object.SetFontSize(endCommentLabel.GetAbsoluteSize().y);
        endCommentLabel.size = game::UDim2(0, endCommentLabel.object.MeasureEx().x, endCommentLabel.size.y.scale, 0);
        endCommentLabel.object.Draw(endCommentLabel.GetAbsolutePosition());

        if (t > 4.0f && !endSoundPlayed) {
            endSound.Play();
            endSoundPlayed = true;
        }

        if (t > 15.0f) {
            quitRequested = true;
        }
    }

    subLabel.object.SetText(currentSub);
    subLabel.object.SetFontSize(subLabel.GetAbsoluteSize().y);
    subLabel.object.SetSpacing(4.0f);
    subLabel.size = game::UDim2(0, subLabel.object.MeasureEx().x, subLabel.size.y.scale, 0);

    subBackdrop.SetPosition(subLabel.GetAbsolutePosition());
    subBackdrop.SetSize(subLabel.GetAbsoluteSize());
    subBackdrop.Draw(raylib::Color(0, 0, 0, 255));

    subLabel.object.Draw(subLabel.GetAbsolutePosition());
}