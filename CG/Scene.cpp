
#include "Scene.h"
#include <vector>
#include "Renderer.h"

#define IN_BOX(v) (v[0] >= -1.0 && v[0] <= 1.0 && v[1] >= -1.0 && v[1] <= 1.0 && v[2] >= -1.0 && v[2] <= 1.0)


void FlatShader(double bary0,double bary1,double bary2,UINT32 color,const light& l1, const light& l2, const light& l3, ZBuff& buffer, std::vector<double>& zBufferMatrix, std::vector<UINT32>& pBufferMatrix,int w,int h,int x,int y)
{
	double zCurrentPixel = abs(bary0)* buffer[0] + abs(bary1) * buffer[1] + abs(bary2) * buffer[2];
	if (zCurrentPixel < zBufferMatrix[x + y * w]) {
		zBufferMatrix[x + y * w] = zCurrentPixel;
		pBufferMatrix[x + y * w] = color;
	}

}

void GouroudShader(double bary0, double bary1, double bary2, UINT32 c0,UINT32 c1,UINT32 c2, const light& l1, const light& l2, const light& l3, ZBuff& buffer, std::vector<double>& zBufferMatrix, std::vector<UINT32>& pBufferMatrix, int w, int h, int x, int y)
{
	double zCurrentPixel = abs(bary0)* buffer[0] + abs(bary1) * buffer[1] + abs(bary2) * buffer[2];

	if (zCurrentPixel < zBufferMatrix[x + y * w]) {
		UINT32 color;
		UINT8* byteColor = (UINT8*)&color;
		UINT8* byteColorC0 = (UINT8*)&c0;
		UINT8* byteColorC1 = (UINT8*)&c1;
		UINT8* byteColorC2 = (UINT8*)&c2;
		for (int i = 0; i <= 2; i++)
		{
			byteColor[i] = abs(bary0)* byteColorC0[i] + abs(bary1) * byteColorC1[i] + abs(bary2) * byteColorC2[i];
		}
		zBufferMatrix[x + y * w] = zCurrentPixel;
		pBufferMatrix[x + y * w] = color;
	}
}

void PhongShader(double bary0, double bary1, double bary2,const TriangleV& normals, const light& l1, const light& l2, const light& l3, ZBuff& buffer, std::vector<double>& zBufferMatrix, std::vector<UINT32>& pBufferMatrix, int w, int h, int x, int y,const Material& material,const TriangleV& vertices,const Vector4& camPos)
{
	double zCurrentPixel = abs(bary0)* buffer[0] + abs(bary1) * buffer[1] + abs(bary2) * buffer[2];

	if (zCurrentPixel < zBufferMatrix[x + y * w]) {
		UINT32 color;
		Vector4 normal;
		for (int i = 0; i <= 2; i++)
		{
			normal[i] = abs(bary0)* normals[0][i] + abs(bary1) * normals[1][i] + abs(bary2) * normals[2][i];
		}
		Vector4 polyPos;
		for (int i = 0; i <= 2; i++)
		{
			polyPos[i] = abs(bary0)* vertices[0][i] + abs(bary1) * vertices[1][i] + abs(bary2) * vertices[2][i];
		}
		color = light::superPosLights(l1, l2, l3, material, normal, polyPos, camPos);
		zBufferMatrix[x + y * w] = zCurrentPixel;
		pBufferMatrix[x + y * w] = color;
	}
}

	double edgeFunction(PointI& a,PointI& b,PointI& c)
{
	return ((a[1] - b[1])*c[0] + (b[0] - a[0])*c[1] + (a[0] * b[1] - a[1] * b[0]));
	
}

// returns pointI such that box[0] = min , box[1] = max
std::array <PointI, 2> boundingBox(TriangleI& t)
{
	std::array<PointI, 2> box = { t[0],t[0] };

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (t[i][j] < box[0][j])
			{
				box[0][j] = t[i][j];
			}
			if (t[i][j] > box[1][j])
			{
				box[1][j] = t[i][j];
			}
		}

	}

	return box;
}

