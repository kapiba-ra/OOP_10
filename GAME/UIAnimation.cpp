#include "UIAnimation.h"

UIAnimation::UIAnimation(AnimState animState)
	: mAnimState(animState)
{
}

UIAnimation::~UIAnimation()
{
}

Slider::Slider(float endX, float speed, Vector2 initialPos, AnimState animState)
	: UIAnimation(animState)
	, mEndX(endX)
	, mSpeed(speed)
	, mCurrentPos(initialPos)
{
}

Slider::~Slider()
{
}

void Slider::Update(float deltaTime)
{
	if (mAnimState == AnimState::EEnded)
	{
		return;
	}
	else if (mAnimState == AnimState::EPlaying)
	{
		float moveAmount = mSpeed * deltaTime;
		mCurrentPos.x += moveAmount;
		// s‚«‰ß‚¬‚Ä‚©‚ç–ß‚é‚±‚Æ‚ª‚È‚¢‚æ‚¤‚É,moveAmount‚ð·‚µˆø‚¢‚Ä‚¢‚é
		if (mCurrentPos.x >= mEndX - moveAmount)
		{
			mCurrentPos.x = mEndX;
			mAnimState = AnimState::EEnded;
		}
	}
}
