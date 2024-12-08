#pragma once
#include "Math.h"

// ボーン最大数
const size_t MAX_SKELETON_BONES = 96;
// あるボーンの逆バインドポーズ行列と,アニメーション中のあるポーズにおける
// ボーンのオブジェクト空間での位置・回転への変換行列の掛け算の結果は、
// アニメーションフレーム単位では変化しない。したがって計算量を減らすために
// このstructに掛け算の結果を記録し、各頂点を簡単に変換できるようにする
// 掛け算の結果はスキニングに使われる
struct MatrixPalette
{
	Matrix4 mEntry[MAX_SKELETON_BONES];
};
