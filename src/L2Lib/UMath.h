#pragma once

#include "UBase.h"

#undef  PI
#define PI 					(3.1415926535897932)
#define SMALL_NUMBER		(1.e-8)
#define KINDA_SMALL_NUMBER	(1.e-4)
#define BIG_NUMBER			(3.4e+38f)

// Aux constants.
#define INV_PI			(0.31830988618)
#define HALF_PI			(1.57079632679)

// Magic numbers for numerical precision.
#define DELTA			(0.00001f)
#define SLERP_DELTA		(0.0001f)

class UVector;
class UPlane;
class UBox;
class USphere;

/*-----------------------------------------------------------------------------
	FGlobalMath.
-----------------------------------------------------------------------------*/

//
// Global mathematics info.
//
class DLLEXPORT FGlobalMath
{
public:
	// Constants.
	enum {ANGLE_SHIFT 	= 2};		// Bits to right-shift to get lookup value.
	enum {ANGLE_BITS	= 14};		// Number of valid bits in angles.
	enum {NUM_ANGLES 	= 16384}; 	// Number of angles that are in lookup table.
	enum {ANGLE_MASK    =  (((1>>ANGLE_BITS)-1)>>(16-ANGLE_BITS))};

	// Basic math functions.
	__forceinline FLOAT SinTab( int i )
	{
		return TrigFLOAT[((i>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	__forceinline FLOAT CosTab( int i )
	{
		return TrigFLOAT[(((i+16384)>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	float SinFloat( float F )
	{
		return SinTab((int)((F*65536.f)/(2.f*PI)));
	}
	float CosFloat( float F )
	{
		return CosTab((int)((F*65536.f)/(2.f*PI)));
	}

	// Constructor.
	FGlobalMath();

private:
	// Tables.
	FLOAT  TrigFLOAT		[NUM_ANGLES];
};

inline INT ReduceAngle( INT Angle )
{
	return Angle & FGlobalMath::ANGLE_MASK;
};

class DLLEXPORT UVector
{
public:
	float X, Y, Z;
public:
	UPackage& Serialize(UPackage& Ar)
	{
		Ar >> X >> Y >> Z;
		return Ar;
	}
	IMPLEMENT_USERIALIZER(UVector);
public:
	__forceinline UVector()
		:	X(0), Y(0), Z(0)
	{
	}
	__forceinline UVector(float InF)
		:	X(InF), Y(InF), Z(InF)
	{
	}
	__forceinline UVector(float InX, float InY, float InZ)
		:	X(InX), Y(InY), Z(InZ)
	{
	}
	// Binary math operators.
	__forceinline UVector operator^( const UVector& V ) const
	{
		return UVector
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
	}
	__forceinline float operator|( const UVector& V ) const
	{
		return X*V.X + Y*V.Y + Z*V.Z;
	}
	friend UVector operator*( float Scale, const UVector& V )
	{
		return UVector( V.X * Scale, V.Y * Scale, V.Z * Scale );
	}
	__forceinline UVector operator+( const UVector& V ) const
	{
		return UVector( X + V.X, Y + V.Y, Z + V.Z );
	}
	__forceinline UVector operator-( const UVector& V ) const
	{
		return UVector( X - V.X, Y - V.Y, Z - V.Z );
	}
	__forceinline UVector operator*( FLOAT Scale ) const
	{
		return UVector( X * Scale, Y * Scale, Z * Scale );
	}
	UVector operator/( float Scale ) const
	{
		float RScale = 1.f/Scale;
		return UVector( X * RScale, Y * RScale, Z * RScale );
	}
	__forceinline UVector operator*( const UVector& V ) const
	{
		return UVector( X * V.X, Y * V.Y, Z * V.Z );
	}

	// Binary comparison operators.
	bool operator==( const UVector& V ) const
	{
		return X==V.X && Y==V.Y && Z==V.Z;
	}
	bool  operator!=( const UVector& V ) const
	{
		return X!=V.X || Y!=V.Y || Z!=V.Z;
	}

	// Unary operators.
	__forceinline UVector operator-() const
	{
		return UVector( -X, -Y, -Z );
	}

	// Assignment operators.
	__forceinline UVector operator+=( const UVector& V )
	{
		X += V.X; Y += V.Y; Z += V.Z;
		return *this;
	}
	__forceinline UVector operator-=( const UVector& V )
	{
		X -= V.X; Y -= V.Y; Z -= V.Z;
		return *this;
	}
	__forceinline UVector operator*=( float Scale )
	{
		X *= Scale; Y *= Scale; Z *= Scale;
		return *this;
	}
	UVector operator/=( float V )
	{
		FLOAT RV = 1.f/V;
		X *= RV; Y *= RV; Z *= RV;
		return *this;
	}
	UVector operator*=( const UVector& V )
	{
		X *= V.X; Y *= V.Y; Z *= V.Z;
		return *this;
	}
	UVector operator/=( const UVector& V )
	{
		X /= V.X; Y /= V.Y; Z /= V.Z;
		return *this;
	}
};

class DLLEXPORT URotator
{
public:
	int32 Pitch, Yaw, Roll;
public:
	UPackage& Serialize(UPackage& Ar)
	{
		Ar >> Pitch >> Yaw >> Roll;
		return Ar;
	}
	IMPLEMENT_USERIALIZER(URotator);
public:
	__forceinline URotator()
		:	Pitch(0), Yaw(0), Roll(0)
	{
	}
	__forceinline URotator(int32 InPitch, int32 InYaw, int32 InRoll)
		:	Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
	{
	}
};

class DLLEXPORT UPlane : public UVector
{
public:
	float W;
public:
	UPackage& Serialize(UPackage& Ar)
	{
		Ar >> (UVector&)(*this) >> W;
		return Ar;
	}
	IMPLEMENT_USERIALIZER(UPlane);
public:
	__forceinline UPlane()
	{
	}
	__forceinline UPlane(float InX, float InY, float InZ, float InW)
		:	UVector(InX, InY, InZ), W(InW)
	{
	}
};

class DLLEXPORT UBox
{
public:
	UVector Min;
	UVector Max;
	int8 IsValid;
public:
	class UPackage& Serialize(UPackage& Ar)
	{
		Ar >> Min >> Max >> IsValid;
		return Ar;
	}
	IMPLEMENT_USERIALIZER(UBox);
public:
	__forceinline UBox()
	{
	}
	__forceinline UBox(int32)
		:	Min(0), Max(0), IsValid(0)
	{
	}
};

class DLLEXPORT USphere : public UPlane
{
public:
	__forceinline USphere()
	{
	}
	__forceinline USphere(int32)
		:	UPlane(0, 0, 0, 0)
	{
	}
};

/*-----------------------------------------------------------------------------
	UMatrix classes.
-----------------------------------------------------------------------------*/

class DLLEXPORT UMatrix
{
public:

	// Variables.
#if __GNUG__
	FLOAT M[4][4] GCC_ALIGN(16);
	static const UMatrix Identity;
#elif _MSC_VER
	union
	{
		float M[4][4]; // __declspec(align(16))
		float Data[16];
	};
	//static const UMatrix Identity; // __declspec(align(16))
#else
#error UMatrix needs to be properly aligned
#endif 

	// Constructors.

	__forceinline UMatrix()
	{
	}

	__forceinline UMatrix(UPlane InX,UPlane InY,UPlane InZ,UPlane InW)
	{
		M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = InX.W;
		M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = InY.W;
		M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = InZ.W;
		M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = InW.W;
	}

	__forceinline UMatrix(UVector InX,UVector InY,UVector InZ,UVector InW)
	{
		M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = 0.0f;
		M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = 0.0f;
		M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = 0.0f;
		M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = 1.0f;
	}

	/*UMatrix(const UVector& Origin, const FQuat& Q)
	{
		FLOAT wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		x2 = Q.X + Q.X;  y2 = Q.Y + Q.Y;  z2 = Q.Z + Q.Z;
		xx = Q.X * x2;   xy = Q.X * y2;   xz = Q.X * z2;
		yy = Q.Y * y2;   yz = Q.Y * z2;   zz = Q.Z * z2;
		wx = Q.W * x2;   wy = Q.W * y2;   wz = Q.W * z2;

		M[0][0] = 1.0f - (yy + zz);	M[1][0] = xy - wz;				M[2][0] = xz + wy;			M[3][0] = Origin.X;
		M[0][1] = xy + wz;			M[1][1] = 1.0f - (xx + zz);		M[2][1] = yz - wx;			M[3][1] = Origin.Y;
		M[0][2] = xz - wy;			M[1][2] = yz + wx;				M[2][2] = 1.0f - (xx + yy);	M[3][2] = Origin.Z;
		M[0][3] = 0.0f;				M[1][3] = 0.0f;					M[2][3] = 0.0f;				M[3][3] = 1.0f;
	}*/

	UMatrix(const UVector& Origin, const URotator& Rotation);

	// Destructor.

	~UMatrix()
	{
	}

	void SetIdentity()
	{
		M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
		M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
		M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
		M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
	}

	// Concatenation operator.

	__forceinline UMatrix operator*(const UMatrix& Other) const
	{
		UMatrix	Result;

#if ASM
		__asm
		{
		mov		eax,[Other]
		mov		ecx,[this]
	
		movups	xmm4,[eax]				// Other.M[0][0-3]
		movups	xmm5,[eax+16]			// Other.M[1][0-3]
		movups	xmm6,[eax+32]			// Other.M[2][0-3]
		movups	xmm7,[eax+48]			// Other.M[3][0-3]

		lea		eax,[Result]

		// Begin first row of result.
		movss	xmm0,[ecx]				// M[0][0] 
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4

		movss	xmm1,[ecx+4]			// M[0][1]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm5

		movss	xmm2,[ecx+8]			// M[0][2]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm6

		addps	xmm1,xmm0				// First row done with xmm0

		movss	xmm3,[ecx+12]			// M[0][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// Begin second row of result.
		movss	xmm0,[ecx+16]			// M[1][0] 
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4
	
		addps	xmm3,xmm2				// First row done with xmm2
		
		movss	xmm2,[ecx+20]			// M[1][1]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm5

		addps	xmm3,xmm1				// First row done with xmm1
	
		movss	xmm1,[ecx+24]			// M[1][2]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm6

		movups	[eax],xmm3				// Store Result.M[0][0-3]
		// Done computing first row.
		
		addps	xmm2,xmm0				// Second row done with xmm0

		movss	xmm3,[ecx+28]			// M[1][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// Begin third row of result.
		movss	xmm0,[ecx+32]			// M[2][0] 
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4
	
		addps	xmm3,xmm1				// Second row done with xmm1
		
		movss	xmm1,[ecx+36]			// M[2][1]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm5

		addps	xmm3,xmm2				// Second row done with xmm2

		movss	xmm2,[ecx+40]			// M[2][2]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm6

		movups	[eax+16],xmm3			// Store Result.M[1][0-3]
		// Done computing second row.

		addps	xmm1,xmm0				// Third row done with xmm0

		movss	xmm3,[ecx+44]			// M[2][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// Begin fourth row of result.
		movss	xmm0,[ecx+48]			// M[3][0]
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4
	
		addps	xmm3,xmm2				// Third row done with xmm2
		
		movss	xmm2,[ecx+52]			// M[3][1]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm5

		addps	xmm3,xmm1				// Third row done with xmm1
		
		movss	xmm1,[ecx+56]			// M[3][2]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm6

		movups	[eax+32],xmm3			// Store Result.M[2][0-3]
		// Done computing third row.

		addps	xmm2,xmm0

		movss	xmm3,[ecx+60]			// M[3][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// stall
	
		addps	xmm3,xmm1
		
		// stall

		addps	xmm3,xmm2
	
		movups	[eax+48],xmm3			// Store Result.M[3][0-3]
		// Done computing fourth row.
		}
#else

		Result.M[0][0] = M[0][0] * Other.M[0][0] + M[0][1] * Other.M[1][0] + M[0][2] * Other.M[2][0] + M[0][3] * Other.M[3][0];
		Result.M[0][1] = M[0][0] * Other.M[0][1] + M[0][1] * Other.M[1][1] + M[0][2] * Other.M[2][1] + M[0][3] * Other.M[3][1];
		Result.M[0][2] = M[0][0] * Other.M[0][2] + M[0][1] * Other.M[1][2] + M[0][2] * Other.M[2][2] + M[0][3] * Other.M[3][2];
		Result.M[0][3] = M[0][0] * Other.M[0][3] + M[0][1] * Other.M[1][3] + M[0][2] * Other.M[2][3] + M[0][3] * Other.M[3][3];

		Result.M[1][0] = M[1][0] * Other.M[0][0] + M[1][1] * Other.M[1][0] + M[1][2] * Other.M[2][0] + M[1][3] * Other.M[3][0];
		Result.M[1][1] = M[1][0] * Other.M[0][1] + M[1][1] * Other.M[1][1] + M[1][2] * Other.M[2][1] + M[1][3] * Other.M[3][1];
		Result.M[1][2] = M[1][0] * Other.M[0][2] + M[1][1] * Other.M[1][2] + M[1][2] * Other.M[2][2] + M[1][3] * Other.M[3][2];
		Result.M[1][3] = M[1][0] * Other.M[0][3] + M[1][1] * Other.M[1][3] + M[1][2] * Other.M[2][3] + M[1][3] * Other.M[3][3];

		Result.M[2][0] = M[2][0] * Other.M[0][0] + M[2][1] * Other.M[1][0] + M[2][2] * Other.M[2][0] + M[2][3] * Other.M[3][0];
		Result.M[2][1] = M[2][0] * Other.M[0][1] + M[2][1] * Other.M[1][1] + M[2][2] * Other.M[2][1] + M[2][3] * Other.M[3][1];
		Result.M[2][2] = M[2][0] * Other.M[0][2] + M[2][1] * Other.M[1][2] + M[2][2] * Other.M[2][2] + M[2][3] * Other.M[3][2];
		Result.M[2][3] = M[2][0] * Other.M[0][3] + M[2][1] * Other.M[1][3] + M[2][2] * Other.M[2][3] + M[2][3] * Other.M[3][3];

		Result.M[3][0] = M[3][0] * Other.M[0][0] + M[3][1] * Other.M[1][0] + M[3][2] * Other.M[2][0] + M[3][3] * Other.M[3][0];
		Result.M[3][1] = M[3][0] * Other.M[0][1] + M[3][1] * Other.M[1][1] + M[3][2] * Other.M[2][1] + M[3][3] * Other.M[3][1];
		Result.M[3][2] = M[3][0] * Other.M[0][2] + M[3][1] * Other.M[1][2] + M[3][2] * Other.M[2][2] + M[3][3] * Other.M[3][2];
		Result.M[3][3] = M[3][0] * Other.M[0][3] + M[3][1] * Other.M[1][3] + M[3][2] * Other.M[2][3] + M[3][3] * Other.M[3][3];

#endif

		return Result;
	}

	__forceinline void operator*=(const UMatrix& Other)
	{
#if ASM
		__asm
		{
		mov		eax,[Other]
		mov		ecx,[this]
	
		movups	xmm4,[eax]				// Other.M[0][0-3]
		movups	xmm5,[eax+16]			// Other.M[1][0-3]
		movups	xmm6,[eax+32]			// Other.M[2][0-3]
		movups	xmm7,[eax+48]			// Other.M[3][0-3]

		// Begin first row of result.
		movss	xmm0,[ecx]				// M[0][0] 
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4

		movss	xmm1,[ecx+4]			// M[0][1]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm5

		movss	xmm2,[ecx+8]			// M[0][2]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm6

		addps	xmm1,xmm0				// First row done with xmm0

		movss	xmm3,[ecx+12]			// M[0][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// Begin second row of result.
		movss	xmm0,[ecx+16]			// M[1][0] 
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4
	
		addps	xmm3,xmm2				// First row done with xmm2
		
		movss	xmm2,[ecx+20]			// M[1][1]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm5

		addps	xmm3,xmm1				// First row done with xmm1
	
		movss	xmm1,[ecx+24]			// M[1][2]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm6

		movups	[ecx],xmm3				// Store M[0][0-3]
		// Done computing first row.
		
		addps	xmm2,xmm0				// Second row done with xmm0

		movss	xmm3,[ecx+28]			// M[1][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// Begin third row of result.
		movss	xmm0,[ecx+32]			// M[2][0] 
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4
	
		addps	xmm3,xmm1				// Second row done with xmm1
		
		movss	xmm1,[ecx+36]			// M[2][1]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm5

		addps	xmm3,xmm2				// Second row done with xmm2

		movss	xmm2,[ecx+40]			// M[2][2]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm6

		movups	[ecx+16],xmm3			// Store M[1][0-3]
		// Done computing second row.

		addps	xmm1,xmm0				// Third row done with xmm0

		movss	xmm3,[ecx+44]			// M[2][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// Begin fourth row of result.
		movss	xmm0,[ecx+48]			// M[3][0]
		shufps	xmm0,xmm0,0
		mulps	xmm0,xmm4
	
		addps	xmm3,xmm2				// Third row done with xmm2
		
		movss	xmm2,[ecx+52]			// M[3][1]
		shufps	xmm2,xmm2,0
		mulps	xmm2,xmm5

		addps	xmm3,xmm1				// Third row done with xmm1
		
		movss	xmm1,[ecx+56]			// M[3][2]
		shufps	xmm1,xmm1,0
		mulps	xmm1,xmm6

		movups	[ecx+32],xmm3			// Store M[2][0-3]
		// Done computing third row.

		addps	xmm2,xmm0

		movss	xmm3,[ecx+60]			// M[3][3]
		shufps	xmm3,xmm3,0
		mulps	xmm3,xmm7
		
		// stall
	
		addps	xmm3,xmm1
		
		// stall

		addps	xmm3,xmm2
	
		movups	[ecx+48],xmm3			// Store M[3][0-3]
		// Done computing fourth row.
		}
#else
		UMatrix Result;
		Result.M[0][0] = M[0][0] * Other.M[0][0] + M[0][1] * Other.M[1][0] + M[0][2] * Other.M[2][0] + M[0][3] * Other.M[3][0];
		Result.M[0][1] = M[0][0] * Other.M[0][1] + M[0][1] * Other.M[1][1] + M[0][2] * Other.M[2][1] + M[0][3] * Other.M[3][1];
		Result.M[0][2] = M[0][0] * Other.M[0][2] + M[0][1] * Other.M[1][2] + M[0][2] * Other.M[2][2] + M[0][3] * Other.M[3][2];
		Result.M[0][3] = M[0][0] * Other.M[0][3] + M[0][1] * Other.M[1][3] + M[0][2] * Other.M[2][3] + M[0][3] * Other.M[3][3];

		Result.M[1][0] = M[1][0] * Other.M[0][0] + M[1][1] * Other.M[1][0] + M[1][2] * Other.M[2][0] + M[1][3] * Other.M[3][0];
		Result.M[1][1] = M[1][0] * Other.M[0][1] + M[1][1] * Other.M[1][1] + M[1][2] * Other.M[2][1] + M[1][3] * Other.M[3][1];
		Result.M[1][2] = M[1][0] * Other.M[0][2] + M[1][1] * Other.M[1][2] + M[1][2] * Other.M[2][2] + M[1][3] * Other.M[3][2];
		Result.M[1][3] = M[1][0] * Other.M[0][3] + M[1][1] * Other.M[1][3] + M[1][2] * Other.M[2][3] + M[1][3] * Other.M[3][3];

		Result.M[2][0] = M[2][0] * Other.M[0][0] + M[2][1] * Other.M[1][0] + M[2][2] * Other.M[2][0] + M[2][3] * Other.M[3][0];
		Result.M[2][1] = M[2][0] * Other.M[0][1] + M[2][1] * Other.M[1][1] + M[2][2] * Other.M[2][1] + M[2][3] * Other.M[3][1];
		Result.M[2][2] = M[2][0] * Other.M[0][2] + M[2][1] * Other.M[1][2] + M[2][2] * Other.M[2][2] + M[2][3] * Other.M[3][2];
		Result.M[2][3] = M[2][0] * Other.M[0][3] + M[2][1] * Other.M[1][3] + M[2][2] * Other.M[2][3] + M[2][3] * Other.M[3][3];

		Result.M[3][0] = M[3][0] * Other.M[0][0] + M[3][1] * Other.M[1][0] + M[3][2] * Other.M[2][0] + M[3][3] * Other.M[3][0];
		Result.M[3][1] = M[3][0] * Other.M[0][1] + M[3][1] * Other.M[1][1] + M[3][2] * Other.M[2][1] + M[3][3] * Other.M[3][1];
		Result.M[3][2] = M[3][0] * Other.M[0][2] + M[3][1] * Other.M[1][2] + M[3][2] * Other.M[2][2] + M[3][3] * Other.M[3][2];
		Result.M[3][3] = M[3][0] * Other.M[0][3] + M[3][1] * Other.M[1][3] + M[3][2] * Other.M[2][3] + M[3][3] * Other.M[3][3];
		*this = Result;

#endif
	}

	// Comparison operators.

	inline bool operator==(const UMatrix& Other) const
	{
		for(INT X = 0;X < 4;X++)
			for(INT Y = 0;Y < 4;Y++)
				if(M[X][Y] != Other.M[X][Y])
					return 0;

		return 1;
	}

	inline bool operator!=(const UMatrix& Other) const
	{
		return !(*this == Other);
	}

	// Homogeneous transform.

	__forceinline UPlane TransformUPlane(const UPlane &P) const
	{
		UPlane Result;

#if ASM
		__asm
		{
			mov		eax,[P]
			mov		ecx,[this]
			
			movups	xmm4,[ecx]			// M[0][0]
			movups	xmm5,[ecx+16]		// M[1][0]
			movups	xmm6,[ecx+32]		// M[2][0]
			movups	xmm7,[ecx+48]		// M[3][0]

			movss	xmm0,[eax]UVector.X
			shufps	xmm0,xmm0,0
			mulps	xmm0,xmm4

			movss	xmm1,[eax]UVector.Y
			shufps	xmm1,xmm1,0
			mulps	xmm1,xmm5

			movss	xmm2,[eax]UVector.Z
			shufps	xmm2,xmm2,0
			mulps	xmm2,xmm6

			addps	xmm0,xmm1

			movss	xmm3,[eax]UPlane.W
			shufps	xmm3,xmm3,0
			mulps	xmm3,xmm7
			
			// stall
			lea		eax,[Result]

			addps	xmm2,xmm3
			
			// stall

			addps	xmm0,xmm2
		
			movups	[eax],xmm0
		}
#else
		Result.X = P.X * M[0][0] + P.Y * M[1][0] + P.Z * M[2][0] + P.W * M[3][0];
		Result.Y = P.X * M[0][1] + P.Y * M[1][1] + P.Z * M[2][1] + P.W * M[3][1];
		Result.Z = P.X * M[0][2] + P.Y * M[1][2] + P.Z * M[2][2] + P.W * M[3][2];
		Result.W = P.X * M[0][3] + P.Y * M[1][3] + P.Z * M[2][3] + P.W * M[3][3];
#endif

		return Result;
	}

	// Regular transform.

	__forceinline UVector TransformUVector(const UVector &V) const
	{
		return TransformUPlane(UPlane(V.X,V.Y,V.Z,1.0f));
	}

	__forceinline UVector InverseTransformUVector(const UVector &V) const
	{
		UVector t, Result;

		t.X = V.X - M[3][0];
		t.Y = V.Y - M[3][1];
		t.Z = V.Z - M[3][2];

		Result.X = t.X * M[0][0] + t.Y * M[0][1] + t.Z * M[0][2];
		Result.Y = t.X * M[1][0] + t.Y * M[1][1] + t.Z * M[1][2];
		Result.Z = t.X * M[2][0] + t.Y * M[2][1] + t.Z * M[2][2];

		return Result;
	}

	// Normal transform.

	__forceinline UPlane TransformNormal(const UVector& V) const
	{
		return TransformUPlane(UPlane(V.X,V.Y,V.Z,0.0f));
	}

	__forceinline UVector InverseTransformNormal(const UVector &V) const
	{
		UVector Result;

		Result.X = V.X * M[0][0] + V.Y * M[0][1] + V.Z * M[0][2];
		Result.Y = V.X * M[1][0] + V.Y * M[1][1] + V.Z * M[1][2];
		Result.Z = V.X * M[2][0] + V.Y * M[2][1] + V.Z * M[2][2];

		return Result;
	}

	// Transpose.

	__forceinline UMatrix Transpose()
	{
		UMatrix	Result;

		Result.M[0][0] = M[0][0];
		Result.M[0][1] = M[1][0];
		Result.M[0][2] = M[2][0];
		Result.M[0][3] = M[3][0];

		Result.M[1][0] = M[0][1];
		Result.M[1][1] = M[1][1];
		Result.M[1][2] = M[2][1];
		Result.M[1][3] = M[3][1];

		Result.M[2][0] = M[0][2];
		Result.M[2][1] = M[1][2];
		Result.M[2][2] = M[2][2];
		Result.M[2][3] = M[3][2];

		Result.M[3][0] = M[0][3];
		Result.M[3][1] = M[1][3];
		Result.M[3][2] = M[2][3];
		Result.M[3][3] = M[3][3];

		return Result;
	}

	// Determinant.

	inline FLOAT Determinant() const
	{
		return	M[0][0] * (
					M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
					M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
					M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
					) -
				M[1][0] * (
					M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
					M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
					M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
					) +
				M[2][0] * (
					M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
					M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
					M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
					) -
				M[3][0] * (
					M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
					M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
					M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
					);
	}

	// Inverse.

	UMatrix Inverse() const
	{
		UMatrix Result;
		FLOAT	Det = Determinant();

		//if(Det == 0.0f)
		//	return UMatrix::Identity;

		FLOAT	RDet = 1.0f / Det;

		Result.M[0][0] = RDet * (
				M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
				M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
				);
		Result.M[0][1] = -RDet * (
				M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
				);
		Result.M[0][2] = RDet * (
				M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
				M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				);
		Result.M[0][3] = -RDet * (
				M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
				M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
				M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				);

		Result.M[1][0] = -RDet * (
				M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][0] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
				M[3][0] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
				);
		Result.M[1][1] = RDet * (
				M[0][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][0] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][0] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
				);
		Result.M[1][2] = -RDet * (
				M[0][0] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
				M[1][0] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][0] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				);
		Result.M[1][3] = RDet * (
				M[0][0] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
				M[1][0] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
				M[2][0] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				);

		Result.M[2][0] = RDet * (
				M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
				M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
				M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1])
				);
		Result.M[2][1] = -RDet * (
				M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
				M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
				M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1])
				);
		Result.M[2][2] = RDet * (
				M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) -
				M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
				M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
				);
		Result.M[2][3] = -RDet * (
				M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) -
				M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
				M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
				);

