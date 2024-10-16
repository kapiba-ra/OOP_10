#pragma once
#include "UIScreen.h"
//#include "Math.h"

class GameClearMenu :
    public UIScreen
{
public:
    GameClearMenu(class Game* game);
    ~GameClearMenu();

    void Draw(class Shader* shader) override;

private:
    class Texture* mComment;
    Vector2 mCommentPos;
};

