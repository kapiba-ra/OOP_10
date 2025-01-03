#pragma once
#include "Component.h"

class SpriteComponent :
    public Component
{
public:
    SpriteComponent(class Actor* owner, int drawOrder = 100);
    ~SpriteComponent();

    virtual void Draw(class Shader* shader);
    virtual void SetTexture(class Texture* texture);

    int GetDrawOrder() const { return mDrawOrder; }
    int GetTexWidth() const { return mTexWidth; }
    int GetTexHeight() const { return mTexHeight; }

    void SetVisible(bool visible) { mVisible = visible; }
    bool GetVisible() const { return mVisible; }

protected:
    class Texture* mTexture;
    int mDrawOrder;
    int mTexWidth;
    int mTexHeight;
    bool mVisible;
};
