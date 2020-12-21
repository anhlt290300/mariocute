#pragma once
#include "GameObject.h"
#include "Items.h"
#include "Mario.h"
#define BRICK_STATE_IDLE 0
#define BRICK_STATE_HIT 1


#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16

class CBrick : public CGameObject
{
public:
	vector <LPGAMEOBJECT> listitems;
	vector<LPGAMEOBJECT> objects;
	CMario* mario;
	bool block = false;
	bool vacham=false;
	float y_start=0;
	int levelmario;
	int ID_brick;
	int width, height;
	bool leftorright;
	CBrick(CMario* a,int ID);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void SetState(int state);
};