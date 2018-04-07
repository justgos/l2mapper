#include "math.h"

_vector2::_vector2()
{
	x = 0.0f;
	y = 0.0f;
}

_vector2::_vector2(float tx, float ty)
{
	x = tx;
	y = ty;
}

_vector2::~_vector2()
{
}

_vector2 _vector2::operator-(const _vector2 tv)
{
	return _vector2(x - tv.x, y - tv.y);
}

_vector3::_vector3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

_vector3::_vector3(double tx, double ty, double tz)
{
	x = tx;
	y = ty;
	z = tz;
}

_vector3::~_vector3()
{
}

_vector3& _vector3::operator=(const _vector3 tv)
{
	x = tv.x;
	y = tv.y;
	z = tv.z;
	return *this;
}

_vector3 _vector3::operator*(const double t)
{
	return _vector3(x * t, y * t, z * t);
}

_vector3 _vector3::operator/(const double t)
{
	return _vector3(x / t, y / t, z / t);
}

_vector3 _vector3::operator+(const _vector3 tv)
{
	return _vector3(x + tv.x, y + tv.y, z + tv.z);
}

_vector3& _vector3::operator+=(const _vector3& tv)
{
	x += tv.x;
	y += tv.y;
	z += tv.z;
	return *this;
}

_vector3 _vector3::operator-(const _vector3 tv)
{
	return _vector3(x - tv.x, y - tv.y, z - tv.z);
}

double _vector3::Length()
{
	return sqrt(x * x + y * y + z * z);
}

_vector3 _vector3::Cross(const _vector3 tv)
{
	return _vector3((y * tv.z) - (z * tv.y),
					(z * tv.x) - (x * tv.z),
					(x * tv.y) - (y * tv.x));
}

_vector3 _vector3::Normalize()
{
	return _vector3(x / Length(), y / Length(), z / Length());
	/*float len = Length();
	x /= len;
	y /= len;
	z /= len;

	return *this;*/
}

float _vector3::Dot(const _vector3 tv)
{
	return x * tv.x + y * tv.y + z * tv.z;
}

void _vector3::SetLength(double l)
{
	double d = l / Length();
	x *= d;
	y *= d;
	z *= d;
}

_matrix _matrix::operator+(const _matrix pm)
{
	float Rdata[16];
	for(int k = 0; k < 16; k++)
		Rdata[k] = data[k] + pm.data[k];
	return _matrix(Rdata);
}

_matrix _matrix::operator-(const _matrix pm)
{
	float Rdata[16];
	for(int k = 0; k < 16; k++)
   		Rdata[k] = data[k] - pm.data[k];
	return _matrix(Rdata);
}

_matrix _matrix::operator*(const _matrix pm)
{
	float Rdata[16];
	for(int row = 0; row < 16; row += 4)
		for(int col = 0; col < 4; col++)
		{
			int l = 0;
			Rdata[row + col] = 0;
			for(int k = 0; k < 4; k++, l += 4)
				Rdata[row + col] += data[row + k] * pm.data[l + col];
		}
	return _matrix(Rdata);
}

_matrix& _matrix::operator*=(const _matrix &pm)
{
	float Rdata[16];
	for(int row = 0; row < 16; row += 4)
		for(int col = 0; col < 4; col++)
		{
			int l = 0;
			Rdata[row + col] = 0;
			for(int k = 0; k < 4; k++, l += 4)
				Rdata[row + col] += data[row + k] * pm.data[l + col];
		}
	memcpy(data, Rdata, 16*sizeof(float));
	return *this;
}

_matrix _matrix::operator*(const float pT)
{
	float Rdata[16];
	memcpy(Rdata, data, 16 * sizeof(float));
	for(int k = 0; k < 16; k++)
		Rdata[k] *= pT;
	return _matrix(Rdata);
}

_vector3 _matrix::operator*(const _vector3& pV)
{
	float vdata[4], pvdata[4];
	pvdata[0] = pV.x;
	pvdata[1] = pV.y;
	pvdata[2] = pV.z;
	pvdata[3] = 0;
	for (int col = 0; col < 4; col++)
	{
		int k = 0;
		vdata[col] = 0;
		for (int row = 0; row < 4; row++, k+=4)
			vdata[col] += data[k + col] * pvdata[row];
	}
	vdata[0] += data[12];
	vdata[1] += data[13];
	vdata[2] += data[14];
	return _vector3(vdata[0], vdata[1], vdata[2]);
}

