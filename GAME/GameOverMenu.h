#pragma once
#include "UIScreen.h"
#include "Math.h"

class GameOverMenu :
    public UIScreen
{
public:
    GameOverMenu(class Game* game);
    ~GameOverMenu();

    void Draw(class Shader* shader) override;

private:
    class Texture* mContinueText;
    Vector2 mContinueTextPos;
};

