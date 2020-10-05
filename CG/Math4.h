#pragma once

#include <iostream>

struct Vector4
{
	double x;
	double y;
	double z;
	double w;

	/* constructors */
	Vector4(double x = 0, double y = 0, double z = 0, double w = 1)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};

	/* functions and operators*/
	double& operator[](int index)
	{
		return (&x)[index];
	}
	const double& operator[](int index) const
	{
		return (&x)[index];
	}

	Vector4     operator-() const
	{
		Vector4 temp;
		temp.x = this->x * (-1.0);
		temp.y = this->y * (-1.0);
		temp.z = this->z * (-1.0);
		temp.w = this->w * (-1.0);
		return temp;
	}

	Vector4   operator+(const Vector4& rightHandSide) const
	{
		Vector4 temp;
		temp.x = x + rightHandSide.x;
		temp.y = y + rightHandSide.y;
		temp.z = z + rightHandSide.z;
		temp.w = w + rightHandSide.w;

		return temp;
	}

	Vector4&   operator+=(const Vector4& rightHandSide)
	{
		this->x += rightHandSide.x;
		this->y += rightHandSide.y;
		this->z += rightHandSide.z;
		this->w += rightHandSide.w;

		return *this;
	}
	Vector4 operator-(const Vector4& rightHandSide) const
	{
		Vector4 temp;
		temp.x = x - rightHandSide.x;
		temp.y = y - rightHandSide.y;
		temp.z = z - rightHandSide.z;
		temp.w = w - rightHandSide.w;

		return temp;
	}
	Vector4&  operator-=(const Vector4& rightHandSide)
	{

		this->x -= rightHandSide.x;
		this->y -= rightHandSide.y;
		this->z -= rightHandSide.z;
		this->w -= rightHandSide.w;

		return *this;
	}

	Vector4     operator*(const double scalar) const
	{
		Vector4 temp;
		temp.x = scalar * x;
		temp.y = scalar * y;
		temp.z = scalar * z;
		temp.w = scalar * w;
		return temp;
	}

	friend Vector4 operator*(const double scalar, const Vector4& rightHandSide)
	{
		return rightHandSide * scalar;
	}

	Vector4 euclid() const
	{
		Vector4 temp;
		temp.x = this->x / this->w;
		temp.y = this->y / this->w;
		temp.z = this->z / this->w;
		return temp;
	}

	Vector4     crossEuclid(const Vector4& rightHandSide) const /*euclidean cross product*/
	{
		Vector4 left = this->euclid();
		Vector4 right = rightHandSide.euclid();
		Vector4 temp;
		temp.x = left.y * right.z - left.z * right.y;
		temp.y = left.z * right.x - left.x * right.z;
		temp.z = left.x * right.y - left.y * right.x;
		return temp;
	}

	double distance() const
	{
		Vector4 temp = this->euclid();
		return sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
	}

	Vector4 normalize() const
	{
		Vector4 norm = *this * (1.0 / this->distance());
		norm.w = 1;
		return norm;
	}

	double       dotEuclid(const Vector4& vec) const /*euclidean dot product */
	{
		Vector4 left = this->euclid();
		Vector4 right = vec.euclid();
		return left.x * right.x + left.y * right.y + left.z * right.z;
	}
	double       operator*(const Vector4& vec) const /*homogenus dot product */
	{
		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector4& vec)
	{
		os << "(" << vec.x << "," << vec.y << "," << vec.z << "," << vec.w << ")";
	}

};


struct Matrix4
{
	Vector4 mat[4];

