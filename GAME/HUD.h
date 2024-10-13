#pragma once
#include "UIScreen.h"
#include <vector>

// HUD(ヘッドアップディスプレイ)
// コンピューターゲーム内で画面に重ねて表示される情報のこと

class HUD :
    public UIScreen
{
public:
    HUD(class Game* game);
    ~HUD();

    void Update(float deltaTime) override;
    void Draw(class Shader* shader) override;

    void AddTargetComponent(class TargetComponent* tc);
    void RemoveTargetComponent(class TargetComponent* tc);

protected:
    // Update内で呼び出すヘルパー関数
    void UpdateCrosshair(float deltaTime);
    void UpdateRadar(float deltaTime);

    // 十字線のテクスチャ
    class Texture* mCrosshair;
    class Texture* mCrosshairEnemy;
    // 十字線が敵を捉えているか
    bool mTargetEnemy;

    // HPバーのテクスチャ
    class Texture* mHPbar;

    // レーダー用のテクスチャ
    class Texture* mRader;
    class Texture* mBlipTex;
    // レーダー中心から輝点への2D相対オフセット
    std::vector<Vector2> mBlips;
    // レーダーの(ワールド空間での)範囲と(レーダーディスプレイの)半径
    float mRaderRange;
    float mRaderRadius;


    std::vector<class TargetComponent*> mTargetComps;
private:
    float mHPdiscardRange;
public:
    void SetHPdiscardRange(float range) { mHPdiscardRange = range; }
};

