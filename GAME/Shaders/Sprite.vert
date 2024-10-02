#version 330

// ���[���h���W�ƃr���[�ˉe�̂��߂�uniform�s��
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 : �ʒu���W, 1 : �@���x�N�g��, 2 : �e�N�X�`�����W.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// �e�N�X�`�����W���o�͂ɒǉ�
out vec2 fragTexCoord;

void main()
{
	// �ʒu���W���瓯�����W�֕ϊ�
	vec4 pos = vec4(inPosition, 1.0);
	
	// �ʒu�����[���h��Ԃɕϊ����Ă���N���b�v��Ԃɕϊ�����
	gl_Position = pos * uWorldTransform * uViewProj;

	// �e�N�X�`�����W���t���O�����g�V�F�[�_�[�ɓn��
	fragTexCoord = inTexCoord;
}

// uViewProj�͈ȉ��̂悤�ɐݒ肳���͂��B(1024*768��z��)
// { 512.0f, 0.0f, 0.0f, 0.0f },
// { 0.0f, 382.0f, 0.0f, 0.0f },
// { 0.0f, 0.0f, 1.0f, 0.0f },
// { 0.0f, 0.0f, 1.0f, 1.0f }