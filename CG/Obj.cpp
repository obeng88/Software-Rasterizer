#include "Obj.h"

Obj::Obj(const Model& mesh, const Matrix4& model)
{
	this->mesh = mesh;
	this->model = model;
}

void Obj::calcBox()
{
	double min[3] = { 0, 0, 0 };
	double max[3] = { 0, 0, 0 };
	for (const auto& v : mesh.vertices)
	{
		if ((v[0] / v[3]) > min[0])
		{
			min[0] = v[0] / v[3];
		}
		if ((v[1] / v[3]) > min[1])
		{
			min[1] = v[1] / v[3];
		}
		if ((v[2] / v[3]) > min[2])
		{
			min[2] = v[2] / v[3];
		}

		if ((v[0] / v[3]) < max[0])
		{
			max[0] = v[0] / v[3];
		}
		if ((v[1] / v[3]) < max[1])
		{
			max[1] = v[1] / v[3];
		}
		if ((v[2] / v[3]) < max[2])
		{
			max[2] = v[2] / v[3];
		}
	}
	box[0] = Vector4(min[0], min[1], min[2]);
	box[1] = Vector4(min[0], max[1], min[2]);
	box[2] = Vector4(max[0], max[1], min[2]);
	box[3] = Vector4(max[0], min[1], min[2]);

	box[4] = Vector4(min[0], min[1], max[2]);
	box[5] = Vector4(min[0], max[1], max[2]);
	box[6] = Vector4(max[0], max[1], max[2]);
	box[7] = Vector4(max[0], min[1], max[2]);
}