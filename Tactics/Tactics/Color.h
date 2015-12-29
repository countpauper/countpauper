#pragma once
#include <gl/GL.h>
struct RGBA 
{ 
	GLubyte r, g, b, a; 
	void Render()
	{
		glColor4ub(r, g, b, a);
	}
};
