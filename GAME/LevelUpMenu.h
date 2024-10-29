#pragma once
#include "UIScreen.h"

class LevelUpMenu :
    public UIScreen
{
public:
    LevelUpMenu(class Game* game);
    ~LevelUpMenu();

    void Update(float deltaTime) override;
    void Draw(class Shader* shader) override;

    enum class Slide
    {
        ESlideIn,
        EStop
    };

private:
    Slide mSlideState;
    float mSlideStartPosX;
    float mSlideEndPosX;
    float mSlideSpeed;

    // ƒ{ƒ^ƒ“‚Ì”wŒi—p,
    std::vector<class Texture*> mButtonBGs;
};

