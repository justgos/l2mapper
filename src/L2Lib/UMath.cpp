#include "UBase.h"
#include "UPackage.h"

#include "UMath.h"

FGlobalMath::FGlobalMath()
{
	// Init base angle table.
	{for( int i=0; i<NUM_ANGLES; i++ )
		TrigFLOAT[i] = sin((float)i * 2.f * PI / (float)NUM_ANGLES);}
}
