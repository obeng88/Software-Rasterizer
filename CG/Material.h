#pragma once
#include "Renderer.h"
struct Material {
	UINT32 kAmbient;
	UINT32 kDiffuse;
	UINT32 kSpecular;
	double normSpecular;
};