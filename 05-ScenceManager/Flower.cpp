#include "Flower.h"
#include "Utils.h"
#include "Mario.h"


CFlower::CFlower(CMario* mario)
{
	this->mario = mario;
	
}

void CFlower::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	y += dy;
	if (y > 380 && ny > 0)
	{
		moveup = true;
		movedown = false;
		ny = -1;
	}
	if (y < 335 && ny < 0)
	{
		moveup = false;
		ny = 1;
		vy = 0;
		timewaittoshoot = GetTickCount();
		if (!leftorright)
		{
			if (!upordown)
				SetState(FLOWER_ANI_FIRE_LEFT_60);
			else
				SetState(FLOWER_ANI_FIRE_LEFT_120);
		}
		else if (leftorright)
		{
			if (!upordown)
				SetState(FLOWER_ANI_FIRE_RIGHT_60);
			else
				SetState(FLOWER_ANI_FIRE_RIGHT_120);
		}
		CFireball* fireball = new CFireball({ x,y }, 1);
		if (!leftorright)
			fireball->nx = -1;
		else
			fireball->nx = 1;
		if (y - GIANT_BOX_HEIGHT - mario->y < 0)
			fireball->isbottom = true;
		else
			fireball->istop = true;
		
		listFireBall.push_back(fireball);


	}
	for (int i = 0; i < listFireBall.size(); i++)
		listFireBall[i]->Update(dt,coObjects);

	if (y > 335)
		timewaittoshoot = 0;
	if (GetTickCount() - timewaittoshoot > 1500&&timewaittoshoot!=0)
		movedown = true;

	if (moveup)
	{
		vy = -a;
		
		if (!leftorright)
		{
			if (upordown)
				SetState(FLOWER_ANI_UP_BUT_DOWN_LEFT);
			else
				SetState(FLOWER_ANI_UP_BUT_UP_LEFT);
		}
		else if (leftorright)
		{
			if (upordown)
				SetState(FLOWER_ANI_UP_BUT_DOWN_RIGHT);
			else
				SetState(FLOWER_ANI_UP_BUT_UP_RIGHT);
		}
	}
	if (movedown)
	{
		vy = a;

		if (!leftorright)
		{
			if (upordown)
				SetState(FLOWER_ANI_UP_BUT_DOWN_LEFT);
			else
				SetState(FLOWER_ANI_UP_BUT_UP_LEFT);
		}
		else if (leftorright)
		{
			if (upordown)
				SetState(FLOWER_ANI_UP_BUT_DOWN_RIGHT);
			else
				SetState(FLOWER_ANI_UP_BUT_UP_RIGHT);
		}
	}








	
	if (mario->y - y > 0)
		upordown = true;
	else
		upordown = false;
	if (mario->x - x > 0)
		leftorright = true;
	else
		leftorright = false;


	
}

void CFlower::Render()
{
	animation_set->at(state)->Render(x, y);
	for (int i = 0; i < listFireBall.size(); i++)
		listFireBall[i]->Render();
}
void CFlower::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FLOWER_ANI_FIRE_LEFT_120:
		break;
	case FLOWER_ANI_FIRE_LEFT_60:
		break;
	case FLOWER_ANI_FIRE_RIGHT_120:
		break;
	case FLOWER_ANI_FIRE_RIGHT_60:
		break;
	case FLOWER_ANI_UP_BUT_DOWN_LEFT:
		break;
	case FLOWER_ANI_UP_BUT_DOWN_RIGHT:
		break;
	case FLOWER_ANI_UP_BUT_UP_LEFT:
		break;
	case FLOWER_ANI_UP_BUT_UP_RIGHT:
		break;
	}	
}


void CFlower::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{

	left = x;
	top = y;
	right = x + GIANT_BOX_WIDTH;
	bottom = y + GIANT_BOX_HEIGHT;

}
