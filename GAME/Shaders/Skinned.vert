#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
// Uniform for matrix palette
uniform mat4 uMatrixPalette[96];

layout(location = 0) in vec3 inPosition;	// �ʒu
layout(location = 1) in vec3 inNormal;		// �@��
layout(location = 2) in uvec4 inSkinBones;	// ���_�ɉe�����y�ڂ��{�[���̃C���f�b�N�X(4�܂�)
layout(location = 3) in vec4 inSkinWeights; // �@��
layout(location = 4) in vec2 inTexCoord;	// �e�N�X�`��

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
	
	// �ʒu�̃X�L�j���O
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	
	// �ʒu�����[���h��Ԃɕϊ�����
	skinnedPos = skinnedPos * uWorldTransform;
	// ���[���h��Ԃł̈ʒu��ۑ�
	fragWorldPos = skinnedPos.xyz;
	// �N���b�v��Ԃɕϊ�����,���W�n��NDC�ɂȂ�(gl_Position�̓O���[�o���ϐ�)
	// ������z��0-1�Ɏ��߂Ă���(0��near,1��far)
	gl_Position = skinnedPos * uViewProj;

	// ���_�@���̃X�L�j���O
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
		+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
		+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
		+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// �@�������[���h��Ԃɕϊ�(w=0)
	fragNormal = (skinnedNormal * uWorldTransform).xyz;

	// �e�N�X�`�����W���t���O�����g�V�F�[�_�[�֓n��
	fragTexCoord = inTexCoord;
}