		Result.M[3][0] = -RDet * (
				M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
				M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
				M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
				);
		Result.M[3][1] = RDet * (
				M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
				M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
				M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1])
				);
		Result.M[3][2] = -RDet * (
				M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) -
				M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
				M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
				);
		Result.M[3][3] = RDet * (
				M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
				M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
				M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
				);

		return Result;
	}

	UMatrix TransposeAdjoint() const
	{
		UMatrix ta;

		ta.M[0][0] = this->M[1][1] * this->M[2][2] - this->M[1][2] * this->M[2][1];
		ta.M[0][1] = this->M[1][2] * this->M[2][0] - this->M[1][0] * this->M[2][2];
		ta.M[0][2] = this->M[1][0] * this->M[2][1] - this->M[1][1] * this->M[2][0];
		ta.M[0][3] = 0.f;

		ta.M[1][0] = this->M[2][1] * this->M[0][2] - this->M[2][2] * this->M[0][1];
		ta.M[1][1] = this->M[2][2] * this->M[0][0] - this->M[2][0] * this->M[0][2];
		ta.M[1][2] = this->M[2][0] * this->M[0][1] - this->M[2][1] * this->M[0][0];
		ta.M[1][3] = 0.f;

		ta.M[2][0] = this->M[0][1] * this->M[1][2] - this->M[0][2] * this->M[1][1];
		ta.M[2][1] = this->M[0][2] * this->M[1][0] - this->M[0][0] * this->M[1][2];
		ta.M[2][2] = this->M[0][0] * this->M[1][1] - this->M[0][1] * this->M[1][0];
		ta.M[2][3] = 0.f;

		ta.M[3][0] = 0.f;
		ta.M[3][1] = 0.f;
		ta.M[3][2] = 0.f;
		ta.M[3][3] = 1.f;

		return ta;
	}

	// Remove any scaling from this matrix (ie magnitude of each row is 1)
	void RemoveScaling()
	{
		FLOAT SquareSum, Scale;

		// For each row, find magnitude, and if its non-zero re-scale so its unit length.
		for(INT i=0; i<3; i++)
		{
			SquareSum = (M[i][0] * M[i][0]) + (M[i][1] * M[i][1]) + (M[i][2] * M[i][2]);
			if(SquareSum > SMALL_NUMBER)
			{
				Scale = 1.f/sqrt(SquareSum);
				M[i][0] *= Scale; 
				M[i][1] *= Scale; 
				M[i][2] *= Scale; 
			}
		}
	}


	void ScaleTranslation(const UVector& Scale3D)
	{
		M[3][0] *= Scale3D.X;
		M[3][1] *= Scale3D.Y;
		M[3][2] *= Scale3D.Z;
	}

	// GetOrigin

	UVector GetOrigin() const
	{
		return UVector(M[3][0],M[3][1],M[3][2]);
	}

	UVector GetAxis(INT i) const
	{
		//checkSlow(i >= 0 && i <= 2);
		return UVector(M[i][0], M[i][1], M[i][2]);

	}

	void SetAxis( INT i, const UVector& Axis )
	{
		//checkSlow(i >= 0 && i <= 2);
		M[i][0] = Axis.X;
		M[i][1] = Axis.Y;
		M[i][2] = Axis.Z;
	}

	void SetOrigin( const UVector& NewOrigin )
	{
		M[3][0] = NewOrigin.X;
		M[3][1] = NewOrigin.Y;
		M[3][2] = NewOrigin.Z;
	}

	URotator Rotator() const;

	// Frustum plane extraction.

	static bool MakeFrustumPlane(FLOAT A,FLOAT B,FLOAT C,FLOAT D,UPlane& OutPlane)
	{
		FLOAT	LengthSquared = A * A + B * B + C * C;
		if(LengthSquared > DELTA*DELTA)
		{
			FLOAT	InvLength = 1.0f / sqrt(LengthSquared);
			OutPlane = UPlane(-A * InvLength,-B * InvLength,-C * InvLength,D * InvLength);
			return 1;
		}
		else
			return 0;
	}

	bool GetFrustumNearPlane(UPlane& OutPlane) const
	{
		return MakeFrustumPlane(
			M[0][2],
			M[1][2],
			M[2][2],
			M[3][2],
			OutPlane
			);
	}

	bool GetFrustumFarPlane(UPlane& OutPlane) const
	{
		return MakeFrustumPlane(
			M[0][3] - M[0][2],
			M[1][3] - M[1][2],
			M[2][3] - M[2][2],
			M[3][3] - M[3][2],
			OutPlane
			);
	}

	bool GetFrustumLeftPlane(UPlane& OutPlane) const
	{
		return MakeFrustumPlane(
			M[0][3] + M[0][0],
			M[1][3] + M[1][0],
			M[2][3] + M[2][0],
			M[3][3] + M[3][0],
			OutPlane
			);
	}

	bool GetFrustumRightPlane(UPlane& OutPlane) const
	{
		return MakeFrustumPlane(
			M[0][3] - M[0][0],
			M[1][3] - M[1][0],
			M[2][3] - M[2][0],
			M[3][3] - M[3][0],
			OutPlane
			);
	}

	bool GetFrustumTopPlane(UPlane& OutPlane) const
	{
		return MakeFrustumPlane(
			M[0][3] - M[0][1],
			M[1][3] - M[1][1],
			M[2][3] - M[2][1],
			M[3][3] - M[3][1],
			OutPlane
			);
	}

	bool GetFrustumBottomPlane(UPlane& OutPlane) const
	{
		return MakeFrustumPlane(
			M[0][3] + M[0][1],
			M[1][3] + M[1][1],
			M[2][3] + M[2][1],
			M[3][3] + M[3][1],
			OutPlane
			);
	}

	// Serializer.
	friend UPackage& operator>>(UPackage& Ar,UMatrix& M)
	{
		return Ar >> 
			M.M[0][0] >> M.M[0][1] >> M.M[0][2] >> M.M[0][3] >> 
			M.M[1][0] >> M.M[1][1] >> M.M[1][2] >> M.M[1][3] >> 
			M.M[2][0] >> M.M[2][1] >> M.M[2][2] >> M.M[2][3] >> 
			M.M[3][0] >> M.M[3][1] >> M.M[3][2] >> M.M[3][3];
	}
};

