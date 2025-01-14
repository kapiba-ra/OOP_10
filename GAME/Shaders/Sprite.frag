#version 330

// ���_�V�F�[�_�[����̃e�N�X�`�����W�̓���
in vec2 fragTexCoord;

// �o�͐F
out vec4 outColor;

// �e�N�X�`���T���v�����O�p
uniform sampler2D uTexture;

// �`������O���邽�߂͈̔�(x������)
uniform float uDiscardRange; // 0.0����1.0�͈̔�

// ���l
uniform float uAlpha;

void main()
{
	// �w�肳�ꂽ�͈͂�discard
    if (fragTexCoord.x > uDiscardRange)
    {
        discard; // ���̃t���O�����g��`�悵�Ȃ�
    }

	// �e�N�X�`������F���T���v�����O����(���l�𒲐�)
    outColor = texture(uTexture, fragTexCoord) * vec4(1.0, 1.0, 1.0, uAlpha);
}