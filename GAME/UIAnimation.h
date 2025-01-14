#pragma once

#include "Math.h"

/// <summary>
/// UIのアニメーションに関連する計算をするクラス.
/// あまり活用できていない(フェードアウトとズームアウトが簡単にできちゃった為)。
/// </summary>
class UIAnimation
{
public:
    enum class AnimState
    {
        EPlaying,   // アニメーション進行中
        EEnded      // アニメーション終了
    };

    UIAnimation(AnimState animState);
    virtual ~UIAnimation();

    virtual void Update(float deltaTime) = 0; // 更新処理

    void SetState(AnimState state) { mAnimState = state; }
    AnimState GetState() const { return mAnimState; }

protected:
    AnimState mAnimState;
};

/// <summary>
/// スライドインアウトのロジックを担当,コンストラクタで関連するパラメータを設定する
/// (元々はスライドする座標の参照を受け取っていたが,依存関係が出て困ったのでGetPositionで
/// 都度返すようにした)
/// 追記:UnityのSliderとは異なる機能
/// </summary>
class Slider :
    public UIAnimation
{
public:
    Slider(float endX, float speed, Vector2 initialPos, 
        UIAnimation::AnimState animState = AnimState::EPlaying);
    ~Slider();

    // 他のクラスのUpdateと違い、Gameクラスで勝手に呼ばれないので
    // 自分で呼び出す必要がある事に注意。
    void Update(float deltaTime) override;
    const Vector2& GetPosition() const { return mCurrentPos; }

private:
    float mEndX;            // スライド終了位置
    float mSpeed;           // スライド速度
    Vector2 mCurrentPos;    // 変更する位置の現在位置
};