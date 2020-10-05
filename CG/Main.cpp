#define _USE_MATH_DEFINES

#include <iostream>

#include <AntTweakBar/include/AntTweakBar.h>
#include <Glew/include/gl/glew.h>
#include <freeglut/include/GL/freeglut.h>

#include <vector>
#include <windows.h>
#include <assert.h>

#include "Utils.h"
#include "Renderer.h"
#include "Obj.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
using namespace std;

LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
LARGE_INTEGER Frequency;



double g_Scale = 1.0;

#define DATA_CAMERA 0
#define DATA_OBJECT 1
#define DATA_WORLD 2

#define PI 3.14159265359


struct Data
{
	Matrix4& mat;
	double* pos;
	double* dir;
	double* quat;
	int type;
};


UINT32 alpha= 120,blue = 0,green = 0,red = 120;
Obj obj;
Camera cam;
double camPos[3];
double objPos[3];
double worldPos[3];
Matrix4 proj;
double camDir[3];
double objDir[3];
double worldDir[3];
double camQuat[4] = { 0, 0, 0, 1 };
double objQuat[4] = { 0, 0, 0, 1 };
double worldQuat[4] = { 0, 0, 0, 1 };
bool showWorldCoords;
bool showObjCoords;
bool showBox;
bool showNormals;
bool showWireframe;
bool projType;
int windowWidth, windowHeight;
std::vector<double> zBufferMatrix;
std::vector<UINT32> pixelBufferMatrix;

Data camData = { cam.view, camPos, camDir, camQuat, DATA_CAMERA };
Data objData = { obj.model, objPos, objDir, objQuat, DATA_OBJECT };
Data worldData = { obj.model, worldPos, worldDir, worldQuat, DATA_WORLD };
ambientLight ambLight;
pointLight p1Light, p2Light;
Material objMaterial;
ShaderType shader;


//obj data type
Wavefront_obj objScene;

void TW_CALL loadOBJModel(void* clientData);
void initScene();
void initGraphics(int argc, char *argv[]);
void Display();
void Reshape(int width, int height);
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void PassiveMouseMotion(int x, int y);
void Keyboard(unsigned char k, int x, int y);
void Special(int k, int x, int y);
void Terminate(void);

void updateMatrix(int type);
void TW_CALL setPos(const void* value, void* clientData);
void TW_CALL getPos(void* value, void* clientData);
void TW_CALL setQuat(const void* value, void* clientData);
void TW_CALL getQuat(void* value, void* clientData);
void TW_CALL setProj(const void* value, void* clientData);
void TW_CALL getProj(void* value, void* clientData);
void TW_CALL setScale(const void* value, void* clientData);
void TW_CALL getScale(void* value, void* clientData);
void TW_CALL lookAt(void* data);



