#pragma once
#include"Mario.h"
#include "GameObject.h"
#define MOVING_SPEED 0.15
#define FIRE_STATE_LOST 2
#define FIRE_BALL_MOVE 0
#define FIRE_BALL_BOOM 1
class CWeapon : public CGameObject
{
	bool spawn = true;
public:
	bool isWaitingForAni;
	float start_x;
	float start_y;
	float getvy()
	{
		return vy;
	}
	float vynow;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	CWeapon(float docaonhatdinh, float dorongnhatdinh, int marionx);
	//virtual void SetState(int state);
};
