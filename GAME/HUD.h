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
    void Reset() override;
    void ResetTimer();

    void AddTargetComponent(class TargetComponent* tc);
    void RemoveTargetComponent(class TargetComponent* tc);

protected:
    // Update内で呼び出すヘルパー関数
    void UpdateRadar(float deltaTime);
    void UpdateTimer(float deltaTime);

    // HPバーのテクスチャ
    class Texture* mHPbar;
    class Texture* mHPbarBG;
    float mHPdiscardRange;

    // レーダー用のテクスチャ
    class Texture* mRader;
    class Texture* mBlipTex;
    // レーダー中心から輝点への2D相対オフセット
    std::vector<Vector2> mBlips;
    // レーダーの(ワールド空間での)範囲とレーダーディスプレイの半径
    float mRaderRange;
    float mRaderRadius;

    // タイマー用
    float mTimeFloat;

    // Level
    class Texture* mLevel;

    // Phase
    class Texture* mPhase;
    int mCurPhaseNum;

    std::vector<class TargetComponent*> mTargetComps;
};