void fillTriangle(const Vector4& camPos,ShaderType shader,Material material,const light& l1, const light& l2, const light& l3,TriangleV vertices,TriangleV normals ,TriangleI t, ZBuff& buffer,std::vector<double>& zBufferMatrix,std::vector<UINT32>& pBufferMatrix , int w, int h,int i)
{
	std::array<PointI, 2> box = boundingBox(t);
	Pixel pixelToDraw;
	TriangleV euclidVertices = { vertices[0].euclid(),vertices[1].euclid(),vertices[2].euclid() };
	Vector4 v0v1 = euclidVertices[1] - euclidVertices[0];
	v0v1.w = 1;
	Vector4 v0v2 = euclidVertices[2] - euclidVertices[0];
	v0v2.w = 1;
	Vector4 normal = v0v1.crossEuclid(v0v2).normalize();
	PointI p;
	UINT32 color;
	UINT32 c0, c1, c2;

	switch (shader)
	{
	case FLAT:
		{
		Vector4 center = (euclidVertices[0] + euclidVertices[1] + euclidVertices[2])* (1.0 / 3.0);
		center.w = 1;
		color = light::superPosLights(l1, l2, l3,material,normal,center,camPos);
		}
		break;
	case GOUROUD:
		{
		c0 = light::superPosLights(l1, l2, l3, material, normals[0], vertices[0], camPos);
		c1 = light::superPosLights(l1, l2, l3, material, normals[1], vertices[1], camPos);
		c2 = light::superPosLights(l1, l2, l3, material, normals[2], vertices[2], camPos);

		}
		break;
	case PHONG:
		{

		}
		break;
	}
	for (int y = box[0][1]; y < box[1][1]; y++)
	{
		for (int x = box[0][0]; x < box[1][0]; x++)
		{
			p = { x,y };

			bool inside = true;
			bool insideCCW = true;
			bool insideCW = false;
			double bary0, bary2, bary1;
			bool edge1, edge2, edge3;

			Vector4 v0(t[1][0] - t[0][0], t[1][1] - t[0][1]);
			Vector4 v1(t[2][0] - t[0][0], t[2][1] - t[0][1]);
			Vector4 crosProd = v0.crossEuclid(v1);

			double area = crosProd.distance();
			bary2 = edgeFunction(t[0], t[1], p) /area;
			bary0 = edgeFunction(t[1], t[2], p) / area;
			bary1 = edgeFunction(t[2], t[0], p) / area;
		
			inside = (bary0>=0 && bary0 <=1 && bary1>=0 && bary1<=1 && bary2>=0 && bary2<=1)
				||(bary0<=0 && bary0 >=-1 && bary1<=0 && bary1 >=-1 && bary2<=0 && bary2>=-1);
			if (inside == true) {
				switch (shader)
				{
				case FLAT:
					FlatShader(bary0, bary1, bary2, color, l1, l2, l3, buffer, zBufferMatrix, pBufferMatrix, w, h, x, y);
					break;
				case GOUROUD:
					GouroudShader(bary0, bary1, bary2, c0,c1,c2, l1, l2, l3, buffer, zBufferMatrix, pBufferMatrix, w, h, x, y);
					break;
				case PHONG:
					PhongShader(bary0, bary1, bary2, normals, l1, l2, l3, buffer, zBufferMatrix, pBufferMatrix, w, h, x, y,material,vertices,camPos);
					break;
				}
					
				
			}
		}
	}
}

void drawLine(int x1, int y1, int x2, int y2, UINT32 color, std::vector<UINT32>& pBuffMatrix)
{
	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	int transCase = transformPoints(x1, y1, x2, y2);
	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = 2 * dy - dx;
	int x = x1;
	int	y = y1;
	int de = 2 * dy;
	int dne = 2 * (dy - dx);

	while (x < x2)
	{
		if (d < 0)
		{
			d = d + de;
			x = x + 1;
		}
		else
		{
			d = d + dne;
			x = x + 1;
			y = y + 1;
		}
		Pixel pixelToDraw;
		pixelToDraw.x = x;
		pixelToDraw.y = y;
		pixelToDraw.color = color;
		transformPixel(pixelToDraw, transCase);
		pBuffMatrix[pixelToDraw.x + pixelToDraw.y * w ] = color;
	}
}

void drawPoints(const PointI& p1, const PointI& p2, UINT32 color, std::vector<UINT32>& pBuffMatrix)
{
	drawLine(p1[0], p1[1], p2[0], p2[1], color,pBuffMatrix);
}