void _matrix::ScalingMatrix(float x, float y, float z)
{
	Identity();
	data[0] = x;
	data[5] = y;
	data[10] = z;
}

void _matrix::TranslationMatrix(float x, float y, float z)
{
	Identity();
	data[12] = x;
	data[13] = y;
	data[14] = z;
}

void _matrix::rotationX(float angle)
{
	//Indentity();
	data[5] = cos(angle);
	data[6] = sin(angle);
	data[9] = -sin(angle);
	data[10] = cos(angle);
}

void _matrix::rotationY(float angle)
{
	//Indentity();
	data[0] = cos(angle);
	data[2] = -sin(angle);
	data[8] = sin(angle);
	data[10] = cos(angle);
}

void _matrix::rotationZ(float angle)
{
	//Indentity();
	data[0] = cos(angle);
	data[1] = -sin(angle);
	data[4] = sin(angle);
	data[5] = cos(angle);
}

void _matrix::QuaternionMatrix(float x, float y, float z, float w)
{
    float xx = x*x; float yy = y*y; float zz = z*z;
    float xy = x*y; float xz = x*z; float yz = y*z;
    float wx = w*x; float wy = w*y; float wz = w*z;

    data[0] = 1 - 2 * ( yy + zz );
    data[1] =     2 * ( xy - wz );
    data[2] =     2 * ( xz + wy );

    data[4] =     2 * ( xy + wz );
    data[5] = 1 - 2 * ( xx + zz );
    data[6] =     2 * ( yz - wx );

    data[8] =     2 * ( xz - wy );
    data[9] =     2 * ( yz + wx );
    data[10] = 1 - 2 * ( xx + yy );

    data[3] = data[7] = data[11] = 0.0f;
    data[12] = data[13] = data[14] = 0.0f;
    data[15] = 1.0f;
}

float _quaternion::qACos(float pValue)
{
	if ( -1.0f < pValue )
	{
		if ( pValue < 1.0f )
			return (float)acos(pValue);
		else
			return 0.0f;
	}
	else
		return _PI;
}

_quaternion _quaternion::operator+(_quaternion pq)
{
	float rdata[4];
	rdata[0] = pq.data[0] + data[0];
	rdata[1] = pq.data[1] + data[1];
	rdata[2] = pq.data[2] + data[2];
	rdata[3] = pq.data[3] + data[3];
	return _quaternion(rdata);
}

_quaternion _quaternion::operator*(float pT)
{
	float rdata[4];
	rdata[0] = pT * data[0];
	rdata[1] = pT * data[1];
	rdata[2] = pT * data[2];
	rdata[3] = pT * data[3];
	return _quaternion(rdata);
}

void _quaternion::fromEuler(float x, float y, float z)
{
	float angle = x * 0.5f;
	float sr = sin(angle);
	float cr = cos(angle);

	angle = y * 0.5f;
	float sp = sin(angle);
	float cp = cos(angle);

	angle = z * 0.5f;
	float sy = sin(angle);
	float cy = cos(angle);

	float cpcy = cp * cy;
	float spcy = sp * cy;
	float cpsy = cp * sy;
	float spsy = sp * sy;

	data[0] = sr * cpcy - cr * spsy;
	data[1] = cr * spcy + sr * cpsy;
	data[2] = cr * cpsy - sr * spcy;
	data[3] = cr * cpcy + sr * spsy;

	normalize();
}

void _quaternion::normalize()
{
	float l = data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3];
	if(l == 1)
		return;

	l = 1.0f / sqrt(l);
	data[0] *= l;
	data[1] *= l;
	data[2] *= l;
	data[3] *= l;
}

float _quaternion::Dot(_quaternion &pq)
{
	return ((data[0] * pq.data[0]) + (data[1] * pq.data[1]) + (data[2] * pq.data[2]) + (data[3] * pq.data[3]));
}

_quaternion _quaternion::Slerp(float pT, _quaternion &pq)
{
	//We calculate the angle spread between both quaternions
	float AngleCos = pq.Dot(*this);
	float Angle = qACos(AngleCos); //see the function ACos above

	if (Angle < MINFLOAT)
		return _quaternion(*this);
	//We calculate the interpolated angle and deduce the resulting quaternion
	float InvAngleSin = (float)(1.0f / sin(Angle));

	float Coeff0 = sin((1-pT) * Angle) * InvAngleSin;
	float Coeff1 = sin(pT * Angle) * InvAngleSin;
	return _quaternion((*this * Coeff0)+(pq * Coeff1));
}

