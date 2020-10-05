#pragma once

#include "Math4.h"

struct Camera
{
	Matrix4 view;
	bool ortho;
	double n;
	double f;
	double fov;

	Camera(const Matrix4& view = Matrix4::identity(), double n = 0.01, double f = 10000, double fov = 60, bool ortho = true);
};