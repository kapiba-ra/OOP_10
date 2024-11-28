#pragma once
#include <GL/glew.h>
#include <string>
#include "Math.h"

class Shader
{
public:
	Shader();
	~Shader();

	// �w�肳�ꂽ���O�̒��_�V�F�[�_�[�ƃt���O�����g�V�F�[�_�[��ǂݍ���
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// �A�N�e�B�u�ȃV�F�[�_�[�v���O�����Ƃ��Đݒ�
	void SetActive();

	void SetMatrixUniform(const char* name, const Matrix4& matrix);
	void SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned int count);
	void SetVectorUniform(const char* name, const Vector3& vector);
	void SetFloatUniform(const char* name, float value);

private:
	// �V�F�[�_�[�̃R���p�C��
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);
	// �V�F�[�_�[�̃R���p�C���ɐ����������̔���
	bool IsCompiled(GLuint shader);
	// ���_�E�t���O�����g�v���O�����̃����N���m�F
	bool IsValidProgram();
	// �V�F�[�_�[�I�u�W�F�N�g��ID���i�[,ID�����ɃA�N�Z�X����
	GLuint mVertexShader;		// ���_�V�F�[�_�[��ID
	GLuint mFragShader;			// �t���O�����g�V�F�[�_�[��ID
	GLuint mShaderProgram;		// �����N���ꂽ�V�F�[�_�[�v���O������ID
};

