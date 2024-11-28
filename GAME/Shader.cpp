#include "Shader.h"
#include <fstream>
#include <sstream>
#include <SDL/SDL.h>
#include "Texture.h"

Shader::Shader()
    : mShaderProgram(0)
    , mVertexShader(0)
    , mFragShader(0)
{
}

Shader::~Shader()
{
}

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
    if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
        !CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
    {
        return false;
    }
    // ���_�V�F�[�_�[�ƃt���O�����g�V�F�[�_�[�������N����
    // �V�F�[�_�[�v���O�������쐬
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragShader);
    glLinkProgram(mShaderProgram);
    if (!IsValidProgram())
    {
        return false;
    }
    return true;
}

void Shader::Unload()
{
    glDeleteProgram(mShaderProgram);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragShader);
}

void Shader::SetActive()
{
    glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
    // Search uniform "name"(name is given as an argument)
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    // Send matrix data to uniform
    glUniformMatrix4fv(
        loc,        // Uniform ID
        1,          // Number of matrices to send to uniform
        GL_TRUE,    // Use row vector
        matrix.GetAsFloatPtr()  // Pointer to matrix data
    );
}

void Shader::SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned int count)
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    // Send the matrix data to the uniform
    glUniformMatrix4fv(loc, count, GL_TRUE, matrices->GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector3& vector)
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform3fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const char* name, float value)
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform1f(loc, value);
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
    // �t�@�C�����J��
    std::ifstream shaderFile(fileName);
    if (shaderFile.is_open())
    {
        // �S�Ẵe�L�X�g���P�̕�����ɓǂݍ���
        std::stringstream sstream;
        sstream << shaderFile.rdbuf();
        std::string contents = sstream.str();
        const char* contentsChar = contents.c_str();

        // �w�肳�ꂽ�^�C�v(���_or�t���O�����g)�̃V�F�[�_�[���쐬
        outShader = glCreateShader(shaderType);
        // �ǂݍ��񂾕�����ł̃R���p�C�������݂�
        glShaderSource(outShader, 1, &contentsChar, nullptr);
        glCompileShader(outShader);

        if (!IsCompiled(outShader))
        {
            SDL_Log("Failded to compile %s", fileName.c_str());
            return false;
        }
    }
    else
    {
        SDL_Log("Failed to find  %s", fileName.c_str());
        return false;
    }
    return true;
}

bool Shader::IsCompiled(GLuint shader)
{
    GLint status;
    // �R���p�C����Ԃ�₢���킹��
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetShaderInfoLog(shader, 511, nullptr, buffer);
        SDL_Log("Failed to compile GLSL : \n%s", buffer);
        return false;
    }
    return true;
}

bool Shader::IsValidProgram()
{
    GLint status;
    // �����N��Ԃ�₢���킹��
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
        SDL_Log("Failed to link GLSL : \n%s", buffer);
        return false;
    }
    return true;
}
