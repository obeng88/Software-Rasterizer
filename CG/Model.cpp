#include "Model.h"
#include <list>

Model::Model(const std::vector<Vector4>& vertices, const std::vector<Vector4>& normals, const std::vector<std::array<int, 3>>& polygons)
{
	this->vertices = vertices;
	this->normals = normals;
	this->polygons = polygons;
}

void Model::importObject(const Wavefront_obj &waveObject)
{
	vertices.clear();
	for (int i = 0; i < waveObject.m_points.size(); i++)
	{
		vertices.push_back(Vector4(waveObject.m_points[i][0], waveObject.m_points[i][1], waveObject.m_points[i][2]));
	}

	polygons.clear();
	for (int i = 0; i < waveObject.m_faces.size(); i++)
	{
		std::array<int, 3> polygon;
		polygon[0] = waveObject.m_faces[i].v[0];
		polygon[1] = waveObject.m_faces[i].v[1];
		polygon[2] = waveObject.m_faces[i].v[2];
		polygons.push_back(polygon);
	}

	addNormals();

	center();
	scale();
}

void Model::addNormals()
{
	normals.clear();
	normals.resize(vertices.size());
	for (int i = 0; i < polygons.size(); i++)
	{
		Vector4 v0 = vertices[polygons[i][0]].euclid();
		Vector4 v1 = vertices[polygons[i][1]].euclid();
		Vector4 v2 = vertices[polygons[i][2]].euclid();

		Vector4 firstTriVec = v1 - v0;
		firstTriVec.w = 1;
		Vector4 secondTriVec = v2 - v0;
		secondTriVec.w = 1;
		
		Vector4 orthogonal = firstTriVec.crossEuclid(secondTriVec);
		orthogonal.w = 0;

		normals[polygons[i][0]] += orthogonal;
		normals[polygons[i][1]] += orthogonal;
		normals[polygons[i][2]] += orthogonal;
	}

	for (int i = 0; i < normals.size(); i++)
	{
		normals[i] = normals[i].normalize();
	}
}

void Model::center() {
	Vector4 modelCenter;
	modelCenter.x = 0;
	modelCenter.y = 0;
	modelCenter.z = 0;
	for (const auto& v : vertices)
	{
		modelCenter.x += v.x;
		modelCenter.y += v.y;
		modelCenter.z += v.z;
	}
	modelCenter = modelCenter * (1.0 / vertices.size());
	for (auto& v : vertices)
	{
		v.x -= modelCenter.x;
		v.y -= modelCenter.y;
		v.z -= modelCenter.z;
	}
}

void Model::scale() {
	double max = vertices[0][0];
	for (const auto& v : vertices)
	{
		for (int j = 0; j < 3; j++)
		{
			if (max < abs(v[j]))
			{
				max = v[j];
			}
		}
	}

	double scale = 10.0 / max;
	for (auto& v : vertices)
	{
		v.x *= scale;
		v.y *= scale;
		v.z *= scale;
	}
}