// Matrix operations.

class FPerspectiveMatrix : public UMatrix
{
public:

	FPerspectiveMatrix(float FOVX, float FOVY, float MultFOVX, float MultFOVY, float MinZ, float MaxZ) :
	  UMatrix(
			UPlane(MultFOVX / tan(FOVX),		0.0f,							0.0f,							0.0f),
			UPlane(0.0f,						MultFOVY / tan(FOVY),		0.0f,							0.0f),
			UPlane(0.0f,						0.0f,							MaxZ / (MaxZ - MinZ),			1.0f),
			UPlane(0.0f,						0.0f,							-MinZ * (MaxZ / (MaxZ - MinZ)),	0.0f))
	{
	}

	FPerspectiveMatrix(float FOV, float Width, float Height, float MinZ, float MaxZ) :
		UMatrix(
			UPlane(1.0f / tan(FOV),	0.0f,							0.0f,							0.0f),
			UPlane(0.0f,				Width / tan(FOV) / Height,	0.0f,							0.0f),
			UPlane(0.0f,				0.0f,							MaxZ / (MaxZ - MinZ),			1.0f),
			UPlane(0.0f,				0.0f,							-MinZ * (MaxZ / (MaxZ - MinZ)),	0.0f))
	{
	}

#define Z_PRECISION	0.001f