	Matrix4() /*identity matrix for default constructor*/
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				mat[i][j] = 0;
			}
	}
	Matrix4(Vector4 r1, Vector4 r2, Vector4 r3, Vector4 r4)
	{
		mat[0] = r1;
		mat[1] = r2;
		mat[2] = r3;
		mat[3] = r4;
	}
	/* functions and operators*/

	Vector4& operator[](int index)
	{
		return mat[index];
	}

	const Vector4& operator[](int index) const
	{
		return mat[index];
	}

	static Matrix4 identity()
	{
		Matrix4 temp(
			Vector4(1, 0, 0, 0),
			Vector4(0, 1, 0, 0),
			Vector4(0, 0, 1, 0),
			Vector4(0, 0, 0, 1)
		);
		return temp;
	}

	static Matrix4 scale(double x, double y, double z)
	{
		Matrix4 temp(
			Vector4(x, 0, 0, 0),
			Vector4(0, y, 0, 0),
			Vector4(0, 0, z, 0),
			Vector4(0, 0, 0, 1)
		);
		return temp;
	}

	static Matrix4 translate(double x, double y, double z)
	{
		Matrix4 temp(
			Vector4(1, 0, 0, 0),
			Vector4(0, 1, 0, 0),
			Vector4(0, 0, 1, 0),
			Vector4(x, y, z, 1)
		);
		return temp;
	}

	static Matrix4 rotateAxisZ(double a)
	{
		Matrix4 temp(
			Vector4(cos(a), sin(a), 0, 0),
			Vector4(-sin(a), cos(a), 0, 0),
			Vector4(0, 0, 1, 0),
			Vector4(0, 0, 0, 1)
		);
		return temp;
	}
	static Matrix4 rotateAxisY(double a)
	{
		Matrix4 temp(
			Vector4(cos(a), 0, -sin(a), 0),
			Vector4(0, 1, 0, 0),
			Vector4(sin(a), 0, cos(a), 0),
			Vector4(0, 0, 0, 1)
		);
		return temp;
	}
	static Matrix4 rotateAxisX(double a)
	{
		Matrix4 temp(
			Vector4(1, 0, 0, 0),
			Vector4(0, cos(a), sin(a), 0),
			Vector4(0, -sin(a), cos(a), 0),
			Vector4(0, 0, 0, 1)
		);
		return temp;
	}
	static Matrix4 rotate(double x, double y, double z)
	{
		return rotateAxisX(x) * rotateAxisY(y) * rotateAxisZ(z);
	}

	static Matrix4 rotateQuat(double* quat)
	{
		Matrix4 rotation;
		double yy2 = 2.0 * quat[1] * quat[1];
		double xy2 = 2.0 * quat[0] * quat[1];
		double xz2 = 2.0 * quat[0] * quat[2];
		double yz2 = 2.0 * quat[1] * quat[2];
		double zz2 = 2.0 * quat[2] * quat[2];
		double wz2 = 2.0 * quat[3] * quat[2];
		double wy2 = 2.0 * quat[3] * quat[1];
		double wx2 = 2.0 * quat[3] * quat[0];
		double xx2 = 2.0 * quat[0] * quat[0];
		rotation[0][0] = -yy2 - zz2 + 1.0f;
		rotation[0][1] = xy2 + wz2;
		rotation[0][2] = xz2 - wy2;
		rotation[0][3] = 0;
		rotation[1][0] = xy2 - wz2;
		rotation[1][1] = -xx2 - zz2 + 1.0f;
		rotation[1][2] = yz2 + wx2;
		rotation[1][3] = 0;
		rotation[2][0] = xz2 + wy2;
		rotation[2][1] = yz2 - wx2;
		rotation[2][2] = -xx2 - yy2 + 1.0f;
		rotation[2][3] = 0;
		rotation[3][0] = 0;
		rotation[3][1] = 0;
		rotation[3][2] = 0;
		rotation[3][3] = 1;
		return rotation;
	}

	Matrix4 transpose() const
	{
		Matrix4 temp;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				temp[j][i] = (*this)[i][j];
			}
		return temp;
	}

	Matrix4 operator-() const
	{
		Matrix4 temp;

		for (int i = 0; i < 4; i++)
		{
			temp[i] = -(*this)[i];
		}
		return temp;
	}

	Matrix4 operator+(const Matrix4& rightHandSide) const
	{
		Matrix4 temp;
		temp[0] = (*this)[0] + rightHandSide[0];
		temp[1] = (*this)[1] + rightHandSide[1];
		temp[2] = (*this)[2] + rightHandSide[2];
		temp[3] = (*this)[3] + rightHandSide[3];

		return temp;
	}
	Matrix4 operator-(const Matrix4& rightHandSide) const
	{
		Matrix4 temp;
		temp[0] = (*this)[0] - rightHandSide[0];
		temp[1] = (*this)[1] - rightHandSide[1];
		temp[2] = (*this)[2] - rightHandSide[2];
		temp[3] = (*this)[3] - rightHandSide[3];

		return temp;
	}


	Matrix4& operator+=(const Matrix4& rightHandSide)
	{
		(*this)[0] = (*this)[0] + rightHandSide[0];
		(*this)[1] = (*this)[1] + rightHandSide[1];
		(*this)[2] = (*this)[2] + rightHandSide[2];
		(*this)[3] = (*this)[3] + rightHandSide[3];

		return *this;
	}
	Matrix4& operator-=(const Matrix4& rightHandSide)
	{
		(*this)[0] = (*this)[0] - rightHandSide[0];
		(*this)[1] = (*this)[1] - rightHandSide[1];
		(*this)[2] = (*this)[2] - rightHandSide[2];
		(*this)[3] = (*this)[3] - rightHandSide[3];

		return *this;
	}

	Matrix4 operator*(const Matrix4& rightHandSide) const
	{
		Matrix4 temp;
		Matrix4 transRHS = rightHandSide.transpose();
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				temp[i][j] = mat[i] * transRHS[j];
			}
		return temp;
	}
	Vector4 operator*(const Vector4& rightHandSide) const
	{
		Vector4 temp;
		for (int i = 0; i < 4; i++)
		{
			temp[i] = mat[i] * rightHandSide;
		}
		return temp;
	}

	friend Vector4 operator*(const Vector4& leftHandSide, const Matrix4& rightHandSide)
	{
		Vector4 temp;
		Matrix4 transRHS = rightHandSide.transpose();
		for (int i = 0; i < 4; i++)
		{
			temp[i] = leftHandSide * transRHS[i];
		}
		return temp;
	}
};