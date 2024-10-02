#version 330

// 頂点シェーダーからのテクスチャ座標の入力
in vec2 fragTexCoord;

// 出力色
out vec4 outColor;

// テクスチャサンプリング用
uniform sampler2D uTexture;

void main()
{
	// テクスチャから色をサンプリングする
    outColor = texture(uTexture, fragTexCoord);
}
