#include "MonneyEffect.h"


CMonneyEffect::CMonneyEffect()
{

	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(9));

}
void CMonneyEffect::Update(ULONGLONG dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
		
		x += dx;
		y += dy;
	
}

void CMonneyEffect::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{

}

void CMonneyEffect::Render()
{
		animation_set->at(state)->Render(x, y);
}

void CMonneyEffect::SetState(int State)
{
	CGameObject::SetState(State);
	switch (State)
	{
	case MAKE_100:
		break;
	case MAKE_200:
		break;
	case MAKE_400:
		break;
	case MAKE_800:
		break;
	case MAKE_1000:
		break;
	case MAKE_2000:
		break;
	case MAKE_4000:
		break;
	case MAKE_8000:
		break;
	}
}
