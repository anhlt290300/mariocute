#include "Game.h"
#include "Textures.h"
#include "Scence.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Items.h"
#include "Weapon.h"
#include "TileMap.h"
#include "Flower.h"

class CPlayScene: public CScene
{
protected: 
	CMario *player;	
	TileMap* map;
	CWeapon* weapon;

	CItems* items;
	// A play scene has to have player, right? 

	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> listweapon;
	vector<LPGAMEOBJECT> listitem;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_TileMap(string line);
	int RandomItems();
	CWeapon* MadeWeapon(float x, float y, int nx)
	{
		CWeapon* a = new CWeapon(x, y, nx);
		a->SetPosition(x, y);
		return a;
	}
	CItems* MadeItems(float x, float y)
	{
		CItems* a = new CItems(y, x);
		a->SetPosition(x, y);
		a->vy = -0.2;
		a->ID_brickkk = player->IDbrickk;
		a->level_mario = player->GetLevel();
		/*a->SetState(Mushroom);*/
	/*	a->SetState(RandomItems());*/
		return a;
	}
public: 
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMario * GetPlayer() { return player; } 

	//friend class CPlayScenceKeyHandler;
};

class CPlayScenceKeyHandler : public CScenceKeyHandler
{
public: 
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) ;
	CPlayScenceKeyHandler(CScene *s) :CScenceKeyHandler(s) {};
};

