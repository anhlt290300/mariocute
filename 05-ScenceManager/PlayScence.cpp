#include <iostream>
#include <fstream>

#include "PlayScence.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Floor.h"
#include "Colorbox.h"
#include "MonneyEffect.h"
#include "Coin.h"

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath):
	CScene(id, filePath)
{
	key_handler = new CPlayScenceKeyHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS	6
#define SCENE_SECTION_TILEMAP 20

#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_BRICK	1
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPAS	3
#define OBJECT_TYPE_FLOOR	4	
#define OBJECT_TYPE_COLORBOX 5
#define OBJECT_TYPE_FLOWER	6
#define OBJECT_TYPE_WEAPON	12
#define OBJECT_TYPE_ITEMS	8
#define OBJECT_TYPE_MONEY	9
#define OBJECT_TYPE_COIN	7

#define OBJECT_TYPE_PORTAL	50

#define MAX_SCENE_LINE 1024

void CPlayScene::_ParseSection_TileMap(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 9) return;
	int ID = atoi(tokens[0].c_str());
	wstring file_texture = ToWSTR(tokens[1]);
	wstring file_path = ToWSTR(tokens[2]);
	int row_on_textures = atoi(tokens[3].c_str());
	int col_on_textures = atoi(tokens[4].c_str());
	int row_on_tile_map = atoi(tokens[5].c_str());
	int col_on_tile_map = atoi(tokens[6].c_str());
	int tile_width = atoi(tokens[7].c_str());
	int tile_height = atoi(tokens[8].c_str());
	//int texID = atoi(tokens[0].c_str());
	map = new TileMap(ID, file_texture.c_str(), file_path.c_str(), row_on_textures, col_on_textures, row_on_tile_map, col_on_tile_map, tile_width, tile_height);

}

void CPlayScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return; 
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i+1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations *animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());
		
		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS] 
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{

	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; 
	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets * animation_sets = CAnimationSets::GetInstance();

	CGameObject *obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player!=NULL) 
		{
			return;
		}
		obj = new CMario(x,y); 
		player = (CMario*)obj;  

		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_GOOMBA:
		obj = new CGoomba();
		break;
	case OBJECT_TYPE_KOOPAS:
		obj = new CKoopas(player);
		break;
	case OBJECT_TYPE_FLOWER: 
		
		obj = new CFlower(player);
		break;
	case OBJECT_TYPE_BRICK:
	{
		int id = atoi(tokens[4].c_str());
		obj = new CBrick(player,id);

		break;
	}
	case OBJECT_TYPE_ITEMS:
	{
		obj = new CItems(x, y);
		break;
	}
	case OBJECT_TYPE_WEAPON:
	{
		obj = new CWeapon(x, y, player->nx);
		break;
	}
	case OBJECT_TYPE_MONEY:
	{
		obj = new CMonneyEffect();
		break;
	}
	case OBJECT_TYPE_FLOOR:
	{
		float width = atof(tokens[4].c_str());
		float height = atof(tokens[5].c_str());
		obj = new CFloor(width, height);
		break;
	}
	case OBJECT_TYPE_COIN:
	{
		obj = new CCoin();
		break;
	}
	case OBJECT_TYPE_COLORBOX:
	{
		float width = atof(tokens[4].c_str());
		float height = atof(tokens[5].c_str());
		obj = new CColorbox(width, height);
		break;
	}
	case OBJECT_TYPE_PORTAL:
		{	
			float r = atof(tokens[4].c_str());
			float b = atof(tokens[5].c_str());
			int scene_id = atoi(tokens[6].c_str());
			obj = new CPortal(x, y, r, b, scene_id);
		}
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
}
int CPlayScene::RandomItems()
{
	//CMario* a = new CMario();
	//a = player;
	//int random = rand()%2;
	if (player->level == MARIO_LEVEL_SMALL)
		return Mushroom;
	else if (player->level == MARIO_LEVEL_BIG)
		return Tree_Leaf;
	/*else if (player->level == MARIO_LEVEL_BIG_TAIL)
		return FIRE_FLOWER;*/
	else if (player->level == MARIO_LEVEL_BIG_FIRE)
		return Tree_Leaf;

	//DebugOut(L"radomitem %d \n", RandomItems());
}
void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;					

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") { 
			section = SCENE_SECTION_SPRITES; continue; }
		if (line == "[ANIMATIONS]") { 
			section = SCENE_SECTION_ANIMATIONS; continue; }
		if (line == "[ANIMATION_SETS]") { 
			section = SCENE_SECTION_ANIMATION_SETS; continue; }
		if (line == "[OBJECTS]") { 
			section = SCENE_SECTION_OBJECTS; continue; }
		if (line == "[TILEMAP]")
		{
			section = SCENE_SECTION_TILEMAP;
			continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }	
		
		//
		// data section
		//
		switch (section)
		{ 
			case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
			case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
			case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
			case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
			case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
			case SCENE_SECTION_TILEMAP: _ParseSection_TileMap(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);


	
	}

	for (size_t i = 0; i < listitem.size(); i++)
	{
		coObjects.push_back(listitem[i]);



	}


	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
		LPGAMEOBJECT a = objects[i];
	}
	if (player->item)
	{
		listitem.push_back(MadeItems(player->xGach, player->yGach - 2));
	}
	if (player->ban && !player->Isdone)
	{
		if (player->nx > 0)
			listweapon.push_back(MadeWeapon(player->x + 10, player->y + 6, player->nx));
		else
			listweapon.push_back(MadeWeapon(player->x - 6, player->y + 6, player->nx));
		player->Isdone = true;
	}
	
	/*player->Collision_items(&listitems);*/
	if (listweapon.size() != 0)
	{
		for (int i = 0; i < listweapon.size(); i++)
		{
			listweapon[i]->Update(dt, &coObjects);
		}
	}
	if (listitem.size() != 0)
	{
		for (int i = 0; i < listitem.size(); i++)
		{
			listitem[i]->Update(dt, &coObjects);
		}
	}


	if (player == NULL) return;

		// Update camera to follow mario
	float cx, cy;
	player->GetPosition(cx, cy);

	CGame* game = CGame::GetInstance();
	cx -= (int)game->GetScreenWidth() / 2;
	cy -= (int)game->GetScreenHeight() / 2;

	CGame::GetInstance()->cam_y = 150;
	if (player->x > (SCREEN_WIDTH / 2) && player->x + (SCREEN_WIDTH / 2) < map->GetWidthTileMap())
	{
		cx = player->x - (SCREEN_WIDTH / 2);
		CGame::GetInstance()->cam_x = cx;
	}
	
}

