
#pragma once
#include "GameObject.h"
#include "Brick.h"
#define Mushroom	1
#define	Tree_Leaf	2
#define Coin		0
#define flower		3

#define NAM_DUNG_YEN 0
#define NAM_DI_CHUYEN 0.05f
class CItems : public CGameObject
{


	bool spawn = true;
public:
	bool leftorright;
	int change_direction = 1;
	float Start_y=0;
	float Start_x;
	bool move;
	int level_mario;
	int ID_brickkk;
	//bool chamsan;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	CItems(float start_x, float start_y);
	virtual void SetState(int state);
};
