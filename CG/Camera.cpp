#include "Camera.h"

Camera::Camera(const Matrix4& view, double n, double f, double fov, bool ortho)
{
	this->view = view;
	this->n = n;
	this->f = f;
	this->fov = fov;
	this->ortho = ortho;
}