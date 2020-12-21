#include "Goomba.h"
#include "Colorbox.h"
#include "Koopas.h"
#include "Floor.h"
CGoomba::CGoomba()
{
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else 	
		bottom = y + GOOMBA_BBOX_HEIGHT;
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt, coObjects);

	

	x += dx;
	y += dy;

	
	if (state == GOOMBA_STATE_DIE && GetTickCount() - timestartdie > 800)
		SetState(GOOMBA_STATE_LOST);


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
			
			else if (dynamic_cast<CColorbox*>(e->obj))
			{
				CColorbox* Colorbox = dynamic_cast<CColorbox*>(e->obj);
				if (e->nx != 0)
				{

					x += dx;

				}

			}
		}
	}
}

void CGoomba::Render()
{
	if (state == GOOMBA_STATE_DIE || state == GOOMBA_STATE_LOST)
		ani = GOOMBA_ANI_DIE;
	else
		ani = GOOMBA_ANI_WALKING;
	

	animation_set->at(ani)->Render(x,y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case GOOMBA_STATE_DIE:
			
			y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
			vx = 0;
			vy = 0;
			break;
		case GOOMBA_STATE_LOST:
			y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
			vx = 0;
			vy = 5;
			break;
		case GOOMBA_STATE_WALKING: 
			vx = GOOMBA_WALKING_SPEED;
			break;
		case GOOMBA_STATE_DIE_FLY:
			vx = -GOOMBA_WALKING_SPEED + 0.04f;
			vy = 0.15;
			break;
	}
}
