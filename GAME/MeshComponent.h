#pragma once
#include "Component.h"

// 3Dメッシュを描画するクラス

class MeshComponent :
    public Component
{
public:
    MeshComponent(Actor* owner);
    ~MeshComponent();

    virtual void Draw(class Shader* shader);  // 指定のシェーダーで描画する
    virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }
    void SetTextureIndex(size_t index) { mTextureIndex = index; }

    void SetVisible(bool visible) { mVisible = visible; }
    bool GetVisible() const { return mVisible; }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
    bool mVisible;
};

