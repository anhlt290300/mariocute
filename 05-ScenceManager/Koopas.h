#pragma once

#include "GameObject.h"
#include "Mario.h"

#define KOOPAS_WALKING_SPEED 0.03f;

#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_HEIGHT_DIE 16

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_DIE 200
#define KOOPAS_STATE_DIE_BUT_RUN 201
#define KOOPAS_STATE_DIE_FLY 202
#define KOOPAS_STATE_WAIT_TO_LIFE_1	203
#define KOOPAS_STATE_WAIT_TO_LIFE_2 204

#define KOOPAS_ANI_WALKING_LEFT 0
#define KOOPAS_ANI_WALKING_RIGHT 1
#define KOOPAS_ANI_DIE_DOWN	2
#define KOOPAS_ANI_DIE_UP	5
#define	KOOPAS_ANI_MOVE_DOWN	4
#define	KOOPAS_ANI_MOVE_UP		6
#define	KOOPAS_ANI_WAIT_LIVE_1	7
#define	KOOPAS_ANI_WAIT_LIVE_2	8

class CKoopas : public CGameObject
{
public:
	bool upordown = false;
	bool lastbring;
	float lastx;
	CMario* mario;
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();
	int laststate;
	CKoopas(CMario *mario);
	virtual void SetState(int state);
};