	FPerspectiveMatrix(float FOV, float Width, float Height, float MinZ) :
		UMatrix(
			UPlane(1.0f / tan(FOV),	0.0f,							0.0f,							0.0f),
			UPlane(0.0f,				Width / tan(FOV) / Height,	0.0f,							0.0f),
			UPlane(0.0f,				0.0f,							(1.0f - Z_PRECISION),			1.0f),
			UPlane(0.0f,				0.0f,							-MinZ * (1.0f - Z_PRECISION),	0.0f))
	{
	}
};

class FOrthoMatrix : public UMatrix
{
public:

	FOrthoMatrix(float Width,float Height,float ZScale,float ZOffset) :
		UMatrix(
			UPlane(1.0f / Width,	0.0f,			0.0f,				0.0f),
			UPlane(0.0f,			1.0f / Height,	0.0f,				0.0f),
			UPlane(0.0f,			0.0f,			ZScale,				0.0f),
			UPlane(0.0f,			0.0f,			ZOffset * ZScale,	1.0f))
	{
	}
};

class DLLEXPORT FTranslationMatrix : public UMatrix
{
public:

	FTranslationMatrix(UVector Delta) :
		UMatrix(
			UPlane(1.0f,	0.0f,	0.0f,	0.0f),
			UPlane(0.0f,	1.0f,	0.0f,	0.0f),
			UPlane(0.0f,	0.0f,	1.0f,	0.0f),
			UPlane(Delta.X,	Delta.Y,Delta.Z,1.0f))
	{
	}
};

