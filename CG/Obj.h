#pragma once

#include "Model.h"

struct Obj
{
	Model mesh;
	Matrix4 model;
	Vector4 box[8];

	Obj(const Model& mesh = Model(), const Matrix4& model = Matrix4::identity());
	void calcBox();
};