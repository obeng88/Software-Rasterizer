#pragma once
#include "Renderer.h"
#include "Obj.h"
#include "Camera.h"
#include "Math4.h"
#include "Light.h"
#include <vector>

enum ShaderType{FLAT,GOUROUD,PHONG};
typedef std::array<double, 2> PointD;
typedef std::array<int, 2> PointI;
typedef std::array<PointD, 3> TriangleD;
typedef std::array<PointI, 3> TriangleI;
typedef std::array<Vector4, 3> TriangleV;
typedef std::array<double, 3> ZBuff;

void fillTriangle(const Vector4& camPos, ShaderType shader, Material material, const light& l1, const light& l2, const light& l3, TriangleV vertices, TriangleV normals, TriangleI t, ZBuff& buffer, std::vector<double>& zBufferMatrix, std::vector<UINT32>& pBufferMatrix, int w, int h, int i);
void drawLine(int, int, int, int, UINT32, std::vector<UINT32>&);
void drawPoints(const PointI&, const PointI&, UINT32, std::vector<UINT32>&);
void drawScene(const Matrix4& objRot,const Material& material,const Vector4& camPos, ShaderType shader, const light& l1, const light& l2, const light& l3, const Obj& obj, const Camera& cam, const Matrix4& proj, std::vector<double>& zBuffMatrix, std::vector<UINT32>& pBuffMatrix, double w, double h, UINT32 color, bool showWorldCoords, bool showObjCoords, bool showBox, bool showNormals, bool showWireframe);
void drawTriangle(const TriangleI&, UINT32, std::vector<UINT32>& pBuffMatrix);
int transformPoints(int&,int&,int&,int&);
void transformPixel(Pixel&,int);