int main(int argc, char *argv[])
{
	// Initialize openGL, glut, glew
	initGraphics(argc, argv);
	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);
	//initialize the timer frequency
	QueryPerformanceFrequency(&Frequency); 
	// Set GLUT callbacks
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(PassiveMouseMotion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	
	//send 'glutGetModifers' function pointer to AntTweakBar.
	//required because the GLUT key event functions do not report key modifiers states.
	//TwGLUTModifiersFunc(glutGetModifiers);


	atexit(Terminate);  //called after glutMainLoop ends

	projType = false;
	double n = cam.n = 0.001;
	double f = cam.f = 1000;
	double fov = cam.fov = 60;
	
	double t = tan(2*M_PI*cam.fov/720)*n;
	double b = -t;
	double r = t*glutGet(GLUT_WINDOW_WIDTH)/(double)glutGet(GLUT_WINDOW_HEIGHT);
	double l = -r;
	proj = Matrix4(
		Vector4(2.0*n / (r - l), 0, 0, 0),
		Vector4(0, 2.0*n / (t - b), 0, 0),
		Vector4((r+l)/(r-l), (t+b)/(t-b), -(f + n) / (f - n), -1),
		Vector4(0, 0, -2.0*f*n/(f-n), 0.0)
	);
	
	objMaterial.kAmbient = 0xffff0000;
	objMaterial.kDiffuse = 0xff00ff00;
	objMaterial.kSpecular = 0xff0000ff;
	objMaterial.normSpecular = 8;
	ambLight.intensity = 0.1;
	p1Light.intensity = 1;
	p2Light.intensity = 1;
	p1Light.position = Vector4(30, 10, 30, 1);
	p2Light.position = Vector4(-30, 10, -30, 1);
	shader = FLAT;
	g_Scale = 0.8;

	camQuat[0] = objQuat[0] = worldQuat[0] = 0;
	camQuat[1] = objQuat[1] = worldQuat[1] = 0;
	camQuat[2] = objQuat[2] = worldQuat[2] = 0;
	camQuat[3] = objQuat[3] = worldQuat[3] = 1;
	camDir[0] = objDir[0] = worldDir[0] = 0;
	camDir[1] = objDir[1] = worldDir[1] = 0;
	camDir[2] = objDir[2] = worldDir[2] = 0;
	cam.view = Matrix4::translate((camPos[0] = 0), (camPos[1] = 0), -(camPos[2] = 20));
	obj.model = Matrix4::identity();
	
	showWorldCoords = true;
	showObjCoords = false;
	showBox = false;
	showNormals = false;

	// Create a tweak bar
	TwBar* bar = TwNewBar("TweakBar");


	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.
	TwDefine(" TweakBar size='200 400' color='96 216 224' "); // change default tweak bar size and color
	TwEnumVal shaderVal[] = { {FLAT,"Flat Shading"},{GOUROUD,"Gouroud Shading"},{PHONG,"Phong Shading"} };
	TwType ShaderChoice = TwDefineEnum("Shader",shaderVal,3);
	TwAddVarRW(bar, "Shader Type", ShaderChoice, &shader,NULL);
	TwAddButton(bar, "open", loadOBJModel, NULL, " label='Open OBJ File...' ");
	TwAddVarCB(bar, "Camera Position", TW_TYPE_DIR3D, setPos, getPos, &camData, " step=1.0 ");
	TwAddVarCB(bar, "Object Position", TW_TYPE_DIR3D, setPos, getPos, &objData, " step=1.0 ");
	TwAddVarCB(bar, "World Position", TW_TYPE_DIR3D, setPos, getPos, &worldData, " step=1.0 ");
	TwAddVarCB(bar, "Camera Rotation", TW_TYPE_QUAT4D, setQuat, getQuat, &camData, " step=1.0 ");
	TwAddVarCB(bar, "Object Rotation", TW_TYPE_QUAT4D, setQuat, getQuat, &objData, " step=1.0 ");
	TwAddVarCB(bar, "World Rotation", TW_TYPE_QUAT4D, setQuat, getQuat, &worldData, " step=1.0 ");
	TwAddVarRW(bar, "Light1 Position", TW_TYPE_DIR3D, &p1Light.position, " step=0.25 ");
	TwAddVarRW(bar, "Light2 Position", TW_TYPE_DIR3D, &p2Light.position, " step=0.25 ");
	TwAddVarRW(bar, "Light1 Intensity", TW_TYPE_DOUBLE, &p1Light.intensity, "min=0.0 max=1.0 step=0.1 ");
	TwAddVarRW(bar, "Light2 Intensity", TW_TYPE_DOUBLE, &p2Light.intensity, "min=0.0 max=1.0 step=0.1 ");
	TwAddVarRW(bar, "Ambient Light Intensity", TW_TYPE_DOUBLE, &ambLight.intensity, "min=0.0 max=1.0 step=0.1 ");
	TwAddVarRW(bar, "Ambient Coeffecient", TW_TYPE_COLOR32, &objMaterial.kAmbient, " step=1 ");
	TwAddVarRW(bar, "Diffuse Coeffecient", TW_TYPE_COLOR32, &objMaterial.kDiffuse, " step=1 ");
	TwAddVarRW(bar, "Specular Coeffecient", TW_TYPE_COLOR32, &objMaterial.kSpecular, " step=1 ");
	TwAddVarRW(bar, "Power Coeffecient", TW_TYPE_DOUBLE, &objMaterial.normSpecular, "min=0.0  step=1 ");
	TwAddVarRW(bar, "World Coordinates", TW_TYPE_BOOLCPP, &showWorldCoords, NULL);
	TwAddVarRW(bar, "Object Coordinates", TW_TYPE_BOOLCPP, &showObjCoords, NULL);
	TwAddVarRW(bar, "Bounding Box", TW_TYPE_BOOLCPP, &showBox, NULL);
	TwAddVarRW(bar, "Vertex Normals", TW_TYPE_BOOLCPP, &showNormals, NULL);
	TwAddVarRW(bar, "Show Wireframe", TW_TYPE_BOOLCPP, &showWireframe, NULL);
	TwAddVarCB(bar, "Projection Type", TW_TYPE_BOOLCPP, setProj, getProj, NULL, NULL);
	TwAddVarCB(bar, "Scale", TW_TYPE_DOUBLE, setScale, getScale, NULL, " min=-2.5 max=2.5 step=0.01 keyIncr=+ keyDecr=- help='Scale the object (1=original size).' ");
	TwAddButton(bar, "Look At", lookAt, NULL, " label='Look at object' ");
	TwAddVarRW(bar, "Red", TW_TYPE_UINT32, &red, " min=0 max=255 step=1 keyIncr=c keyDecr=C help='Color' ");
	TwAddVarRW(bar, "Green", TW_TYPE_UINT32, &green, " min=0 max=255 step=1 keyIncr=v keyDecr=V help='Color' ");
	TwAddVarRW(bar, "Blue", TW_TYPE_UINT32, &blue, " min=0 max=255 step=1 keyIncr=b keyDecr=B help='Color' ");
	TwAddVarRW(bar, "Alpha", TW_TYPE_UINT32, &alpha, " min=0 max=255 step=1 keyIncr=v keyDecr=V help='Color' ");
	TwAddVarRO(bar, "time (us)", TW_TYPE_UINT32, &ElapsedMicroseconds.LowPart, "help='shows the drawing time in micro seconds'");
	


	// Call the GLUT main loop
	glutMainLoop();

	return 0;
}


