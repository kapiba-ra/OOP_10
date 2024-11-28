#pragma once
#include <GL/glew.h>
#include <string>
#include "Math.h"

class Shader
{
public:
	Shader();
	~Shader();

	// 指定された名前の頂点シェーダーとフラグメントシェーダーを読み込む
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// アクティブなシェーダープログラムとして設定
	void SetActive();

	void SetMatrixUniform(const char* name, const Matrix4& matrix);
	void SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned int count);
	void SetVectorUniform(const char* name, const Vector3& vector);
	void SetFloatUniform(const char* name, float value);

private:
	// シェーダーのコンパイル
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);
	// シェーダーのコンパイルに成功したかの判定
	bool IsCompiled(GLuint shader);
	// 頂点・フラグメントプログラムのリンクを確認
	bool IsValidProgram();
	// シェーダーオブジェクトのIDを格納,IDを元にアクセスする
	GLuint mVertexShader;		// 頂点シェーダーのID
	GLuint mFragShader;			// フラグメントシェーダーのID
	GLuint mShaderProgram;		// リンクされたシェーダープログラムのID
};

