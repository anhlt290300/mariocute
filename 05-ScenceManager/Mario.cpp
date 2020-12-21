#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"
#include "Floor.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Portal.h"
#include "Colorbox.h"
#include "Brick.h"
#include "Items.h"
#include "Coin.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_BIG_FIRE;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);

	start_x = x+10; 
	start_y = y; 
	this->x = x; 
	this->y = y; 
}
bool CMario::GetIsFalling()
{
	return IsFalling;
}
void CMario::SetIsFalling()
{
	CMario::IsFalling = true;
}
int CMario::GetLevel()
{
	return level;
}
void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);
	item = false;
	// Simple fall down
	vy += MARIO_GRAVITY*dt;
	last_vy = vy;
	//DebugOut(L"vy: \n%f: ", vy);
	if (vy >= 0)
	{
		SetIsFalling();

	}
	if (vx >= MARIO_RUNNING_MAX_SPEED||vx<= -MARIO_RUNNING_MAX_SPEED)
		IsMaxSpeed = true;	
	if (vx < MARIO_RUNNING_MAX_SPEED &&vx>-MARIO_RUNNING_MAX_SPEED)
		IsMaxSpeed = false;
	if (level == MARIO_LEVEL_BIG_TAIL && GetTickCount() - timeFly > 7000&&IsJumping&&IsMaxSpeed)
	{
		IsMaxSpeed = false;
		if(vx>0)
			vx -= MARIO_RUNNING_MAX_SPEED*0.005*dt;
		else
			vx += MARIO_RUNNING_MAX_SPEED*0.005*dt;
	}
	

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	if (isWaitingForAni && animation_set->at(ani)->IsRenderOver())
	{
		isWaitingForAni = false;
	}

	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{
		last_vx = vx;
		x += dx; 
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0; 
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 
		
		// block every object first!
		x += min_tx*dx + nx*0.4f;
		y += min_ty*dy + ny*0.4f;

		if (nx != 0)
		{
			if (IsRuning)
				vx = 0;
		}
		if (ny != 0)
		{
			if (ny < 0)
			{
				IsJumping = false;
			}
			vy = 0;
		}
		TimeNow = GetTickCount();
		

		// SET TIME VA CHAM

		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState()!= GOOMBA_STATE_DIE)
					{
						vy = -0.2;
						/*die = true;*/
						IsJumping = true;						
							goomba->SetState(GOOMBA_STATE_DIE);
							goomba->timestartdie = GetTickCount();
						
					}
					
					
				}
				else if (e->nx != 0)
				{
					if (GetState() == MARIO_ANI_BIG_TAIL_ATTACKING_RIGHT || GetState() == MARIO_ANI_BIG_TAIL_ATTACKING_LEFT)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							goomba->SetState(GOOMBA_STATE_DIE_FLY);
							goomba->vx = -goomba->vx;
						}
					}
					else if (untouchable==0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE || goomba->GetState() != GOOMBA_STATE_DIE_FLY)
						{
							if (level > MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_BIG;
								StartUntouchable();
							}
							else if (level == MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
							{
								SetState(MARIO_ANI_DIE);
								return;
							}
						}
					}
				}

			} 			
			else if (dynamic_cast<CKoopas*>(e->obj))
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if (e->ny < 0)
				{
					if (koopas->GetState() == KOOPAS_STATE_WALKING||koopas->GetState()== KOOPAS_STATE_DIE_BUT_RUN)
					{
						vy = -0.2;

						IsJumping = true;
						koopas->SetState(KOOPAS_STATE_DIE);
						koopas->timestartdie = GetTickCount();

					}
					else if (koopas->GetState() == KOOPAS_STATE_DIE)
					{
						koopas->SetState(KOOPAS_STATE_DIE_BUT_RUN);
						SetState(MARIO_STATE_KICK);
					}
				}
				else if (e->nx != 0)
				{
					if (GetState() == MARIO_ANI_BIG_TAIL_ATTACKING_RIGHT || GetState() == MARIO_ANI_BIG_TAIL_ATTACKING_LEFT)
					{
						
							koopas->upordown = true;
							koopas->vy = -0.1;
							koopas->timestartdie = GetTickCount();
							koopas->SetState(KOOPAS_STATE_DIE);

						
						
					}
					else if (koopas->GetState() == KOOPAS_STATE_DIE)
					{
						if (!IsRuning)
						{
							koopas->nx = -nx;
							koopas->SetState(KOOPAS_STATE_DIE_BUT_RUN);
							SetState(MARIO_STATE_KICK);
						}
						else
						{
							IsBring = true;
							
						}

					}
					else if (untouchable == 0)
					{
						if (koopas->GetState() == KOOPAS_STATE_WALKING )
						{
							if (level > MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_BIG;
								StartUntouchable();
							}
							else if (level == MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
							{
								SetState(MARIO_ANI_DIE);
								return;
							}
						}
					}
				}
			}
			else if (dynamic_cast<CPortal *>(e->obj))
			{
				CPortal *p = dynamic_cast<CPortal *>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
			else if (dynamic_cast<CFloor*>(e->obj))
			{
				if (e->ny < 0)
					IsFalling = false;
				/*else if (e->nx != 0)*/
					//DebugOut(L"Aaaaaaa");
			}
			else if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				if (e->ny > 0)
				{
					if (brick->block == false)
					{
						xGach = brick->x;
						yGach = brick->y;
						IDbrickk = brick->ID_brick;
						brick->block = true;
						brick->vacham = true;
						brick->vy = -0.1;
						brick->SetState(BRICK_STATE_HIT);
						brick->levelmario = GetLevel();
						item = true;
					}
				
						
				}
				else if (e->ny < 0)
				{
					vy = 0;
					IsFalling = false;
				}
				
			}
			else if (dynamic_cast<CItems*>(e->obj))
			{
				CItems* items = dynamic_cast<CItems*>(e->obj);
				if (e->ny != 0 || e->nx != 0)
				{
					if (level == MARIO_LEVEL_SMALL)
						SetLevel(MARIO_LEVEL_BIG);
					else if (level == MARIO_LEVEL_BIG)
						SetLevel(MARIO_LEVEL_BIG_TAIL);
					else
						SetLevel(MARIO_LEVEL_BIG_FIRE);
					items->vy = -999;
					vy = -0.14;
				}
			}
			else if (dynamic_cast<CCoin*>(e->obj))
			{
				CCoin* coin = dynamic_cast<CCoin*>(e->obj);
				if (e->ny != 0 || e->nx != 0)
				{
					vy = last_vy;
					coin->Isdone = true;
				}

			}
			else if (dynamic_cast<CColorbox*>(e->obj))
			{
				if (e->ny < 0)
				{
					IsFalling = false;
				}
				if (e->nx != 0)
				{
					vx = last_vx;
					x += dx;
				}
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	
}
void CMario::Render()
{
	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(state)->Render(x, y, alpha);

	RenderBoundingBox();
}

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
		/*DebugOut(L" me no \n", IsFlying);*/
	case MARIO_STATE_BIG_TAIL_WALKING_RIGHT:
		if (IsRuning)
		{
			if (!IsJumping)
			{
				if (vx < 0)
				{
					SetState(MARIO_ANI_BIG_TAIL_WALKING_STOP_LEFT);
					vx += MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
					if (vx < MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				if (IsSitting)
				{
					SetState(MARIO_STATE_SITTING);
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
				}
				else
				{
					if (!IsMaxSpeed)
					{

						SetState(MARIO_STATE_JUMP);
						timeFly = GetTickCount();
						vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
						if (vx >= MARIO_RUNNING_MAX_SPEED)
							vx = MARIO_RUNNING_MAX_SPEED;
					}
					else
					{
						SetState(MARIO_STATE_JUMP);

						vx = MARIO_RUNNING_MAX_SPEED;
					}

				}
			}
		}
		else if (IsJumping)
		{
			/*if (IsFlying)
			{
				if (vy < 0)
					ani = MARIO_ANI_BIG_TAIL_RUNNING_FLYING_RIGHT;
				else
					ani = MARIO_ANI_BIG_TAIL_RUNNING_FALLING_RIGHT;
					vx = MARIO_RUNNING_MAX_SPEED;
			}*/
			if (IsSitting)
			{
				SetState(MARIO_STATE_SITTING);
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx += MARIO_WALKING_ACCELEROMETER * dt * .05;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
			}
		}
		else if (!IsRuning)
		{
			if (vx < 0)
			{
				SetState(MARIO_ANI_BIG_TAIL_WALKING_STOP_LEFT);
				vx += MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx += MARIO_WALKING_ACCELEROMETER * dt * .05;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}



		nx = 1;
		break;
	case MARIO_STATE_BIG_TAIL_WALKING_LEFT:
		if (IsRuning)
		{
			if (!IsJumping)
			{
				if (vx > 0)
				{
					SetState(MARIO_ANI_BIG_TAIL_WALKING_STOP_RIGHT);
					vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
					if (vx > -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				if (IsSitting)
				{
					SetState(MARIO_STATE_SITTING);
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
				}
				else
				{
					if (!IsMaxSpeed)
					{
						timeFly = GetTickCount();
						SetState(MARIO_STATE_JUMP);
						vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
						if (vx <= -MARIO_RUNNING_MAX_SPEED)
							vx = -MARIO_RUNNING_MAX_SPEED;
					}
					else
					{
						SetState(MARIO_STATE_JUMP);
						vx = -MARIO_RUNNING_MAX_SPEED;
					}

				}
			}
		}
		else if (IsJumping)
		{
			/*if(IsFlying)
			{
				if (vy < 0)
					ani = MARIO_ANI_BIG_TAIL_RUNNING_FLYING_LEFT;
				else
					ani = MARIO_ANI_BIG_TAIL_RUNNING_FALLING_LEFT;
				vx = -MARIO_RUNNING_MAX_SPEED;
			}*/
			if (IsSitting)
			{
				SetState(MARIO_STATE_SITTING);
				vx -= MARIO_WALKING_ACCELEROMETER * dt * .05;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx -= MARIO_WALKING_ACCELEROMETER * dt * .05;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
			}
		}
		else if (!IsRuning)
		{
			if (vx > 0)
			{
				SetState(MARIO_ANI_BIG_TAIL_WALKING_STOP_RIGHT);
				vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx -= MARIO_WALKING_ACCELEROMETER * dt * .05;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}


		nx = -1;
		break;
	case MARIO_STATE_BIG_WALKING_RIGHT:
		if (IsRuning)
		{
			if (!IsJumping)
			{
				if (vx < 0)
				{
					SetState(MARIO_ANI_BIG_WALKING_STOP_RIGHT);
					vx += MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
					if (vx < MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				if (IsSitting)
				{
					SetState(MARIO_STATE_SITTING);
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
				}
				else
				{
					SetState(MARIO_STATE_JUMP);
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
				}
			}
		}
		else if (IsJumping)
		{
			if (IsSitting)
			{
				SetState(MARIO_STATE_SITTING);
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
			}
		}
		else if (!IsRuning)
		{
			if (vx < 0)
			{
				SetState(MARIO_ANI_BIG_WALKING_STOP_LEFT);
				vx += MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}



		nx = 1;
		break;
	case MARIO_STATE_BIG_WALKING_LEFT:
		if (IsRuning)
		{
			if (!IsJumping)
			{
				if (vx > 0)
				{
					SetState(MARIO_ANI_BIG_WALKING_STOP_LEFT);
					vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
					if (vx > -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					else if (vx <= -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				if (IsSitting)
				{
					SetState(MARIO_STATE_SITTING);
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
				}
				else
				{
					SetState(MARIO_STATE_JUMP);
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
				}
			}
		}
		else if (IsJumping)
		{
			if (IsSitting)
			{
				SetState(MARIO_STATE_SITTING);
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
			}
		}
		else if (!IsRuning)
		{
			if (vx > 0)
			{
				SetState(MARIO_ANI_BIG_WALKING_STOP_RIGHT);
				vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}


		nx = -1;
		break;
	case MARIO_STATE_BIG_FIRE_WALKING_RIGHT:
		if (IsRuning)
		{
			if (!IsJumping) {
				if (vx < 0)
				{
					SetState(MARIO_ANI_BIG_FIRE_WALKING_STOP_LEFT);
					vx += MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
					if (vx < MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				if (IsSitting)
				{
					SetState(MARIO_STATE_SITTING);
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
				}
				else
				{
					SetState(MARIO_STATE_JUMP);
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
				}
			}
		}
		else if (IsJumping)
		{
			if (IsSitting)
			{
				SetState(MARIO_STATE_SITTING);
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
			}
		}
		else if (!IsRuning)
		{
			if (vx < 0)
			{
				SetState(MARIO_ANI_BIG_FIRE_WALKING_STOP_LEFT);
				vx += MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}



		nx = 1;
		break;
	case MARIO_STATE_BIG_FIRE_WALKING_LEFT:
		if (IsRuning)
		{
			if (!IsJumping) {
				if (vx > 0)
				{
					SetState(MARIO_ANI_BIG_FIRE_WALKING_STOP_RIGHT);
					vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
					if (vx > -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					else if (vx <= -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				if (IsSitting)
				{
					SetState(MARIO_STATE_SITTING);
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
				}
				else
				{
					SetState(MARIO_STATE_JUMP);
					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
				}
			}
		}
		else if (IsJumping)
		{
			if (IsSitting)
			{
				SetState(MARIO_STATE_SITTING);
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
			}
		}
		else if (!IsRuning)
		{
			if (vx > 0)
			{
				SetState(MARIO_ANI_BIG_FIRE_WALKING_STOP_RIGHT);
				vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}


		nx = -1;
		break;
	case MARIO_STATE_SMALL_WALKING_RIGHT:
		if (IsRuning)
		{
			if (!IsJumping)
			{
				if (vx < 0)
				{
					SetState(MARIO_ANI_SMALL_WALKING_STOP_LEFT);
					vx += MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{
					vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						vx = MARIO_RUNNING_MAX_SPEED;
					if (vx < MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					if (vx >= MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{

				SetState(MARIO_STATE_JUMP);
				vx += MARIO_WALKING_ACCELEROMETER * dt * 0.005;
				if (vx >= MARIO_RUNNING_MAX_SPEED)
					vx = MARIO_RUNNING_MAX_SPEED;
			}
		}
		else if (IsJumping)
		{
			SetState(MARIO_STATE_JUMP);
			vx += MARIO_WALKING_ACCELEROMETER * dt;
			if (vx >= MARIO_WALKING_MAX_SPEED)
				vx = MARIO_WALKING_MAX_SPEED;
		}
		else if (!IsRuning)
		{
			if (vx < 0)
			{
				SetState(MARIO_ANI_SMALL_WALKING_STOP_LEFT);
				vx += MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx += MARIO_WALKING_ACCELEROMETER * dt;
				if (vx >= MARIO_WALKING_MAX_SPEED)
					vx = MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}
			nx = 1;
			break;
	case MARIO_STATE_SMALL_WALKING_LEFT:
		if (IsRuning)
		{
			if (!IsJumping)
			{
				if (vx > 0)
				{
					SetState(MARIO_ANI_SMALL_WALKING_STOP_RIGHT);
					vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
				}
				else
				{

					vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						vx = -MARIO_RUNNING_MAX_SPEED;
					if (vx > -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_WALKING);
					if (vx <= -MARIO_RUNNING_MAX_SPEED)
						SetState(MARIO_STATE_RUNNING);
				}
			}
			else
			{
				SetState(MARIO_STATE_JUMP);
				vx -= MARIO_WALKING_ACCELEROMETER * dt * 0.005;
				if (vx <= -MARIO_RUNNING_MAX_SPEED)
					vx = -MARIO_RUNNING_MAX_SPEED;
			}
		}
		else if (IsJumping)
		{
			SetState(MARIO_STATE_JUMP);
			vx -= MARIO_WALKING_ACCELEROMETER * dt;
			if (vx <= -MARIO_WALKING_MAX_SPEED)
				vx = -MARIO_WALKING_MAX_SPEED;
		}
		else if (!IsRuning)
		{
			if (vx > 0)
			{
				SetState(MARIO_ANI_SMALL_WALKING_STOP_RIGHT);
				vx -= MARIO_WALKING_ACCELEROMETER * 1.8;
			}
			else
			{
				vx -= MARIO_WALKING_ACCELEROMETER * dt;
				if (vx <= -MARIO_WALKING_MAX_SPEED)
					vx = -MARIO_WALKING_MAX_SPEED;
				SetState(MARIO_STATE_WALKING);
			}
		}
		nx = -1;
		break;
	case MARIO_STATE_IDLE:
		IsRuning = false;
		IsSitting = false;
		if (level == MARIO_LEVEL_BIG)
		{
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx < 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx < 0)
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx > 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx == 0)
			{
				if (nx > 0)
					SetState(MARIO_ANI_BIG_IDLE_RIGHT);
				else
					SetState(MARIO_ANI_BIG_IDLE_LEFT);
			}
		}
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx < 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx < 0)
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx > 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx == 0)
			{
				if (nx > 0)
					SetState(MARIO_ANI_SMALL_IDLE_RIGHT);
				else
					SetState(MARIO_ANI_SMALL_IDLE_LEFT);
			}
		}
		else if (level == MARIO_LEVEL_BIG_FIRE)
		{
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx < 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx < 0)
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx > 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx == 0)
			{
				if (nx > 0)
					SetState(MARIO_ANI_BIG_FIRE_IDLE_RIGHT);
				else
					SetState(MARIO_ANI_BIG_FIRE_IDLE_LEFT);
			}
		}
		else
		{
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx < 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx < 0)
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx > 0)
					vx = 0;
				SetState(MARIO_STATE_WALKING);
			}
			if (vx == 0)
			{
				if (!IsBring)
				{
					if (nx > 0)
						SetState(MARIO_ANI_BIG_TAIL_IDLE_RIGHT);
					else
						SetState(MARIO_ANI_BIG_TAIL_IDLE_LEFT);
				}
				else
				{
					if (nx > 0)
						SetState(MARIO_ANI_BIG_TAIL_BRING_IDLE_RIGHT);
					else
						SetState(MARIO_ANI_BIG_TAIL_BRING_IDLE_LEFT);
				}
			}

		}
		break;
	case MARIO_ANI_BIG_TAIL_FLYING_RIGHT:
		break;
	case MARIO_ANI_BIG_TAIL_FLYING_LEFT:
		break;
	case MARIO_ANI_BIG_TAIL_FALLING_RIGHT:
		break;
	case MARIO_ANI_BIG_TAIL_FALLING_LEFT:
		break;
	case MARIO_ANI_BIG_FLYING_RIGHT:
		break;
	case MARIO_ANI_BIG_FLYING_LEFT:
		break;
	case MARIO_ANI_BIG_FIRE_FLYING_RIGHT:
		break;
	case MARIO_ANI_BIG_FIRE_FLYING_LEFT:
		break;
	case MARIO_ANI_BIG_FIRE_FALLING_LEFT:
		break;
	case MARIO_ANI_BIG_FIRE_FALLING_RIGHT:
		break;
	case MARIO_ANI_SMALL_FLYING_RIGHT:
		break;
	case MARIO_ANI_SMALL_FLYING_LEFT:
		break;
	case MARIO_ANI_BIG_FALLING_RIGHT:
		break;
	case MARIO_ANI_BIG_FALLING_LEFT:
		break;
	case MARIO_ANI_SMALL_IDLE_RIGHT:
		break;
	case MARIO_ANI_SMALL_IDLE_LEFT:
		break;
	case MARIO_ANI_BIG_IDLE_RIGHT:
		break;
	case MARIO_ANI_BIG_IDLE_LEFT:
		break;
	case MARIO_ANI_BIG_TAIL_IDLE_RIGHT:
		break;
	case MARIO_ANI_BIG_TAIL_IDLE_LEFT:
		break;
	case MARIO_ANI_BIG_FIRE_IDLE_RIGHT:
		break;
	case MARIO_ANI_BIG_FIRE_IDLE_LEFT:
		break;
	case MARIO_ANI_BIG_SITTING_RIGHT:
		break;
	case MARIO_ANI_BIG_SITTING_LEFT:
		break;
	case MARIO_ANI_BIG_TAIL_SITTING_RIGHT:
		break;
	case MARIO_ANI_BIG_TAIL_SITTING_LEFT:
		break;
	case MARIO_ANI_BIG_FIRE_SITTING_RIGHT:
		break;
	case MARIO_ANI_BIG_FIRE_SITTING_LEFT:
		break;
	case MARIO_ANI_BIG_TAIL_BRING_IDLE_RIGHT:
		break;
	case MARIO_ANI_BIG_TAIL_BRING_IDLE_LEFT:
		break;
	case MARIO_ANI_BIG_TAIL_BRING_WALKING_RIGHT:
		break;
	case MARIO_ANI_BIG_TAIL_BRING_WALKING_LEFT:
		break;
	case MARIO_ANI_BIG_TAIL_WALKING_STOP_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_WALKING_STOP_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_WALKING_STOP_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_WALKING_STOP_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_ATTACKING_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_ATTACKING_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_FLYING_ATTACKING_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_FLYING_ATTACKING_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_WALKING_STOP_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_WALKING_STOP_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_SMALL_WALKING_STOP_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_SMALL_WALKING_STOP_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_ATTACKING_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_ATTACKING_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_SLOW_FALLING_FLYING_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_SLOW_FALLING_FLYING_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_RUNNING_FALLING_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_RUNNING_FALLING_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_KICK_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_FIRE_KICK_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_KICK_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_TAIL_KICK_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_KICK_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_BIG_KICK_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_SMALL_KICK_RIGHT:
		ResetAni();
		isWaitingForAni = true;
		break;
	case MARIO_ANI_SMALL_KICK_LEFT:
		ResetAni();
		isWaitingForAni = true;
		break;

	case MARIO_STATE_SITTING:
		if (GetLevel() == 2)
		{
			if (nx > 0)
				SetState(MARIO_ANI_BIG_SITTING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_SITTING_LEFT);
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
			else
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
		}
		else if (GetLevel() == 3)
		{
			if (nx > 0)
				SetState(MARIO_ANI_BIG_TAIL_SITTING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_TAIL_SITTING_LEFT);
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
			else
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
		}
		else if (GetLevel() == 4)
		{
			if (nx > 0)
				SetState(MARIO_ANI_BIG_FIRE_SITTING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_FIRE_SITTING_LEFT);
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
			else
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
		}
		/*else
		{
			if (nx > 0)
				ani = MARIO_ANI_BIG_TAIL_SITTING_RIGHT;
			else
				ani = MARIO_ANI_BIG_TAIL_SITTING_LEFT;
			if (vx > 0)
			{
				vx -= MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
			else
			{
				vx += MARIO_STOPRUNNING * dt;
				if (vx == 0)
					vx = 0;
			}
		}*/

		break;

	case MARIO_STATE_WALKING:
		if (level == MARIO_LEVEL_SMALL)
			if (!IsBring)
			{
				if (nx == 1)
					SetState(MARIO_ANI_SMALL_WALKING_RIGHT);
				else
					SetState(MARIO_ANI_SMALL_WALKING_LEFT);
			}
			else
			{
				/*if (nx == 1)
					SetState(MARIO_ANI_SMALL_BRING_SHELL_WALKING_RIGHT);
				else
					SetState(MARIO_ANI_SMALL_WALKING_LEFT);*/
			}
		else if (level == MARIO_LEVEL_BIG)
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_WALKING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_WALKING_LEFT);
		}
		else if (level == MARIO_LEVEL_BIG_TAIL)
		{
			if (!IsBring)
			{
				if (nx == 1)
					SetState(MARIO_ANI_BIG_TAIL_WALKING_RIGHT);
				else
					SetState(MARIO_ANI_BIG_TAIL_WALKING_LEFT);
			}
			else
			{
				if (nx == 1)
					SetState(MARIO_ANI_BIG_TAIL_BRING_WALKING_RIGHT);
				else
					SetState(MARIO_ANI_BIG_TAIL_BRING_WALKING_LEFT);
			}
		}
		else
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_FIRE_WALKING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_FIRE_WALKING_LEFT);
		}
		break;
	case MARIO_STATE_RUNNING:
		if (level == MARIO_LEVEL_SMALL)
			if (nx == 1)
				SetState(MARIO_ANI_SMALL_RUNNING_RIGHT);
			else
				SetState(MARIO_ANI_SMALL_RUNNING_LEFT);
		else if (level == MARIO_LEVEL_BIG)
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_RUNNING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_RUNNING_LEFT);
		}
		else if (level == MARIO_LEVEL_BIG_TAIL)
		{
			if (!IsBring)
			{
				if (nx == 1)
					SetState(MARIO_ANI_BIG_TAIL_RUNNING_RIGHT);
				else
					SetState(MARIO_ANI_BIG_TAIL_RUNNING_LEFT);
			}
			else
			{
				if (nx == 1)
					SetState(MARIO_ANI_BIG_TAIL_BRING_WALKING_RIGHT);
				else
					SetState(MARIO_ANI_BIG_TAIL_BRING_WALKING_LEFT);
			}
		}
		else
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_FIRE_RUNNING_RIGHT);
			else
				SetState(MARIO_ANI_BIG_FIRE_RUNNING_LEFT);
		}
		break;
	case MARIO_STATE_JUMP:
		if (level == MARIO_LEVEL_SMALL)
			if (nx == 1)
				SetState(MARIO_ANI_SMALL_FLYING_RIGHT);
			else
				SetState(MARIO_ANI_SMALL_FLYING_LEFT);
		else if (level == MARIO_LEVEL_BIG)
		{
			if (nx == 1)
			{
				if (vy < 0)
					SetState(MARIO_ANI_BIG_FLYING_RIGHT);
				else
					SetState(MARIO_ANI_BIG_FALLING_RIGHT);
			}
			else
			{
				if (vy < 0)
					SetState(MARIO_ANI_BIG_FLYING_LEFT);
				else
					SetState(MARIO_ANI_BIG_FALLING_LEFT);
			}
		}
		else if (level == MARIO_LEVEL_BIG_TAIL)
		{
			if (!IsMaxSpeed)
			{
				if (nx == 1)
				{
					if (vy < 0)
						SetState(MARIO_ANI_BIG_TAIL_FLYING_RIGHT);
					else
						SetState(MARIO_ANI_BIG_TAIL_FALLING_RIGHT);
				}
				else
				{
					if (vy < 0)
						SetState(MARIO_ANI_BIG_TAIL_FLYING_LEFT);
					else
						SetState(MARIO_ANI_BIG_TAIL_FALLING_LEFT);
				}
			}
			else
			{
				if (nx == 1)
				{
					if (vy < 0)
						SetState(MARIO_ANI_BIG_TAIL_RUNNING_FLYING_RIGHT);
					else
						SetState(MARIO_ANI_BIG_TAIL_RUNNING_FALLING_RIGHT);
				}
				else
				{
					if (vy < 0)
						SetState(MARIO_ANI_BIG_TAIL_RUNNING_FLYING_LEFT);
					else
						SetState(MARIO_ANI_BIG_TAIL_RUNNING_FALLING_LEFT);
				}

			}
		}
		else
		{
			if (nx == 1)
			{
				if (vy < 0)
					SetState(MARIO_ANI_BIG_FIRE_FLYING_RIGHT);
				else
					SetState(MARIO_ANI_BIG_FIRE_FALLING_RIGHT);
			}
			else
			{
				if (vy < 0)
					SetState(MARIO_ANI_BIG_FIRE_FLYING_LEFT);
				else
					SetState(MARIO_ANI_BIG_FIRE_FALLING_LEFT);
			}
		}

		break;
	case MARIO_STATE_KICK:
		if (level == MARIO_LEVEL_BIG_FIRE)
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_FIRE_KICK_RIGHT);
			else
				SetState(MARIO_ANI_BIG_FIRE_KICK_LEFT);
		}
		else if(level==MARIO_LEVEL_BIG_TAIL)
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_TAIL_KICK_RIGHT);
			else
				SetState(MARIO_ANI_BIG_TAIL_KICK_LEFT);
		}
		else if (level == MARIO_LEVEL_BIG)
		{
			if (nx == 1)
				SetState(MARIO_ANI_BIG_KICK_RIGHT);
			else
				SetState(MARIO_ANI_BIG_KICK_LEFT);
		}
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (nx == 1)
				SetState(MARIO_ANI_SMALL_KICK_RIGHT);
			else
				SetState(MARIO_ANI_SMALL_KICK_LEFT);
		}
		break;
	}

	
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level==MARIO_LEVEL_BIG || level==MARIO_LEVEL_BIG_FIRE)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if(level == MARIO_LEVEL_SMALL)
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	else
	{
		right = x + MARIO_BIG_TAIL_BBOX_WIDTH;
		bottom = y + MARIO_BIG_TAIL_BBOX_HEIGHT;

	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);

	SetLevel(MARIO_LEVEL_BIG_TAIL);
	SetPosition(start_x+1100, start_y);
	SetSpeed(0, 0);
	CGame::GetInstance()->cam_x = x-20;
}