class DLLEXPORT FRotationMatrix : public UMatrix
{
public:

#if 0
	FRotationMatrix(URotator Rot) :
		UMatrix(
			Identity *
			UMatrix(	// Roll
				UPlane(1.0f,					0.0f,					0.0f,						0.0f),
				UPlane(0.0f,					+GMath.CosTab(Rot.Roll),-GMath.SinTab(Rot.Roll),	0.0f),
				UPlane(0.0f,					+GMath.SinTab(Rot.Roll),+GMath.CosTab(Rot.Roll),	0.0f),
				UPlane(0.0f,					0.0f,					0.0f,						1.0f)) *
			UMatrix(	// Pitch
				UPlane(+GMath.CosTab(Rot.Pitch),0.0f,					+GMath.SinTab(Rot.Pitch),	0.0f),
				UPlane(0.0f,					1.0f,					0.0f,						0.0f),
				UPlane(-GMath.SinTab(Rot.Pitch),0.0f,					+GMath.CosTab(Rot.Pitch),	0.0f),
				UPlane(0.0f,					0.0f,					0.0f,						1.0f)) *
			UMatrix(	// Yaw
				UPlane(+GMath.CosTab(Rot.Yaw),	+GMath.SinTab(Rot.Yaw), 0.0f,	0.0f),
				UPlane(-GMath.SinTab(Rot.Yaw),	+GMath.CosTab(Rot.Yaw), 0.0f,	0.0f),
				UPlane(0.0f,					0.0f,					1.0f,	0.0f),
				UPlane(0.0f,					0.0f,					0.0f,	1.0f))
			)
	{
	}
#else
	FRotationMatrix(URotator Rot)
	{
		FLOAT	SR	= GMath.SinTab(Rot.Roll),
				SP	= GMath.SinTab(Rot.Pitch),
				SY	= GMath.SinTab(Rot.Yaw),
				CR	= GMath.CosTab(Rot.Roll),
				CP	= GMath.CosTab(Rot.Pitch),
				CY	= GMath.CosTab(Rot.Yaw);

		M[0][0]	= CP * CY;
		M[0][1]	= CP * SY;
		M[0][2]	= SP;
		M[0][3]	= 0.f;

		M[1][0]	= SR * SP * CY - CR * SY;
		M[1][1]	= SR * SP * SY + CR * CY;
		M[1][2]	= - SR * CP;
		M[1][3]	= 0.f;

		M[2][0]	= -( CR * SP * CY + SR * SY );
		M[2][1]	= CY * SR - CR * SP * SY;
		M[2][2]	= CR * CP;
		M[2][3]	= 0.f;

		M[3][0]	= 0.f;
		M[3][1]	= 0.f;
		M[3][2]	= 0.f;
		M[3][3]	= 1.f;
	}
#endif
};

