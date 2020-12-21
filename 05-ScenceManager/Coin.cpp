#include "Coin.h"
#include "MonneyEffect.h"

CCoin::CCoin()
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(8));
	
}
void CCoin::Update(ULONGLONG dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);
	y += dy;

	

	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Update(dt, coObjects);
	}


}

void CCoin::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (!Isdone)
	{
		left = x;
		top = y;
		right = x + 2;
		bottom = y + 2;
	}
}

void CCoin::Render()
{
	if (!Isdone)
		animation_set->at(1)->Render(x, y);
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Render();
	}
	RenderBoundingBox();
}



