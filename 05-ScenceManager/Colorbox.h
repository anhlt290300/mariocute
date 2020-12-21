
#include "GameObject.h"


class CColorbox : public CGameObject
{
	int height;
	int width;
public:
	CColorbox(int width, int height);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

};