class FInverseRotationMatrix : public UMatrix
{
public:

	FInverseRotationMatrix(URotator Rot) :
		UMatrix(
			UMatrix(	// Yaw
				UPlane(+GMath.CosTab(-Rot.Yaw),	+GMath.SinTab(-Rot.Yaw), 0.0f,	0.0f),
				UPlane(-GMath.SinTab(-Rot.Yaw),	+GMath.CosTab(-Rot.Yaw), 0.0f,	0.0f),
				UPlane(0.0f,					0.0f,					1.0f,	0.0f),
				UPlane(0.0f,					0.0f,					0.0f,	1.0f)) *
			UMatrix(	// Pitch
				UPlane(+GMath.CosTab(-Rot.Pitch),0.0f,					+GMath.SinTab(-Rot.Pitch),	0.0f),
				UPlane(0.0f,					1.0f,					0.0f,						0.0f),
				UPlane(-GMath.SinTab(-Rot.Pitch),0.0f,					+GMath.CosTab(-Rot.Pitch),	0.0f),
				UPlane(0.0f,					0.0f,					0.0f,						1.0f)) *
			UMatrix(	// Roll
				UPlane(1.0f,					0.0f,					0.0f,						0.0f),
				UPlane(0.0f,					+GMath.CosTab(-Rot.Roll),-GMath.SinTab(-Rot.Roll),	0.0f),
				UPlane(0.0f,					+GMath.SinTab(-Rot.Roll),+GMath.CosTab(-Rot.Roll),	0.0f),
				UPlane(0.0f,					0.0f,					0.0f,						1.0f))
			)
	{
	}
};

