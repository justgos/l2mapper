#pragma once

#include "UMath.h"
#include "UObject.h"
#include "AActor.h"
#include "UTerrain.h"
#include "UProperty.h"
#include "math.h"
#include "L2LibVarsHolder.h"

class UURL
{
public:
	// URL components.
	UString Protocol;	// Protocol, i.e. "unreal" or "http".
	UString Host;		// Optional hostname, i.e. "204.157.115.40" or "unreal.epicgames.com", blank if local.
	int32		Port;       // Optional host port.
	UString Map;		// Map name, i.e. "SkyCity", default is "Index".
	jfArray<UString, uint32> Op;	// Options.
	UString Portal;		// Portal to enter through, default is "".

	// Status.
	bool Valid;		// Whether parsed successfully.

/*	// Statics.
	static FString DefaultProtocol;
	static FString DefaultProtocolDescription;
	static FString DefaultName;
	static FString DefaultMap;
	static FString DefaultLocalMap;
	static FString DefaultHost;
	static FString DefaultPortal;
	static FString DefaultMapExt;
	static FString DefaultSaveExt;
	static INT DefaultPort;

	// Constructors.
	FURL( const TCHAR* Filename=0 );
	FURL( FURL* Base, const TCHAR* TextURL, ETravelType Type );
	static void StaticInit( );
	static void StaticExit( );

	// Functions.
	UBOOL IsInternal( ) const;
	UBOOL IsLocalInternal( ) const;
	UBOOL HasOption( const TCHAR* Test ) const;
	const TCHAR* GetOption( const TCHAR* Match, const TCHAR* Default ) const;
	void LoadURLConfig( const TCHAR* Section, const TCHAR* Filename=0 );
	void SaveURLConfig( const TCHAR* Section, const TCHAR* Item, const TCHAR* Filename=0 ) const;
	void AddOption( const TCHAR* Str );
	FString String( UBOOL FullyQualified=0 ) const;
	ENGINE_API friend FArchive& operator<<( FArchive& Ar, FURL& U );

	// Operators.
	UBOOL operator==( const FURL& Other ) const;
*/

	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> Protocol >> Host;
		if(Host.Length() > 0) // ich nehme das mal an...
			Ar >> Port;
		Ar >> Map >> Op >> Portal;
		Valid = true; // parsed sucessfully
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UURL );
};

class UReachSpec
{
public:
	int32 distance; 
	UObject /*AActor*/ *Start;
	UObject /*AActor*/ *End; //actor at endpoint of this path (next waypoint or goal)
	int32 CollisionRadius; 
    int32 CollisionHeight; 
	int32 reachFlags; //see defined bits above
	int8  bPruned;

	/*
	supports() -
	 returns true if it supports the requirements of aPawn.  Distance is not considered.
	*/
/*	inline int supports (int iRadius, int iHeight, int moveFlags) {
		return ( (CollisionRadius >= iRadius) 
			&& (CollisionHeight >= iHeight)
			&& ((reachFlags & moveFlags) == reachFlags) );
	}
	FReachSpec operator+ (const FReachSpec &Spec) const;
	int defineFor (AActor * begin, AActor * dest, APawn * Scout);
	int operator<= (const FReachSpec &Spec);
	int operator== (const FReachSpec &Spec);
	int MonsterPath();
	int BotOnlyPath();

	void Init( void ) {
		fguard_beg( FReachSpec::Init );
		// Init everything here.
		Start = End = 0;
		distance = CollisionRadius = CollisionHeight = 0;
		reachFlags = 0;
		bPruned = 0;
		fguard_end( FReachSpec::Init );
	};

	friend FArchive& operator<< (FArchive &Ar, FReachSpec &ReachSpec ) {
		guard(FReachSpec<<);
		Ar << ReachSpec.distance << ReachSpec.Start << ReachSpec.End;
		Ar << ReachSpec.CollisionRadius << ReachSpec.CollisionHeight;
		Ar << ReachSpec.reachFlags << ReachSpec.bPruned;
		return Ar;
		unguard;
	};

	int findBestReachable( FVector &Start, FVector &Destination, APawn * Scout );
*/
	// Input - Serializer
	UPackage& Serialize( UPackage& Ar )
	{
		Ar >> distance >> Start >> End;
		Ar >> CollisionRadius >> CollisionHeight;
		Ar >> reachFlags >> bPruned;
		return Ar;
	}
	IMPLEMENT_USERIALIZER( UReachSpec );
};

class DLLEXPORT ULevel : public UObject
{
public:
	UURL URL;

	// Number of blocks of descriptive text to allocate with levels.
	enum {NUM_LEVEL_TEXT_BLOCKS=16};

	// Main variables, always valid.
	jfArray<UReachSpec, uint32> ReachSpecs;
	UModel *Model;		// the main BSP Model
	jfArray<UObject*, uint32> AllObjects;
	jfArray<AActor*, uint32> AllActors;
	//UTextBuffer *TextBlocks[NUM_LEVEL_TEXT_BLOCKS];
	//FTime TimeSeconds;
	_vector3 _mapLoc;
	_vector3 _camPos;
	float _visibleDist;
	L2LibVarsHolder *L2LibVars;
	bool _isVisible;
public:
	virtual bool SetVariable(const class UProperty& Prop);
	virtual void OnLoaded() {};

	void setL2LibVars(L2LibVarsHolder *vars);

	virtual void Tick();

	virtual void finishLoad();
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(ULevel);
public:
	ULevel(void);
	~ULevel(void);
};

