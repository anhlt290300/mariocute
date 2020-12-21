#include "Items.h"
#include "Brick.h"
#include "Utils.h"
#define GRAVITY 0.3f
#define Item_move 62 // quang duong item di chuyen khi troi len
CItems::CItems(float start_x, float start_y)
{
	SetPosition(start_x, start_y);
	this->Start_x = start_x;
	this->Start_y = start_y;
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(5));
}

void CItems::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	
	if (!Isdone)
	{
		left = x;
		top = y;
		right = x + 18;
		bottom = y + 18;
	}
}
void CItems::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (Start_y == 0)
	{
		Start_y = y;
	}
	if (ID_brickkk == 1||ID_brickkk ==3)
	{
	
		if (y < Start_y - 45)
			vy = -20 ;
	}

	if (ID_brickkk == 2)
	{
		
		if (y < Start_y - 30)
			vy = 0.28;
	}





	CGameObject::Update(dt);
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
		//x += dx;
		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block every object first!
		/*if (ny != 0)
		{
			vy = 0;
		}*/

		y += min_ty * dy + ny * 0.5f;
		x += min_tx * dx + nx * 0.5f;

		// += dx;
		/*if (coEvents.size() <= 2)
		{
			x += dx;
		}
		else
			change_direction *= -1;*/
	/*	if (nx != 0) change_direction *= -1;*/

		/*if (ny==0 && nx!=0)
		{
			doihuong *= -1;
			x += nx * 2;
		}*/



		//Collision logic with other objects

   	for (UINT i = 0; i < coEventsResult.size(); i++)
	   {
		   LPCOLLISIONEVENT e = coEventsResult[i];
		   if (dynamic_cast<CBrick*>(e->obj))
		   {
			   if (e->ny < 0)
			   {
				   if (leftorright)
					   vx = 0.1;
				   else
					   vx = -0.1;
			   }
				   
		   }

	   }
	}

	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}
void CItems::Render()
{
	DebugOut(L"state %d\n", state);
	
	if (ID_brickkk == 1||ID_brickkk ==3)
		ani =Coin;
	else if (ID_brickkk == 2)
	{
		if (level_mario == 1)
			ani = Mushroom;
		else if (level_mario == 2)
			ani = Tree_Leaf;
		else
			ani = flower;
	}
	if (!Isdone)
		animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void CItems::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case Mushroom:
		
		break;
	case Tree_Leaf:
	
		break;
	case Coin:

		break;
	case flower:
		break;
	}
}