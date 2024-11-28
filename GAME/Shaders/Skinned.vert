#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
// Uniform for matrix palette
uniform mat4 uMatrixPalette[96];

layout(location = 0) in vec3 inPosition;	// 位置
layout(location = 1) in vec3 inNormal;		// 法線
layout(location = 2) in uvec4 inSkinBones;	// 頂点に影響を及ぼすボーンのインデックス(4つまで)
layout(location = 3) in vec4 inSkinWeights; // 法線
layout(location = 4) in vec2 inTexCoord;	// テクスチャ

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
	
	// 位置のスキニング
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	
	// 位置をワールド空間に変換する
	skinnedPos = skinnedPos * uWorldTransform;
	// ワールド空間での位置を保存
	fragWorldPos = skinnedPos.xyz;
	// クリップ空間に変換する,座標系もNDCになる(gl_Positionはグローバル変数)
	// ただしzは0-1に収めている(0がnear,1がfar)
	gl_Position = skinnedPos * uViewProj;

	// 頂点法線のスキニング
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
		+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
		+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
		+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// 法線をワールド空間に変換(w=0)
	fragNormal = (skinnedNormal * uWorldTransform).xyz;

	// テクスチャ座標をフラグメントシェーダーへ渡す
	fragTexCoord = inTexCoord;
}
