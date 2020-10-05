#pragma once

#include "Math4.h"
#include <vector>
#include "Obj Parser/wavefront_obj.h"
#include <array>

struct Model
{
	std::vector<Vector4> vertices;
	std::vector<Vector4> normals;
	std::vector<std::array<int, 3>> polygons;

	Model() {}
	Model(const std::vector<Vector4>& vertices, const std::vector<Vector4>& normals, const std::vector<std::array<int, 3>>& polygons);

	void importObject(const Wavefront_obj &waveObject);
	void addNormals();
	void center();
	void scale();
};