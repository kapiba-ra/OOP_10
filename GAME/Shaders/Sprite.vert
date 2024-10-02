#version 330

// ワールド座標とビュー射影のためのuniform行列
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 : 位置座標, 1 : 法線ベクトル, 2 : テクスチャ座標.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// テクスチャ座標を出力に追加
out vec2 fragTexCoord;

void main()
{
	// 位置座標から同次座標へ変換
	vec4 pos = vec4(inPosition, 1.0);
	
	// 位置をワールド空間に変換してからクリップ空間に変換する
	gl_Position = pos * uWorldTransform * uViewProj;

	// テクスチャ座標をフラグメントシェーダーに渡す
	fragTexCoord = inTexCoord;
}

// uViewProjは以下のように設定されるはず。(1024*768を想定)
// { 512.0f, 0.0f, 0.0f, 0.0f },
// { 0.0f, 382.0f, 0.0f, 0.0f },
// { 0.0f, 0.0f, 1.0f, 0.0f },
// { 0.0f, 0.0f, 1.0f, 1.0f }