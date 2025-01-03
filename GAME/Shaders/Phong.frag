#version 330

// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;
// Normal (in world space)
in vec3 fragNormal;
// Position (in world space)
in vec3 fragWorldPos;

out vec4 outColor;

uniform sampler2D uTexture;

// 平行光源の構造体
struct DirectionalLight
{
	// 光の方向
	vec3 mDirection;
	// 拡散反射色
	vec3 mDiffuseColor;
	// 鏡面反射色
	vec3 mSpecColor;
};

// ライティング用のuniform
// カメラの位置
uniform vec3 uCameraPos;
// 環境光の強さ
uniform vec3 uAmbientLight;
// 表面の鏡面反射指数(光沢を表現)
uniform float uSpecPower;
// 平行光源
uniform DirectionalLight uDirLight;

void main()
{
	// 表面法線 N
	vec3 N = normalize(fragNormal);
	// 表面から光源へのベクトル L
	vec3 L = normalize(-uDirLight.mDirection);
	// 表面からカメラへのベクトル V
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// Nに関する　-L の反射 R
	vec3 R = normalize(reflect(-L, N));

	// フォンの反射を計算
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

	// 最終的な色はテクスチャの色×フォンの光 (alpha=1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