void _quaternion::AxisAngle(float x, float y, float z, float w)
{
	float angle = float((w / 180.0f) * _PI);

	float result = (float)sin( angle / 2.0f );
		
	data[3] = (float)cos( angle / 2.0f );

	data[0] = float(x * result);
	data[1] = float(y * result);
	data[2] = float(z * result);
}

void math::tangentVectors(const _vector3 &v0, const _vector3 &v1, const _vector3 &v2, const _vector2 &t0, const _vector2 &t1, const _vector2 &t2, _vector3 &sdir, _vector3 &tdir, _vector3 &normal)
{
	_vector3 dv0 = (_vector3)v1 - v0;
	_vector3 dv1 = (_vector3)v2 - v0;

	_vector2 dt0 = (_vector2)t1 - t0;
	_vector2 dt1 = (_vector2)t2 - t0;

	float r = 1.0f / (dt0.x * dt1.y - dt1.x * dt0.y);
	sdir = _vector3(dt1.y * dv0.x - dt0.y * dv1.x, dt1.y * dv0.y - dt0.y * dv1.y, dt1.y * dv0.z - dt0.y * dv1.z) * r;
	tdir = _vector3(dt0.x * dv1.x - dt1.x * dv0.x, dt0.x * dv1.y - dt1.x * dv0.y, dt0.x * dv1.z - dt1.x * dv0.z) * r;
}

_vector3 math::TransformCoord(const _vector3 vec, const _matrix mat)
{
	_vector3 tmp;
	tmp.x = mat.data[0] * vec.x + mat.data[4] * vec.y + mat.data[8] * vec.z + mat.data[12]; // + mat.data[3]
	tmp.y = mat.data[1] * vec.x + mat.data[5] * vec.y + mat.data[9] * vec.z + mat.data[13]; //
	tmp.z = mat.data[2] * vec.x + mat.data[6] * vec.y + mat.data[10] * vec.z + mat.data[14]; //  + mat.data[14] ...
	return tmp;
}