void drawScene(const Matrix4& objRot,const Material& material,const Vector4& camPos,ShaderType shader,const light& l1,const light& l2,const light& l3,const Obj& obj, const Camera& cam, const Matrix4& proj,std::vector<double>& zBuffMatrix,std::vector<UINT32>& pBuffMatrix ,double w, double h, UINT32 color, bool showWorldCoords, bool showObjCoords, bool showBox, bool showNormals, bool showWireframe)
{
	static std::vector<Vector4> projVertices;
	static std::vector<TriangleD> viewTris;
	static std::vector<TriangleI> screenTris;
	static std::vector<TriangleV> triangles;
	static std::vector<ZBuff> zbufferTri;
	static UINT32 wireColor = 0xffffffff;
	if (obj.mesh.vertices.size() == 0)
		return;

	Renderer renderer;

	
	projVertices.clear();
	viewTris.clear();
	screenTris.clear();
	triangles.clear();
	zbufferTri.clear();

	for (auto & z : zBuffMatrix)
		z = INFINITY;
	
	for (auto & pix : pBuffMatrix)
		pix = 0;

	//world coordinates
	if (showWorldCoords)
	{
		Vector4 xAxis(2, 0, 0);
		Vector4 yAxis(0, 2, 0);
		Vector4 zAxis(0, 0, 2);
		Vector4 zeroAxis(0, 0, 0);
		Matrix4 mult = cam.view * proj;
		xAxis = xAxis * mult;
		yAxis = yAxis * mult;
		zAxis = zAxis * mult;
		zeroAxis = zeroAxis * mult;
		xAxis = xAxis.euclid();
		yAxis = yAxis.euclid();
		zAxis = zAxis.euclid();
		zeroAxis = zeroAxis.euclid();
		PointI x = { w / 2.0 * (xAxis[0] + 1), h / 2.0 * (xAxis[1] + 1) };
		PointI y = { w / 2.0 * (yAxis[0] + 1), h / 2.0 * (yAxis[1] + 1) };
		PointI z = { w / 2.0 * (zAxis[0] + 1), h / 2.0 * (zAxis[1] + 1) };
		PointI zero = { w / 2.0 * (zeroAxis[0] + 1), h / 2.0 * (zeroAxis[1] + 1) };
		if (IN_BOX(zeroAxis) && IN_BOX(xAxis))
		{
			drawLine(zero[0], zero[1], x[0], x[1], 0xff0000ff, pBuffMatrix);
		}
		if (IN_BOX(zeroAxis) && IN_BOX(yAxis))
		{
			drawLine(zero[0], zero[1], y[0], y[1], 0xff00ff00, pBuffMatrix);
		}
		if (IN_BOX(zeroAxis) && IN_BOX(zAxis))
		{
			drawLine(zero[0], zero[1], z[0], z[1], 0xffff0000, pBuffMatrix);
		}
	}

	Matrix4 mult = obj.model * cam.view * proj;

	//object coordinates
	if (showObjCoords)
	{
		Vector4 xAxis(2, 0, 0);
		Vector4 yAxis(0, 2, 0);
		Vector4 zAxis(0, 0, 2);
		Vector4 zeroAxis(0, 0, 0);
		xAxis = xAxis * mult;
		yAxis = yAxis * mult;
		zAxis = zAxis * mult;
		zeroAxis = zeroAxis * mult;
		xAxis = xAxis.euclid();
		yAxis = yAxis.euclid();
		zAxis = zAxis.euclid();
		zeroAxis = zeroAxis.euclid();
		PointI x = { w / 2.0 * (xAxis[0] + 1), h / 2.0 * (xAxis[1] + 1) };
		PointI y = { w / 2.0 * (yAxis[0] + 1), h / 2.0 * (yAxis[1] + 1) };
		PointI z = { w / 2.0 * (zAxis[0] + 1), h / 2.0 * (zAxis[1] + 1) };
		PointI zero = { w / 2.0 * (zeroAxis[0] + 1), h / 2.0 * (zeroAxis[1] + 1) };
		if (IN_BOX(zeroAxis) && IN_BOX(xAxis))
		{
			drawLine(zero[0], zero[1], x[0], x[1], 0xff0000ff, pBuffMatrix);
		}
		if (IN_BOX(zeroAxis) && IN_BOX(yAxis))
		{
			drawLine(zero[0], zero[1], y[0], y[1], 0xff00ff00, pBuffMatrix);
		}
		if (IN_BOX(zeroAxis) && IN_BOX(zAxis))
		{
			drawLine(zero[0], zero[1], z[0], z[1], 0xffff0000, pBuffMatrix);
		}
	}

	//transform the vertices and perspective division
	for (const auto& v : obj.mesh.vertices)
	{
		projVertices.push_back((v * mult).euclid());
	}

	//convert the polygons in object (3 index integers) to a vector of 3 Vector4s only the polygons inside the identity box
	for (const auto& p : obj.mesh.polygons)
	{
		if (IN_BOX(projVertices[p[0]]) && IN_BOX(projVertices[p[1]]) && IN_BOX(projVertices[p[2]]))
		{
			TriangleV tri = { projVertices[p[0]], projVertices[p[1]], projVertices[p[2]] };
			ZBuff buffer = { projVertices[p[0]].z, projVertices[p[1]].z, projVertices[p[2]].z };
			zbufferTri.push_back(buffer);
			triangles.push_back(tri);
		}
	}

	//2d transform and put points in triangle formation
	for (const auto& t : triangles)
	{
		TriangleD triangle;
		int i = 0;
		for (const auto& v : t) //each vertex in the triangle
		{
			PointD point = { v[0], v[1] };
			triangle[i++] = point;
		}
		viewTris.push_back(triangle);
	}

	//screenspace transform
	for (const auto& t : viewTris)
	{
		TriangleI triangle;
		int i = 0;
		for (const auto& p : t)
		{
			PointI point = { round(w/2 * (p[0]+1)), round(h/2 * (p[1]+1)) };
			triangle[i++] = point;
		}
		screenTris.push_back(triangle);
	}

	//bounding box
	if (showBox)
	{

		Vector4 projBox[8] =
		{
			(obj.box[0] * mult).euclid(),
			(obj.box[1] * mult).euclid(),
			(obj.box[2] * mult).euclid(),
			(obj.box[3] * mult).euclid(),
			(obj.box[4] * mult).euclid(),
			(obj.box[5] * mult).euclid(),
			(obj.box[6] * mult).euclid(),
			(obj.box[7] * mult).euclid()};
			
		PointI boxPoints[8] = {
			{round(w/2*(projBox[0][0]+1)), round(h/2*(projBox[0][1]+1))},
			{round(w/2*(projBox[1][0]+1)), round(h/2*(projBox[1][1]+1))},
			{round(w/2*(projBox[2][0]+1)), round(h/2*(projBox[2][1]+1))},
			{round(w/2*(projBox[3][0]+1)), round(h/2*(projBox[3][1]+1))},
			{round(w/2*(projBox[4][0]+1)), round(h/2*(projBox[4][1]+1))},
			{round(w/2*(projBox[5][0]+1)), round(h/2*(projBox[5][1]+1))},
			{round(w/2*(projBox[6][0]+1)), round(h/2*(projBox[6][1]+1))},
			{round(w/2*(projBox[7][0]+1)), round(h/2*(projBox[7][1]+1))},
		};

		UINT32 color = 0xff88ff88;
		if (IN_BOX(projBox[0]) && IN_BOX(projBox[1]))
		{
			drawPoints(boxPoints[0], boxPoints[1], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[1]) && IN_BOX(projBox[2]))
		{
			drawPoints(boxPoints[1], boxPoints[2], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[2]) && IN_BOX(projBox[3]))
		{
			drawPoints(boxPoints[2], boxPoints[3], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[0]) && IN_BOX(projBox[3]))
		{
			drawPoints(boxPoints[0], boxPoints[3], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[4]) && IN_BOX(projBox[5]))
		{
			drawPoints(boxPoints[4], boxPoints[5], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[5]) && IN_BOX(projBox[6]))
		{
			drawPoints(boxPoints[5], boxPoints[6], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[6]) && IN_BOX(projBox[7]))
		{
			drawPoints(boxPoints[6], boxPoints[7], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[4]) && IN_BOX(projBox[7]))
		{
			drawPoints(boxPoints[4], boxPoints[7], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[0]) && IN_BOX(projBox[4]))
		{
			drawPoints(boxPoints[0], boxPoints[4], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[1]) && IN_BOX(projBox[5]))
		{
			drawPoints(boxPoints[1], boxPoints[5], color, pBuffMatrix);
		}
		if (IN_BOX(projBox[2]) && IN_BOX(projBox[6]))
		{
			drawPoints(boxPoints[2], boxPoints[6], color,	pBuffMatrix);
		}
		if (IN_BOX(projBox[3]) && IN_BOX(projBox[7]))
		{
			drawPoints(boxPoints[3], boxPoints[7], color, pBuffMatrix);
		}
	}

	//vertex normals
	if (showNormals)
	{
		static std::vector<Vector4> projNormals;
		static std::vector<PointI> normalPoints;
		static std::vector<PointI> verticesPoints;
		
		projNormals.clear();
		normalPoints.clear();
		verticesPoints.clear();

		for (int i = 0; i < obj.mesh.normals.size(); i++)
		{
			Vector4 tmp = obj.mesh.normals[i].euclid();
			tmp[3] = 0;
			projNormals.push_back(obj.mesh.vertices[i].euclid() + tmp);
			projNormals[i] = (projNormals[i] * mult).euclid();
		}

		for (const auto& n : projNormals)
		{
			PointI p = {round(w/2*(n[0]+1)), round(h/2*(n[1]+1))};
			normalPoints.push_back(p);
		}
		for (const auto& v : projVertices)
		{
			PointI p = {round(w/2*(v[0]+1)), round(h/2*(v[1]+1))};
			verticesPoints.push_back(p);
		}

		UINT32 color = 0xffff8888;
		for (int i = 0; i < projNormals.size(); i++)
		{
			if (IN_BOX(projNormals[i]) && IN_BOX(projVertices[i]))
			{
				drawPoints(normalPoints[i], verticesPoints[i], color, pBuffMatrix);
			}
		}
	}
	
	//draw
	
	for (int i = 0; i < screenTris.size();i++)
	{
		
		if (IN_BOX(projVertices[obj.mesh.polygons[i][0]]) && IN_BOX(projVertices[obj.mesh.polygons[i][1]]) && IN_BOX(projVertices[obj.mesh.polygons[i][2]]))
		{
			if (showWireframe)
			{
				drawTriangle(screenTris[i], wireColor, pBuffMatrix);
			}
			TriangleV vertices = { obj.mesh.vertices[obj.mesh.polygons[i][0]] * obj.model,obj.mesh.vertices[obj.mesh.polygons[i][1]] * obj.model,obj.mesh.vertices[obj.mesh.polygons[i][2]] * obj.model };
			TriangleV normals = { obj.mesh.normals[obj.mesh.polygons[i][0]] * objRot,obj.mesh.normals[obj.mesh.polygons[i][1]] * objRot,obj.mesh.normals[obj.mesh.polygons[i][2]] * objRot };
			fillTriangle(camPos, shader, material, l1, l2, l3, vertices, normals, screenTris[i], zbufferTri[i], zBuffMatrix, pBuffMatrix, w, h, i % 6);
		}
	}
	renderer.drawPixels(pBuffMatrix);
}

