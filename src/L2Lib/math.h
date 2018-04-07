#pragma once
#include "UBase.h"

#define MINFLOAT 1.17549435E-38F

#define _PI 3.141592653589793

class DLLEXPORT _vector2
{
public:
	_vector2 operator-(const _vector2 tv);
public:
	float x,y;
public:
	_vector2();
	_vector2(float tx, float ty);
	~_vector2();
};

class DLLEXPORT _vector3
{
public:
	_vector3& operator=(const _vector3 tv);
	bool operator==(const _vector3 tv) {return x == tv.x && y == tv.y && z == tv.z;};
	bool operator!=(const _vector3 tv) {return !(x == tv.x && y == tv.y && z == tv.z);};
	_vector3 operator*(const double t);
	_vector3 operator/(const double t);
	_vector3 operator+(const _vector3 tv);
	_vector3& operator+=(const _vector3& tv);
	_vector3 operator-(const _vector3 tv);
	double Length();
	_vector3 Cross(const _vector3 tv);
	_vector3 Normalize();
	void SetLength(double l);
	float Dot(const _vector3 tv);
public:
	float x,y,z;
public:
	_vector3();
	_vector3(double tx, double ty, double tz);
	~_vector3();
};

class DLLEXPORT _matrix
{
public:
	float data[16];
	_matrix(){memset(data, 0, 16 * sizeof(float)); Identity();};
	_matrix(const _matrix &pm){memcpy(data, pm.data, 16 * sizeof(float));};
	_matrix(const float* pT){memcpy(data, pT, 16 * sizeof(float));};
	void Identity(){memset(data, 0, 16 * sizeof(float)); data[0]=1.0f; data[5]=1.0f; data[10]=1.0f; data[15]=1.0f;};
	_matrix& operator=(const _matrix &pm){memcpy(data, pm.data, 16 * sizeof(float));return *this;};
	_matrix operator+(const _matrix pm);
	_matrix operator-(const _matrix pm);
	_matrix operator*(const _matrix pm);
	_matrix& operator*=(const _matrix &pm);
	_matrix operator*(const float pT);
	_vector3 operator*(const _vector3& pV);

	float operator[](int index)const{return data[index];};
	//float operator[int, int](int index, int index1) {return data[index * 4 + index1];};
	float& operator[](int index){return data[index];};

	void ScalingMatrix(float x, float y, float z);
	void TranslationMatrix(float x, float y, float z);
	void rotationX(float angle);
	void rotationY(float angle);
	void rotationZ(float angle);
	void QuaternionMatrix(float x, float y, float z, float w);
};

class DLLEXPORT _quaternion
{
public:
	float data[4]; // 0 - x, 1 - y, 2 - z, 3 - w
	_quaternion(void){memset(data, 0, 4 * sizeof(float));};
	_quaternion(const _quaternion &pq){memcpy(data, pq.data, 4 * sizeof(float));};
	_quaternion(const float* pT){memcpy(data, pT, 4 * sizeof(float));};
	void Zero(void){memset(data, 0, 4 * sizeof(float));};
	float operator[](int index)const{return data[index];};
	float& operator[](int index){return data[index];};
	_quaternion& operator=(const _quaternion &pq){memcpy(data, pq.data, 4 * sizeof(float));return *this;};
	_quaternion operator+(_quaternion pq);
	_quaternion operator*(_quaternion pq);
	_quaternion operator*(float pT);
	void fromEuler(float x, float y, float z);
	void normalize();
	float Dot(_quaternion &pq);
	_quaternion Slerp(float pT, _quaternion &pq);
	void AxisAngle(float x, float y, float z, float w);
private:
	float qACos(float pValue);
};

class DLLEXPORT math
{
public:
	void tangentVectors(const _vector3 &v0, const _vector3 &v1, const _vector3 &v2, const _vector2 &t0, const _vector2 &t1, const _vector2 &t2, _vector3 &sdir, _vector3 &tdir, _vector3 &normal);
	_vector3 TransformCoord(const _vector3 vec, const _matrix mat);
	_matrix MatrixInverse(const _matrix m);
	_matrix MatrixTranspose(const _matrix m);
	void Exchangei(int *a, int *b);
	bool checkIntersectionRayTriangle(_vector3 morig, _vector3 mdir, _vector3 v0, _vector3 v1, _vector3 v2,
											float *t, float *u, float *v);
};