_matrix math::MatrixInverse(const _matrix m)
{
	_matrix ret;

	float p00 = m.data[10] * m.data[15];
	float p01 = m.data[14] * m.data[11];
	float p02 = m.data[6] * m.data[15];
	float p03 = m.data[14] * m.data[7];
	float p04 = m.data[6] * m.data[11];
	float p05 = m.data[10] * m.data[7];
	float p06 = m.data[2] * m.data[15];
	float p07 = m.data[14] * m.data[3];
	float p08 = m.data[2] * m.data[11];
	float p09 = m.data[10] * m.data[3];
	float p10 = m.data[2] * m.data[7];
	float p11 = m.data[6] * m.data[3];

	ret.data[0] = (p00 * m.data[5] + p03 * m.data[9] + p04 * m.data[13]) - (p01 * m.data[5] + p02 * m.data[9] + p05 * m.data[13]);
	ret.data[1] = (p01 * m.data[1] + p06 * m.data[9] + p09 * m.data[13]) - (p00 * m.data[1] + p07 * m.data[9] + p08 * m.data[13]);
	ret.data[2] = (p02 * m.data[1] + p07 * m.data[5] + p10 * m.data[13]) - (p03 * m.data[1] + p06 * m.data[5] + p11 * m.data[13]);
	ret.data[3] = (p05 * m.data[1] + p08 * m.data[5] + p11 * m.data[9]) - (p04 * m.data[1] + p09 * m.data[5] + p10 * m.data[9]);
	ret.data[4] = (p01 * m.data[4] + p02 * m.data[8] + p05 * m.data[12]) - (p00 * m.data[4] + p03 * m.data[8] + p04 * m.data[12]);
	ret.data[5] = (p00 * m.data[0] + p07 * m.data[8] + p08 * m.data[12]) - (p01 * m.data[0] + p06 * m.data[8] + p09 * m.data[12]);
	ret.data[6] = (p03 * m.data[0] + p06 * m.data[4] + p11 * m.data[12]) - (p02 * m.data[0] + p07 * m.data[4] + p10 * m.data[12]);
	ret.data[7] = (p04 * m.data[0] + p09 * m.data[4] + p10 * m.data[8]) - (p05 * m.data[0] + p08 * m.data[4] + p11 * m.data[8]);

	float q00 = m.data[8] * m.data[13];
	float q01 = m.data[12] * m.data[9];
	float q02 = m.data[4] * m.data[13];
	float q03 = m.data[12] * m.data[5];
	float q04 = m.data[4] * m.data[9];
	float q05 = m.data[8] * m.data[5];
	float q06 = m.data[0] * m.data[13];
	float q07 = m.data[12] * m.data[1];
	float q08 = m.data[0] * m.data[9];
	float q09 = m.data[8] * m.data[1];
	float q10 = m.data[0] * m.data[5];
	float q11 = m.data[4] * m.data[1];

	ret.data[8] = (q00 * m.data[7] + q03 * m.data[11] + q04 * m.data[15]) - (q01 * m.data[7] + q02 * m.data[11] + q05 * m.data[15]);
	ret.data[9] = (q01 * m.data[3] + q06 * m.data[11] + q09 * m.data[15]) - (q00 * m.data[3] + q07 * m.data[11] + q08 * m.data[15]);
	ret.data[10] = (q02 * m.data[3] + q07 * m.data[7] + q10 * m.data[15]) - (q03 * m.data[3] + q06 * m.data[7] + q11 * m.data[15]);
	ret.data[11] = (q05 * m.data[3] + q08 * m.data[7] + q11 * m.data[11]) - (q04 * m.data[3] + q09 * m.data[7] + q10 * m.data[11]);
	ret.data[12] = (q02 * m.data[10] + q05 * m.data[14] + q01 * m.data[6]) - (q04 * m.data[14] + q00 * m.data[6] + q03 * m.data[10]);
	ret.data[13] = (q08 * m.data[14] + q00 * m.data[2] + q07 * m.data[10]) - (q06 * m.data[10] + q09 * m.data[14] + q01 * m.data[2]);
	ret.data[14] = (q06 * m.data[6] + q11 * m.data[14] + q03 * m.data[2]) - (q10 * m.data[14] + q02 * m.data[2] + q07 * m.data[6]);
	ret.data[15] = (q10 * m.data[10] + q04 * m.data[2] + q09 * m.data[6]) - (q08 * m.data[6] + q11 * m.data[10] + q05 * m.data[2]);

	return ret * (1.0f / (m.data[0] * ret.data[0] + m.data[4] * ret.data[1] + m.data[8] * ret.data[2] + m.data[12] * ret.data[3]));
}

_matrix math::MatrixTranspose(const _matrix m)
{
	_matrix ret;

	ret[0] = m[0];
	ret[1] = m[4];
	ret[2] = m[8];
	ret[3] = m[12];

	ret[4] = m[1];
	ret[5] = m[5];
	ret[6] = m[9];
	ret[7] = m[13];

	ret[8] = m[2];
	ret[9] = m[6];
	ret[10] = m[10];
	ret[11] = m[14];

	ret[12] = m[3];
	ret[13] = m[7];
	ret[14] = m[11];
	ret[15] = m[15];

	return ret;
}

void math::Exchangei(int *a, int *b)
{
	*a += *b;
	*b = *a - *b;
	*a -= *b;
}

bool math::checkIntersectionRayTriangle(_vector3 morig, _vector3 mdir, _vector3 v0, _vector3 v1, _vector3 v2,
				float *t, float *u, float *v)
{
	// Find vectors for two edges sharing vert0
    _vector3 edge1 = v1 - v0;
    _vector3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    _vector3 pvec;
	//pvec = dir;
	pvec = mdir.Cross(edge2);
    //D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
   // FLOAT det = D3DXVec3Dot( &edge1, &pvec );
	float det = edge1.Dot(pvec);

    _vector3 tvec;
    if( det > 0 )
    {
        tvec = morig - v0;
    }
    else
    {
        tvec = v0 - morig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
	*u = tvec.Dot(pvec);
    if( *u < 0.0f || *u > det )
        return false;

    // Prepare to test V parameter
    _vector3 qvec;
    //D3DXVec3Cross( &qvec, &tvec, &edge1 );
	qvec = tvec.Cross(edge1);

    // Calculate V parameter and test bounds
	*v = mdir.Dot(qvec);
    if( *v < 0.0f || *u + *v > det )
        return false;

    // Calculate t, scale parameters, ray intersects triangle
	*t = edge2.Dot(qvec);
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}
