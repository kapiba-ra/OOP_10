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

    void AddButtonRandom();

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
    class Slider* mTitleSlider;
    class Slider* mBGSlider;
    std::vector<class Slider*> mButtonSliders;

    // �X�L���A�C�R���Ɛ������̔w�i
    std::vector<class Texture*> mButtonBGs;
    // �A�C�R��
    std::vector<class Texture*> mIcons;
};

