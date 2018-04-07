#pragma once

#ifdef _MSC_VER	/* M$-speziefisch */
# define DBSPAPI	__cdecl
# define DLIBAPI 	__stdcall
//# define DLLEXPORT	__declspec( dllexport )
# define DLLEXPORT
//# define asm		__asm
# pragma warning( disable: 4018 ) // signed/unsigned mismatch
# pragma warning( disable: 4138 ) // '*/' found outside of comment
# pragma warning( disable: 4193 ) // #pragma warning(pop) : no matching '#pragma warning(push)'
# pragma warning( disable: 4309 ) // 'initializing' : truncation of constant value
# pragma warning( disable: 4800 ) // int -> bool conversion
# pragma warning( disable: 4244 ) // double -> float / int64 -> int conversions
# pragma warning( disable: 4267 ) // 'size_t' -> 'int'
# pragma warning( disable: 4251 ) // class 'std::* needs to have dll-interface to be used by clients of class ...
# pragma warning( disable: 4275 ) // non dll-interface class * used as base for dll-interface class
# pragma warning( disable: 4305 ) // 'argument' : truncation from 'double' to 'GLfloat'
//# pragma warning( disable: 4311 ) // auch 64 bit
#elif defined(__GNUC__)
# define DBSPAPI
# define DLIBAPI
# define DLLEXPORT
#endif

#ifndef _WIN32
#define stricmp strcasecmp
#else
//#define snprintf _snprintf
//#define vsnprintf _vsnprintf
#endif

// Make sure that _DEBUG, DEBUG and NDEBUG have known values -- pref
#ifdef NDEBUG
#undef _DEBUG
#undef DEBUG
#endif

#ifdef DEBUG
#ifndef _DEBUG
#define _DEBUG 1
#endif
#endif

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG 1
#endif
#endif

#if !defined(DEBUG) && !defined(_DEBUG)
#ifndef NDEBUG
#define NDEBUG
#endif
#endif

//#if defined( _DEBUG ) && defined( MEM_MGR )
//#include "cm_dbg_block.h"			// Speicher-Manager ( nur im Debug-Bereich! )
//#endif /* _DEBUG && MEM_MGR */

#define GL_GLEXT_PROTOTYPES 1

#include <cstdlib>					// standart C-Bibliotheken
#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <exception>
#include <math.h>

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/wglext.h>
//#include <gl/glext.h>

//#include "glext.h"

//#include <tbb/tbb.h>

#include "Singleton.h"
#include "math.h"

#define g_math ::Singleton<math>::get_instance()

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLu32.lib")
#pragma comment (lib, "glaux.lib")
#pragma comment (lib, "glew32s.lib")
//#pragma comment (lib, "dsound.lib")

// HINWEIS: im Gegensatz zu assert() muss der Ausdruck 'n' falsch sein um keine Exception auszulösen
#define CHECK(n)			if( n ) __excp_assertsfunc( #n, __FILE__, __LINE__ );
#define CHECKS(n,s)			if( n ) __excp_assertsfunc(  s, __FILE__, __LINE__ );

// Übliches ASSERT(), verwendet eigene __excp_assertsfunc
#ifdef DEBUG
#define ASSERT(expr)		if (!(expr)) __excp_assertsfunc( #expr, __FILE__, __LINE__)
#else
#define ASSERT(expr)
#endif

#ifdef __GNUC__
#define PRINTF_FORMAT(b,c) __attribute__ (( __format__ (__printf__,b,c) ))
#else
#define PRINTF_FORMAT(b,c)
#endif

#define PATHSEPARATOR(c)	( (c) == '\\' || (c) == '/' )
#define LOWERCASE(c)		( (c) <= 'Z' && (c) >= 'A' ? (c) + ('a'-'A') : (c) )
#define UPPERCASE(c)		((c) <= 'z' && (c) >= 'a' ? (c) - ('a'-'A') : (c))

#define ENG_MAXNAME	48	// max Zeichen für sound/texture/shader/ ect

#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define $Line					MakeString( Stringize, __LINE__ )
#define REMINDER				__FILE__ "(" $Line ") : Reminder: "

// typedefs from MaNGOS WoW serv
typedef __int64            int64;
typedef long               int32;
typedef short              int16;
typedef char               int8;
typedef unsigned __int64   uint64;
typedef unsigned long      uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;

#define IMPLEMENT_USERIALIZER(cl)		friend inline UPackage& operator >> (UPackage& Ar, cl& Obj) { return Obj.Serialize(Ar);  }

#define UPACKAGE_UNREAL2_VERSION	100

// These need to be public for UnrealEd etc.
const FLOAT P2UScale = 50.0f;
const FLOAT U2PScale = 0.02f;

const FLOAT Rad2U = 10430.2192f;
const FLOAT U2Rad = 0.000095875262f;

class	UPackage;
struct		UImport;
struct		UExport;

class	UObject;
class		AActor;
class			UBrush;
class			UStaticMeshActor;
class		UPrimitive;
class			UModel;
class			UPolys;
class			UStaticMesh;
class		UMaterial;
class			UTexture;
class		ULevel;
class		UTerrain;

class UString;

template <typename T, typename T1> class jfArray;

extern class FGlobalMath GMath;

#include "utils.h"

#include "Platform.h"
