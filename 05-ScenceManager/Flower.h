#pragma once

#include "GameObject.h"
#include "Mario.h"
#include "Fireball.h"

#define GIANT_BOX_WIDTH					16
#define GIANT_BOX_HEIGHT				32
#define GIANT_GREEN_BOX_HEIGHT			24


#define FLOWER_ANI_UP_BUT_DOWN_LEFT		2
#define FLOWER_ANI_UP_BUT_UP_RIGHT		4
#define FLOWER_ANI_UP_BUT_DOWN_RIGHT	6
#define FLOWER_ANI_UP_BUT_UP_LEFT		0


#define FLOWER_ANI_FIRE_RIGHT_60		5
#define FLOWER_ANI_FIRE_LEFT_60			1
#define FLOWER_ANI_FIRE_RIGHT_120		7	
#define FLOWER_ANI_FIRE_LEFT_120		3

#define FLOWER_STATE_UP					100
#define FLOWER_STATE_DOWN				200

class CFlower : public CGameObject
{
	int stateLocal;
public:
	vector<LPGAMEOBJECT> objects;
	vector <LPGAMEOBJECT> listFireBall;
	int ny=-1;
	float a = 0.015f;
	CMario* mario;
	DWORD timewaittoshoot ;
	bool moveup = true;
	bool movedown= false;
	bool upordown = false;
	bool leftorright = false;
	CFlower(CMario* mario);
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

};


