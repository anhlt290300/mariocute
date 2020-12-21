#include "ColorBox.h"


CColorbox::CColorbox(int width, int height) : CGameObject::CGameObject()
{
	this->width = width;
	this->height = height;
}

void CColorbox::Render()
{
	/*RenderBoundingBox();*/
}

void CColorbox::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + 16 * width;
	b = y + 16 * height;
}