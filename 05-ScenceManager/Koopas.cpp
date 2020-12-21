#include "Koopas.h"
#include "Floor.h"
#include "Goomba.h"
#include "Colorbox.h"
#include "Mario.h"
CKoopas::CKoopas(CMario *mario)
{
	this->mario = mario;
	SetState(KOOPAS_STATE_DIE);
}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;


	if (state == KOOPAS_STATE_DIE_FLY || state == KOOPAS_STATE_DIE)
	{
		right = x + KOOPAS_BBOX_WIDTH - 1;
		bottom = y + KOOPAS_BBOX_HEIGHT_DIE +1 ;
	}
	else if (state == KOOPAS_STATE_DIE_BUT_RUN)
	{
		right = x + KOOPAS_BBOX_WIDTH - 1;
		bottom = y + KOOPAS_BBOX_HEIGHT_DIE - 1;
	}
	else if (state == KOOPAS_STATE_WAIT_TO_LIFE_1 || state == KOOPAS_STATE_WAIT_TO_LIFE_2)
	{
		right = x + KOOPAS_BBOX_WIDTH - 1;
		bottom = y + KOOPAS_BBOX_HEIGHT_DIE;
	}
	else
	{
		
			right = x + KOOPAS_BBOX_WIDTH;
			bottom = y + KOOPAS_BBOX_HEIGHT;
		
	}
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt, coObjects);
	vy += MARIO_GRAVITY * dt;

	lastbring = mario->IsBring;
	laststate = state;
	

	if (mario->IsBring)
	{
		vy = 0;
		if (mario->nx > 0)
		{
			x = mario->x +20;
			y = mario->y +5;
		}
		else
		{
			x = mario->x -15;
			y = mario->y +5;
		}
	}
	if (lastbring) 
	{
		nx = mario->nx;
		SetState(KOOPAS_STATE_DIE);
	}
	if (lastbring && GetTickCount() - timestartdie > 1000)
	{
		timestartdie = GetTickCount();
		nx = mario->nx;
	}

	
	if (state == KOOPAS_STATE_WAIT_TO_LIFE_2 && GetTickCount() - timestartdie > 5000&&!mario->IsBring)
	{
		vy = -0.15;
		SetState(KOOPAS_STATE_WALKING);
	}
	if (state == KOOPAS_STATE_DIE && GetTickCount() - timestartdie > 2500 && !mario->IsBring)
	{
		SetState(KOOPAS_STATE_WAIT_TO_LIFE_1);
	}
	if (state == KOOPAS_STATE_WAIT_TO_LIFE_1 && GetTickCount() - timestartdie > 3500 && !mario->IsBring)
	{
		SetState(KOOPAS_STATE_WAIT_TO_LIFE_2);
	}
	
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		//chamsan = true;
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;
		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block every object first!


		y += min_ty * dy + ny * 0.5f;
		//x+= min_tx * dx + nx * 0.5f;
		
		if (ny != 0)
		{
			vy = 0;
		}

		//Collision logic with other objects

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{

			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CFloor*>(e->obj))
			{
				CFloor* floor = dynamic_cast<CFloor*>(e->obj);
				if (e->nx != 0)
					vx = -vx;
			}
			else if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (e->nx != 0)
				{
					if (goomba->GetState() == GOOMBA_STATE_WALKING && GetState()==KOOPAS_STATE_DIE_BUT_RUN)
						goomba->SetState(GOOMBA_STATE_DIE_FLY);
				}
			}
			else if (dynamic_cast<CColorbox*>(e->obj))
			{
				CColorbox* Colorbox = dynamic_cast<CColorbox*>(e->obj);
				if (e->nx != 0)
				{
					
					x += dx ;

				}

			}
		}
	}

}

void CKoopas::Render()
{
	int ani = KOOPAS_ANI_WALKING_LEFT;

	if (state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_FLY)
	{
		if (upordown)
			ani = KOOPAS_ANI_DIE_DOWN;
		else
			ani = KOOPAS_ANI_DIE_UP;
	}
	else if (state == KOOPAS_STATE_DIE_BUT_RUN)
	{
		if (upordown)
			ani = KOOPAS_ANI_MOVE_DOWN;
		else
			ani = KOOPAS_ANI_MOVE_UP;
	}
	else if (state == KOOPAS_STATE_WALKING)
	{
	if (vx > 0)
		ani = KOOPAS_ANI_WALKING_RIGHT;
	else   ani = KOOPAS_ANI_WALKING_LEFT;
	}
	else if (state ==KOOPAS_STATE_WAIT_TO_LIFE_1)
		ani = KOOPAS_ANI_WAIT_LIVE_1;

	else if (state ==KOOPAS_STATE_WAIT_TO_LIFE_2)
		ani = KOOPAS_ANI_WAIT_LIVE_2;
	animation_set->at(ani)->Render(x, y);

	/*RenderBoundingBox();*/
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPAS_STATE_DIE:
		/*y += KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_DIE + 1;*/
		
		vx = 0;
		vy = 0;
		y -= 2;
		break;
	case KOOPAS_STATE_WALKING:
		upordown = true;
		vx = KOOPAS_WALKING_SPEED;
		break;
	case KOOPAS_STATE_DIE_BUT_RUN:
		if (nx == 1)
			vx = 0.1;
		else
			vx = -0.1;
		
		break;
	case KOOPAS_STATE_DIE_FLY:	
		vx = 0;
		vy =-0.2 ;
		break;
	case KOOPAS_STATE_WAIT_TO_LIFE_1:
		vx = 0;
		vy = 0;
		break;
	case KOOPAS_STATE_WAIT_TO_LIFE_2:
		vx = 0;
		vy = 0;
		break;
	}

}