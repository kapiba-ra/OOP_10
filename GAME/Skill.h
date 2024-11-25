#pragma once
#include <string>
#include <functional>

/// <summary>
/// Player�����e�X�L���Ɋւ���N���X
/// Skill��enum�ϐ�Type���������A���킩�p�����[�^�֘A���ŏ����𕪂�����@�������Ă݂���
/// </summary>
class Skill
{
public:
	Skill(const std::string& name, std::function<void(class PlayerActor*, int)> effect);
	~Skill();

	void OnLevelUp(class PlayerActor* player);

	bool IsLevelMax() const { return mCurLv == mMaxLv; }

	std::string GetName() const { return mName; }
	int GetCurLv() const { return mCurLv; }
	int GetMaxLv() const { return mMaxLv; }
	void SetCurLv(int lv) { mCurLv = lv; }
	void SetMaxLv(int lv) { mMaxLv = lv; }

private:
	std::string mName;	// �X�L����
	std::function<void(class PlayerActor*, int)> mEffect;	// LevelUp���̌���.int��mCurLv��z��
	int mMaxLv;
	int mCurLv;
};

