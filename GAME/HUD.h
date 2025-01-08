#pragma once
#include "UIScreen.h"
#include <vector> 

/// <summary>
/// HUD(ヘッドアップディスプレイ),ゲーム内で画面に重ねて表示される情報のこと
/// HUDの表示を担当するクラス
/// </summary>
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

    /* HP */
    class Texture* mHPbarGreen; // 緑のHPbar
    class Texture* mHPbarRed;   // 赤のHPbar
    class Texture* mHPbarBG;    // HPbarの背景
    float mHPdiscardRange;      // Hpbarの描画範囲を決定(0-1)
    /* レーダー */
    class Texture* mRadar;          // レーダーDisplayの丸い円
    class Texture* mPlayerInRadar;  // レーダー中心のプレイヤー
    class Texture* mBlipTex;        // 敵の位置を示す
    std::vector<Vector2> mBlips;    // レーダー中心から輝点への2D相対オフセット
    float mRadarRange;      // ワールド空間でのレーダー範囲
    float mRadarRadius;     // RaderDisplayでのレーダー半径
    std::vector<class TargetComponent*> mTargetComps;   // 敵の位置を取得できる
    /* スキルのレベル */
    std::vector<class Texture*> mIcons; // 獲得したスキルのアイコン
    class Texture* mIconFrame;       // アイコンのフレーム

    class Texture* mLevel;  // 「Lv.」 のテキストテクスチャ
    class Texture* mPhase;  // 「Phase」のテキストテクスチャ
    int mCurPhaseNum;       // 現在のフェーズ
    float mTimeFloat;       // Phase開始からの時間を記録する

    /* 表示位置系,Updateで使う日に備えてメンバにしておいた(基本はDrawで使用) */
    class Vector2 mHpbarPos;    // Hpbarの表示位置
    class Vector2 mHpNumPos;    // Hp(数字)の表示位置
    class Vector2 mRadarPos;    // Radarの表示位置
    class Vector2 mSkillPos;    // Skillの表示位置
    class Vector2 mLevelPos;    // 「Lv.」表示位置
    class Vector2 mPhasePos;    // 「Phase」表示位置
    class Vector2 mTimePos;     // Phase経過時間のテクスチャ位置
};

