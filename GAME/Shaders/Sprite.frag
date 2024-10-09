#version 330

// 頂点シェーダーからのテクスチャ座標の入力
in vec2 fragTexCoord;

// 出力色
out vec4 outColor;

// テクスチャサンプリング用
uniform sampler2D uTexture;

// 描画を除外するための範囲
uniform float uDiscardRange; // 0.0から1.0の範囲

void main()
{
	// 指定された範囲でdiscard
    if (fragTexCoord.x > uDiscardRange) {
        discard; // このフラグメントを描画しない
    }

	// テクスチャから色をサンプリングする
    outColor = texture(uTexture, fragTexCoord);
}