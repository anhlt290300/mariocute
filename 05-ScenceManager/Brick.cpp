#include "Brick.h"
#include "Utils.h"

CBrick::CBrick(CMario *a,int ID)
{
	this->mario = a;
	this->ID_brick = ID;
}
void CBrick::Render()
{
	animation_set->at(ani)->Render(x, y);
	if (state == BRICK_STATE_IDLE)
		ani = 0;
	else if(state==BRICK_STATE_HIT)
		ani = 1;
	for (int i = 0; i < listitems.size(); i++)
		listitems[i]->Render();
	RenderBoundingBox();
}
void CBrick::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt, coObjects);
	DebugOut(L"\nSize Item: %f", vy);

	
	if (y_start == 0)
		y_start = y;
	if (y < y_start - 10)
		vy = 0.08;
	
	if (y > y_start)
	{
		y = y_start;
		vy = 0;
	}
	

	x += dx;
	y += dy;


		
		

}
void CBrick::GetBoundingBox(float &l, float &t, float &r, float &b)
{
	l = x;
	t = y_start;
	r = l + BRICK_BBOX_WIDTH;
	b = t + BRICK_BBOX_HEIGHT;
}


void CBrick::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BRICK_STATE_IDLE:
	
		break;
	case BRICK_STATE_HIT:
		
		break;
	
	}
}