#pragma once
#include "Math.h"

// �{�[���ő吔
const size_t MAX_SKELETON_BONES = 96;
// ����{�[���̋t�o�C���h�|�[�Y�s���,�A�j���[�V�������̂���|�[�Y�ɂ�����
// �{�[���̃I�u�W�F�N�g��Ԃł̈ʒu�E��]�ւ̕ϊ��s��̊|���Z�̌��ʂ́A
// �A�j���[�V�����t���[���P�ʂł͕ω����Ȃ��B���������Čv�Z�ʂ����炷���߂�
// ����struct�Ɋ|���Z�̌��ʂ��L�^���A�e���_���ȒP�ɕϊ��ł���悤�ɂ���
// �|���Z�̌��ʂ̓X�L�j���O�Ɏg����
struct MatrixPalette
{
	Matrix4 mEntry[MAX_SKELETON_BONES];
};