class DLLEXPORT FScaleMatrix : public UMatrix
{
public:

	FScaleMatrix(UVector Scale) :
		UMatrix(
			UPlane(Scale.X,	0.0f,		0.0f,		0.0f),
			UPlane(0.0f,	Scale.Y,	0.0f,		0.0f),
			UPlane(0.0f,	0.0f,		Scale.Z,	0.0f),
			UPlane(0.0f,	0.0f,		0.0f,		1.0f))
	{
	}
};

//
//	FRangeMapMatrix - Maps points from a bounded range to another bounded range.
//

struct FRangeMapMatrix: UMatrix
{
	FRangeMapMatrix(const UVector& MinA,const UVector& MaxA,const UVector& MinB,const UVector& MaxB)
	{
		for(UINT RowIndex = 0;RowIndex < 4;RowIndex++)
			for(UINT ColumnIndex = 0;ColumnIndex < 4;ColumnIndex++)
				M[RowIndex][ColumnIndex] = 0.0f;
		M[3][3] = 1.0f;

		for(UINT ComponentIndex = 0;ComponentIndex < 3;ComponentIndex++)
		{
			FLOAT	RangeA = (&MaxA.X)[ComponentIndex] - (&MinA.X)[ComponentIndex],
				RangeB = (&MaxB.X)[ComponentIndex] - (&MinB.X)[ComponentIndex];

			M[ComponentIndex][ComponentIndex] = RangeB / RangeA;
			M[3][ComponentIndex] = ((&MinB.X)[ComponentIndex] - (&MinA.X)[ComponentIndex]) * M[ComponentIndex][ComponentIndex];
		}
	}
};

//
//	FBasisVectorMatrix
//

struct FBasisVectorMatrix: UMatrix
{
	FBasisVectorMatrix(const UVector& XAxis,const UVector& YAxis,const UVector& ZAxis,const UVector& Origin)
	{
		for(UINT RowIndex = 0;RowIndex < 3;RowIndex++)
		{
			M[RowIndex][0] = (&XAxis.X)[RowIndex];
			M[RowIndex][1] = (&YAxis.X)[RowIndex];
			M[RowIndex][2] = (&ZAxis.X)[RowIndex];
			M[RowIndex][3] = 0.0f;
		}
		M[3][0] = Origin | XAxis;
		M[3][1] = Origin | YAxis;
		M[3][2] = Origin | ZAxis;
		M[3][3] = 1.0f;
	}
};

//
//	FBox::TransformBy
//

/*inline FBox FBox::TransformBy(const UMatrix& M) const
{
	FBox	NewBox(0);

	for(INT X = 0;X < 2;X++)
		for(INT Y = 0;Y < 2;Y++)
			for(INT Z = 0;Z < 2;Z++)
				NewBox += M.TransformUVector(UVector(GetExtrema(X).X,GetExtrema(Y).Y,GetExtrema(Z).Z));

	return NewBox;
}*/