void TW_CALL loadOBJModel(void *data)
{
	std::wstring str = getOpenFileName();

	objScene.m_faces.clear();
	objScene.m_normals.clear();
	objScene.m_points.clear();
	objScene.m_textureCoordinates.clear();

	bool result = objScene.load_file(str);

	if (result)
	{
		std::cout << "The obj file was loaded successfully" << std::endl;
	}
	else
	{
		std::cerr << "Failed to load obj file" << std::endl;
	}

	std::cout << "The number of vertices in the model is: " << objScene.m_points.size() << std::endl;
	std::cout << "The number of triangles in the model is: " << objScene.m_faces.size() << std::endl;

	obj.mesh.importObject(objScene);
	obj.model = Matrix4::identity();
	obj.calcBox();
}
	


//do not change this function unless you really know what you are doing!
void initGraphics(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(960, 640);
	glutCreateWindow("Computer Graphics HW2 using AntTweakBar and freeGlut");
	glutCreateMenu(NULL);

	// Initialize OpenGL
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.0, 0.0);

	// Initialize GLEW
	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		assert(0);
		return;
	}
}




//this will make sure that integer coordinates are mapped exactly to corresponding pixels on screen
void glUseScreenCoordinates(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// Callback function called by GLUT to render screen
void Display()
{



    glClearColor(0, 0, 0, 1); //background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//time measuring - don't delete
	QueryPerformanceCounter(&StartingTime);
	
	drawScene(Matrix4::rotateQuat(objQuat)*Matrix4::rotateQuat(worldQuat),objMaterial,Vector4(camPos[0],camPos[1],camPos[2]),shader,ambLight,p1Light,p2Light,obj, cam, proj,zBufferMatrix,pixelBufferMatrix, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), red + (green << 8) + (blue << 16) + (alpha << 24), showWorldCoords, showObjCoords, showBox, showNormals,showWireframe);
	
	//time measuring - don't delete
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

	// Draw tweak bars
	TwDraw();

	//swap back and front frame buffers
	glutSwapBuffers();
}


// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	glUseScreenCoordinates(width, height);

	//////////////////////////////////////
	///////add your reshape code here/////////////
	windowHeight = height;
	windowWidth = width;
	zBufferMatrix.resize(width*height);
	pixelBufferMatrix.resize(width*height*2);
	for (auto & z : zBufferMatrix) 
		z = INFINITY;

	for (auto & pix : pixelBufferMatrix)
		pix = 0;
	
	if (!projType)
	{
		double t = tan(2*M_PI*cam.fov/720)*cam.n;
		double b = -t;
		double r = t * glutGet(GLUT_WINDOW_WIDTH) / (double)glutGet(GLUT_WINDOW_HEIGHT);
		double l = -r;
		proj = Matrix4(
			Vector4(2.0*cam.n / (r - l), 0, 0, 0),
			Vector4(0, 2.0*cam.n / (t - b), 0, 0),
			Vector4((r + l) / (r - l), (t + b) / (t - b), -(cam.f + cam.n) / (cam.f - cam.n), -1),
			Vector4(0, 0, -2.0*cam.f*cam.n / (cam.f - cam.n), 0.0)
		);
	}
	else
	{
		double t = tan(2*M_PI*cam.fov/720)*cam.n * 15000;
		double b = -t;
		double r = t * glutGet(GLUT_WINDOW_WIDTH) / (double)glutGet(GLUT_WINDOW_HEIGHT);
		double l = -r;
		proj = Matrix4(
			Vector4(2.0 / (r - l), 0, 0, 0),
			Vector4(0, 2.0 / (t - b), 0, 0),
			Vector4(0, 0, -2.0 / (cam.f - cam.n), 0),
			Vector4(-(r + l) / (r - l), -(t + b) / (t - b), -(cam.f + cam.n) / (cam.f - cam.n), 1)
		);
	}

	//////////////////////////////////////

    // Send the new window size to AntTweakBar
    TwWindowSize(width, height);
	glutPostRedisplay();
}



void MouseButton(int button, int state, int x, int y)
{
	TwEventMouseButtonGLUT(button, state, x, y);
	glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	TwEventMouseMotionGLUT(x, y);
	glutPostRedisplay();
}

void PassiveMouseMotion(int x, int y)
{
	TwEventMouseMotionGLUT(x, y);
}


void Keyboard(unsigned char k, int x, int y)
{
	TwEventKeyboardGLUT(k, x, y);
	glutPostRedisplay();
}


void Special(int k, int x, int y)
{
	TwEventSpecialGLUT(k, x, y);
	glutPostRedisplay();
}


// Function called at exit
void Terminate(void)
{ 
    TwTerminate();
}


void updateMatrix(int type)
{
	if (type == DATA_CAMERA)
	{
		cam.view = Matrix4::scale(g_Scale, g_Scale, g_Scale) * Matrix4::translate(-camPos[0], -camPos[1], -camPos[2]) * Matrix4::rotateQuat(camQuat).transpose();
	}
	else
	{
		
		obj.model = Matrix4::scale(g_Scale, g_Scale, g_Scale) * Matrix4::rotateQuat(objQuat) * Matrix4::translate(objPos[0], objPos[1], objPos[2]) * Matrix4::rotateQuat(worldQuat) * Matrix4::translate(worldPos[0], worldPos[1], worldPos[2]);
	}
}
void TW_CALL setPos(const void* value, void* clientData)
{
	const double* pos = (const double*)value;
	Data& data = *((Data*)clientData);
	data.pos[0] = pos[0];
	data.pos[1] = pos[1];
	data.pos[2] = pos[2];
	updateMatrix(data.type);
}
void TW_CALL getPos(void* value, void* clientData)
{
	double* pos = (double*)value;
	Data& data = *((Data*)clientData);
	pos[0] = data.pos[0];
	pos[1] = data.pos[1];
	pos[2] = data.pos[2];
}