void drawTriangle(const TriangleI& tris, UINT32 color, std::vector<UINT32>& pBuffMatrix)
{
	for (int i = 0; i < 3; i++)
	{
		drawLine(tris[i][0], tris[i][1], tris[(i + 1) % 3][0], tris[(i + 1) % 3][1], color, pBuffMatrix);
	}
}

int transformPoints(int& x1,int& y1,int& x2,int& y2)
// return value is a case seperation for us to remember
{
	int temp = 0;
	int dx=0, dy = 0;
	if (x1 > x2)
	{
		// swapping p1 and p2
		temp = x2;
		x2 = x1;
		x1 = temp;
		temp = y2;
		y2 = y1;
		y1 = temp;
	}
	dx = x2 - x1;
	dy = y2 - y1;
	if (dy > dx)
	{
		// when the angle is between 45 to 90
		// rotating -90 and mirroring via x axis
		temp = x1;
		x1 = y1;
		y1 = temp;
		temp = x2;
		x2 = y2;
		y2 = temp;
		return 1;
	}
	else if ((dy >= -dx) && (dy < 0))
	{
		// when angle is between 0 to -45
		// mirroring via x axis
		y1 = -y1;
		y2 = -y2;
		return 2;
	}
	else if ((dy < -dx) && (dy < 0))
	{
		// when angle is between -45 to -90
		// rotating 90
		temp = x1;
		x1 = -y1;
		y1 = temp;
		temp = x2;
		x2 = -y2;
		y2 = temp;
		return 3;
	}
	return 4;
}

void transformPixel(Pixel& pixel, int transCase)
{
	int temp = 0;
	
	switch (transCase)
	{
	case 1:
		// mirroring via x axis and rotating 90
		temp = pixel.x;
		pixel.x = pixel.y;
		pixel.y = temp;
		break;
	case 2:
		// mirroring via x axis
		pixel.y = -pixel.y;
		break;
	case 3:
		// rotating -90
		temp = pixel.x;
		pixel.x = pixel.y;
		pixel.y = -temp;
		break;
	}
}