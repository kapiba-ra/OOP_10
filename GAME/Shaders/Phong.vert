#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// ���� 0 : �ʒu, 1 : �@��, 2 : �e�N�X�`�����W.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;
// �@�� (���[���h���)
out vec3 fragNormal;
// �ʒu (���[���h���)
out vec3 fragWorldPos;

void main()
{
	// �ʒu�𓯎����W�n�ɕϊ�����
	vec4 pos = vec4(inPosition, 1.0);
	// �ʒu�����[���h��Ԃɕϊ�����
	pos = pos * uWorldTransform;
	// ���[���h��Ԃ̈ʒu��ۑ�
	fragWorldPos = pos.xyz;
	// �N���b�v��Ԃɕϊ�����
	gl_Position = pos * uViewProj;

	// �@�������[���h��Ԃɕϊ�(w=0)
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	// �e�N�X�`�����W���t���O�����g�V�F�[�_�[�֓n��
	fragTexCoord = inTexCoord;
}

// uViewProj�͂���Ȋ����ɂȂ�(����p��70��ݒ肵�Ă���)
// { 0,	     0,     1.0025,	  0 }
// { 0.273,  0, 	0, 		  0 }
// { 0, 	 0.364, 0, 		  1 }
// { 0, 	 0, 	?25.0625, 0 }