void TW_CALL setQuat(const void* value, void* clientData)
{
	const double* quat = (const double*)value;
	Data& data = *((Data*)clientData);
	data.quat[0] = quat[0];
	data.quat[1] = quat[1];
	data.quat[2] = quat[2];
	data.quat[3] = quat[3];
	updateMatrix(data.type);
}
void TW_CALL getQuat(void* value, void* clientData)
{
	double* quat = (double*)value;
	Data& data = *((Data*)clientData);
	quat[0] = data.quat[0];
	quat[1] = data.quat[1];
	quat[2] = data.quat[2];
	quat[3] = data.quat[3];
}
void TW_CALL setProj(const void* value, void* clientData)
{
	bool type = *(const bool*)value;
	projType = type;
	if (!type)
	{
		double t = tan(2 * M_PI*cam.fov / 720)*cam.n;
		double b = -t;
		double r = t * glutGet(GLUT_WINDOW_WIDTH) / (double)glutGet(GLUT_WINDOW_HEIGHT);
		double l = -r;
		proj = Matrix4(
			Vector4(2.0*cam.n / (r - l), 0, 0, 0),
			Vector4(0, 2.0*cam.n / (t - b), 0, 0),
			Vector4((r + l) / (r - l), (t + b) / (t - b), -(cam.f + cam.n) / (cam.f - cam.n), -1),
			Vector4(0, 0, -2.0*cam.f*cam.n / (cam.f - cam.n), 0.0)
		);
	}
	else
	{
		double t = tan(2 * M_PI*cam.fov / 720)*cam.n * 15000;
		double b = -t;
		double r = t * glutGet(GLUT_WINDOW_WIDTH) / (double)glutGet(GLUT_WINDOW_HEIGHT);
		double l = -r;
		proj = Matrix4(
			Vector4(2.0 / (r - l), 0, 0, 0),
			Vector4(0, 2.0 / (t - b), 0, 0),
			Vector4(0, 0, -2.0 / (cam.f - cam.n), 0),
			Vector4(-(r + l) / (r - l), -(t + b) / (t - b), -(cam.f + cam.n) / (cam.f - cam.n), 1)
		);
	}
}
void TW_CALL getProj(void* value, void* clientData)
{
	bool& type = *(bool*)value;
	type = projType;
}void TW_CALL setScale(const void* value, void* clientData)
{
	double scale = *(const double*)value;
	if (scale >= -2.5 && scale <= 2.5)
	{
		g_Scale = scale;
		updateMatrix(DATA_OBJECT);
	}
}
void TW_CALL getScale(void* value, void* clientData)
{
	double& scale = *(double*)value;
	scale = g_Scale;
}
void TW_CALL lookAt(void* data)
{
	Vector4 at(objPos[0] + worldPos[0], objPos[1] + worldPos[1], objPos[2] + worldPos[2], 0);
	Vector4 eye(camPos[0], camPos[1], camPos[2]);
	Vector4 up(0, 1, 0);
	Vector4 n = (eye - at).normalize();
	Vector4 u = up.crossEuclid(n).normalize();
	Vector4 v = n.crossEuclid(u).normalize();
	n.w = 0;
	u.w = 0;
	v.w = 0;
	Vector4 t;
	Matrix4 c(u, v, n, t);
	cam.view = Matrix4::scale(g_Scale, g_Scale, g_Scale) * Matrix4::translate(-camPos[0], -camPos[1], -camPos[2]) * c.transpose();
}
