#pragma once
#include "GameObject.h"


class CFireball : public CGameObject
{
public:
	bool isbottom;
	bool istop;
	CFireball(D3DXVECTOR2 position, int nx);
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

	~CFireball();

};

