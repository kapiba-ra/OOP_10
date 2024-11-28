#pragma once
#include "Component.h"

// 3D���b�V����`�悷��N���X

class MeshComponent :
    public Component
{
public:
    MeshComponent(Actor* owner, bool isSkeletal = false);
    ~MeshComponent();

    virtual void Draw(class Shader* shader);  // �w��̃V�F�[�_�[�ŕ`�悷��
    virtual void SetMesh(class Mesh* mesh) { mMesh = mesh; }
    void SetTextureIndex(size_t index) { mTextureIndex = index; }

    void SetVisible(bool visible) { mVisible = visible; }
    bool GetVisible() const { return mVisible; }
    bool GetIsSkeletal() const { return mIsSkeletal; }

protected:
    class Mesh* mMesh;
    size_t mTextureIndex;
    bool mVisible;
    bool mIsSkeletal;
};