void CPlayScene::Render()
{
	map->Draw();

	if (listweapon.size() != 0)
	{
		for (int i = 0; i < listweapon.size(); i++)
		{
			listweapon.at(i)->Render();
			//DebugOut(L"LISEEEEEEE %d \n", listweapon.size());
		}
	}

	if (listitem.size() != 0)
	{
		for (int i = 0; i < listitem.size(); i++)
		{
			listitem.at(i)->Render();
			//DebugOut(L"LISEEEEEEE %d \n", listweapon.size());
		}
	}
	for (int i = 0; i < objects.size(); i++)
		objects[i]->Render();
}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	player = NULL;

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMario *mario = ((CPlayScene*)scence)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_SPACE:
		if (mario->IsJumping)
			return;
		mario->vy = -MARIO_JUMP_MAX_SPEED_Y;
		mario->IsJumping = true;
		mario->IsFalling = false;
		break;
	case DIK_A: 
		mario->Reset();

		break;
	case DIK_F:
		if (mario->GetLevel() == 4)
		{
			DebugOut(L"xuat ra \n", mario->x);
			mario->ban = true;
			if (mario->IsJumping)
			{
				if (mario->nx == 1)
					mario->SetState(MARIO_ANI_BIG_FIRE_FLYING_ATTACKING_RIGHT);
				else
					mario->SetState(MARIO_ANI_BIG_FIRE_FLYING_ATTACKING_LEFT);
			}
			else
			{
				if (mario->nx == 1)
					mario->SetState(MARIO_ANI_BIG_FIRE_ATTACKING_RIGHT);
				else
					mario->SetState(MARIO_ANI_BIG_FIRE_ATTACKING_LEFT);
			}
		}
		break;
	case DIK_S:
		if (mario->GetLevel() == 3)
		{
			if (mario->nx == 1)
				mario->SetState(MARIO_ANI_BIG_TAIL_ATTACKING_RIGHT);
			else
				mario->SetState(MARIO_ANI_BIG_TAIL_ATTACKING_LEFT);
		}
		else
			return;
		break;
	case DIK_1:
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		mario->vy = -0.2;
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		mario->vy = -0.2;
		mario->SetLevel(MARIO_LEVEL_BIG_TAIL);
		break;
	case DIK_4:
		mario->vy = -0.2;
		mario->SetLevel(MARIO_LEVEL_BIG_FIRE);
		break;
	}
}
void CPlayScenceKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMario* mario = ((CPlayScene*)scence)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_RIGHT:
		break;
	case DIK_LEFT:
		break;
	case DIK_LSHIFT:
		mario->IsRuning = false;
		if (mario->IsBring)
		{
			mario->SetState(MARIO_STATE_KICK);
			mario->IsBring = false;
		}
		break;
	case DIK_F:
		mario->ban = false;
		mario->Isdone = false;
		break;
	case DIK_SPACE:
		if (mario->IsFalling)
		{
			if (mario->GetLevel() == 3 && mario->IsJumping)
			{
				if (!mario->IsRuning)
				{
					if (!mario->IsSitting)
					{
						if (mario->nx > 0)
							mario->SetState(MARIO_ANI_BIG_TAIL_SLOW_FALLING_FLYING_RIGHT);
						else
							mario->SetState(MARIO_ANI_BIG_TAIL_SLOW_FALLING_FLYING_LEFT);
						mario->vy = 0;
					}
					else
						return;
				}
				else
				{
					if (!mario->IsMaxSpeed)
					{
						if (mario->nx > 0)
							mario->SetState(MARIO_ANI_BIG_TAIL_SLOW_FALLING_FLYING_RIGHT);
						else
							mario->SetState(MARIO_ANI_BIG_TAIL_SLOW_FALLING_FLYING_LEFT);
						mario->vy = 0;
						if (mario->vx > 0)
							mario->vx -= MARIO_WALKING_ACCELEROMETER * mario->dt*0.05;
						else
							mario->vx += MARIO_WALKING_ACCELEROMETER * mario->dt*0.05;
					}
					else
					{
						if (mario->nx > 0)
						{
							if(mario->vy<0)
								mario->SetState(MARIO_ANI_BIG_TAIL_RUNNING_FLYING_RIGHT);
							else
								mario->SetState(MARIO_ANI_BIG_TAIL_RUNNING_FALLING_RIGHT);
						}
						else
						{
							if (mario->vy < 0)
								mario->SetState(MARIO_ANI_BIG_TAIL_RUNNING_FLYING_LEFT);
							else
								mario->SetState(MARIO_ANI_BIG_TAIL_RUNNING_FALLING_LEFT);
						}
						mario->vy = -0.1;
					}
				}
			}
			else
				mario->vy = mario->vy + MARIO_GRAVITY * 15 * mario->dt;
		}
		else
			mario->vy = 0;
		break;


	case DIK_DOWN:
		mario->IsSitting = false;
		break;

	}

}

void CPlayScenceKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	if (mario->GetState() == MARIO_STATE_DIE) return;

	

	if (game->IsKeyDown(DIK_LSHIFT))
	{
			mario->IsRuning = true;

	}
	if (game->IsKeyDown(DIK_RIGHT) && game->IsKeyDown(DIK_LEFT))
	{
		mario->SetState(MARIO_STATE_IDLE);
		return;
	}
	if (mario->isWaitingForAni)
		return;
	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (mario->GetLevel() == 1)
			mario->SetState(MARIO_STATE_SMALL_WALKING_RIGHT);
		else if (mario->GetLevel() == 2)
			mario->SetState(MARIO_STATE_BIG_WALKING_RIGHT);
		else if (mario->GetLevel() == 4)
			mario->SetState(MARIO_STATE_BIG_FIRE_WALKING_RIGHT);
		else
			mario->SetState(MARIO_STATE_BIG_TAIL_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (mario->GetLevel() == 1)
			mario->SetState(MARIO_STATE_SMALL_WALKING_LEFT);
		else if (mario->GetLevel() == 2)
			mario->SetState(MARIO_STATE_BIG_WALKING_LEFT);
		else if (mario->GetLevel() == 4)
			mario->SetState(MARIO_STATE_BIG_FIRE_WALKING_LEFT);
		else
			mario->SetState(MARIO_STATE_BIG_TAIL_WALKING_LEFT);
	}
	else if (game->IsKeyDown(DIK_DOWN))
	{
		mario->IsSitting = true;
		if (mario->GetLevel() == 2|| mario->GetLevel()==3||mario->GetLevel()==4)
			mario->SetState(MARIO_STATE_SITTING);
		else
			return;
	}
	
	else
	{
		if (mario->IsJumping)
		{
			if (mario->GetLevel() == 2)
			{
				if (mario->GetIsFalling())
				{
					if (mario->nx > 0)
						mario->SetState(MARIO_ANI_BIG_FALLING_RIGHT);
					else
						mario->SetState(MARIO_ANI_BIG_FALLING_LEFT);
				}
				else if (!mario->GetIsFalling())
				{
					if (mario->nx > 0)
						mario->SetState(MARIO_ANI_BIG_FLYING_RIGHT);
					else
						mario->SetState(MARIO_ANI_BIG_FLYING_LEFT);
				}
			}
			else if (mario->GetLevel() == 1)
			{
				if (mario->nx > 0)
					mario->SetState(MARIO_ANI_SMALL_FLYING_RIGHT);
				else
					mario->SetState(MARIO_ANI_SMALL_FLYING_LEFT);
			}
			else if(mario->GetLevel()==3)
			{
				if (!mario->IsMaxSpeed)
				{
					if (mario->GetIsFalling())
					{
						if (mario->nx > 0)
							mario->SetState(MARIO_ANI_BIG_TAIL_FALLING_RIGHT);
						else
							mario->SetState(MARIO_ANI_BIG_TAIL_FALLING_LEFT);
					}
					else if (!mario->GetIsFalling())
					{
						if (mario->nx > 0)
							mario->SetState(MARIO_ANI_BIG_TAIL_FLYING_RIGHT);
						else
							mario->SetState(MARIO_ANI_BIG_TAIL_FLYING_LEFT);
					}
				}
				else
				{
					if (mario->vx > 0)
					{
						mario->vx -= MARIO_WALKING_ACCELEROMETER * mario->dt*0.005;
						
					}
					else
					{
						mario->vx += MARIO_WALKING_ACCELEROMETER * mario->dt*0.005;
				
					}
						
				}
			}
			else if (mario->GetLevel() == 4)
			{
				if (mario->GetIsFalling())
				{
					if (mario->nx > 0)
						mario->SetState(MARIO_ANI_BIG_FIRE_FALLING_RIGHT);
					else
						mario->SetState(MARIO_ANI_BIG_FIRE_FALLING_LEFT);
				}
				else if (!mario->GetIsFalling())
				{
					if (mario->nx > 0)
						mario->SetState(MARIO_ANI_BIG_FIRE_FLYING_RIGHT);
					else
						mario->SetState(MARIO_ANI_BIG_FIRE_FLYING_LEFT);
				}
			}
			return;
		}
		
		mario->SetState(MARIO_STATE_IDLE);
			
		
	}
}