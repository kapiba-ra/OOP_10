#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// 属性 0 : 位置, 1 : 法線, 2 : テクスチャ座標.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;
// 法線 (ワールド空間)
out vec3 fragNormal;
// 位置 (ワールド空間)
out vec3 fragWorldPos;

void main()
{
	// 位置を同次座標系に変換する
	vec4 pos = vec4(inPosition, 1.0);
	// 位置をワールド空間に変換する
	pos = pos * uWorldTransform;
	// ワールド空間の位置を保存
	fragWorldPos = pos.xyz;
	// クリップ空間に変換する,座標系もNDCになる(gl_Positionはグローバル変数)
	// ただしzは0-1に収めている(0がnear,1がfar)
	gl_Position = pos * uViewProj;

	// 法線をワールド空間に変換(w=0)
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	// テクスチャ座標をフラグメントシェーダーへ渡す
	fragTexCoord = inTexCoord;
}

// uViewProjはこんな感じになる(視野角に70を設定している,誤差もある)
// { 0,    0,     1.00125, 1 }
// { 1.07, 0,     0, 	   0 }
// { 0,    1.427, 0,       0 }
// { 0,    0,     ?25.03,  0 }
// uWorldTransform(ゲーム開始時左後方の床の場合)
// { 10,	0,		0,   0 }
// { 0,		10,		0,   0 }
// { 0,		0,		10,  0 }
// { -1250, -1250, -100, 1 }
