#include "UBase.h"
#include "UPackage.h"

#include "UProperty.h"

#include "UObject.h"
#include "UObj.h"
#include "UTerrain.h"

UProperty::UProperty(void)
{
	Data = 0;
	DataLength = 0;
	ArrayIndex = -1;
	StructType = 0;
}

UProperty::~UProperty(void)
{
}

void UProperty::ReadArrayIndex(UPackage &Ar)
{
	int8 B;
	
	Ar >> B;

	ArrayIndex = B;

	if( ArrayIndex < 128 )
		return;

	Ar.Read((int8*)&ArrayIndex + sizeof(int8), sizeof(int8)); // weiteres Byte lesen
	ArrayIndex &= 0x7fff;

	if(ArrayIndex < 16384)
		return;

	ArrayIndex &= 0x3fffffff;
	Ar.Read((int8*)&ArrayIndex + sizeof(int8) * 2, sizeof(int8) * 2);
}

void UProperty::ReadStruct(UPackage &Ar)
{
	int32 Index;

	if(UTIL_strcmpi(StructType, "Color") == 0)
	{
		/*readInt8(); // R
		readInt8(); // G
		readInt8(); // B
		readInt8(); // A*/
		Write(Ar, 4 * sizeof(int8));
	}
	else if(UTIL_strcmpi(StructType, "Vector") == 0)
	{
		/*readFloat(); // X
		readFloat(); // Y
		readFloat(); // Z*/
		Write(Ar, 3 * sizeof(float));
		/*UVector tv = *(UVector*)Data;
		printf(": %f %f %f\n", tv.X, tv.Y, tv.Z);
		int gg = 0;*/
	}
	else if(UTIL_strcmpi(StructType, "Rotator") == 0)
	{
		/*readFloat(); // Pitch
		readFloat(); // Yaw
		readFloat(); // Roll*/
		Write(Ar, 3 * sizeof(int32));
		/*URotator tv = *(URotator*)Data;
		printf(": %d %d %d\n", tv.Pitch, tv.Yaw, tv.Roll);
		int gg = 0;*/
	}
	else if(UTIL_strcmpi(StructType, "Scale") == 0)
	{
		/*readFloat(); // X
		readFloat(); // Y
		readFloat(); // Z
		readInt32(); // sheer rate
		readInt8(); // sheer axis*/
		Write(Ar, 3 * sizeof(float) + sizeof(float) + sizeof(int8));
		Ar.Seek(8);
	}
	else if(UTIL_strcmpi(StructType, "Plane") == 0)
	{
		//Write(Ar, 4 * sizeof(float));
		UObject *Object = new UObject();
		Ar >> *Object;
		Write(&Object, sizeof(UObject*));
	}
	else if(UTIL_strcmpi(StructType, "RangeVector") == 0)
	{
		//Write(Ar, 4 * sizeof(float));
		UObject *Object = new UObject();
		Ar >> *Object;
		Write(&Object, sizeof(UObject*));
	}
	else if(UTIL_strcmpi(StructType, "Range") == 0)
	{
		//Write(Ar, 4 * sizeof(float));
		UObject *Object = new UObject();
		Ar >> *Object;
		Write(&Object, sizeof(UObject*));
	}
	else if(UTIL_strcmpi(StructType, "Matrix") == 0)
	{
		//Write(Ar, 16 * sizeof(float));
		UObject *Object = new UObject();
		Ar >> *Object;
		Write(&Object, sizeof(UObject*));
	}
	else if(UTIL_strcmpi(StructType, "PointRegion") == 0)
	{
		UPointRegion *Object = new UPointRegion();
		Ar >> *Object;
		Write(&Object, sizeof(UPointRegion*));
	}
	else if(UTIL_strcmpi(StructType, "TextureModifyinfo") == 0)
	{
		UObject *Object = new UObject();
		Ar >> *Object;
		Write(&Object, sizeof(UObject*));
	}
	else if(UTIL_strcmpi(StructType, "TerrainLayer") == 0)
	{
		//Ar.TestDump(100, true);

		UTerrainLayer *Object = new UTerrainLayer();
		Ar >> *Object;
		Write(&Object, sizeof(UTerrainLayer*));

		//Ar.TestDump(100, true);

		/*uint32 t = Ar.Tell();
		for(uint32 u = 0; u < 1000; u++)
		{
			Ar.Seek(t + u, U_SEEK_SET);
			int32 Index;
			Ar >> UIndex(Index);
			if(Index >= 0 && Index < Ar.Names.Size() && UTIL_strcmpi(Ar.Names[Index].Name, "TerrainLayer") == 0)
			{
				printf("Name: %d, %s (%d, 0x%02X), Offset: %d\n", u, Ar.Names[Index].Name, Index, (Index > 0 && Index < 256 ? (uint8)Index : 0), t + u);
			}
		}*/

		int gg = 0;
	}
}

UPackage& UProperty::Serialize(class UPackage& Ar)
{
	int32 Index;
	int8 Info;
	
	DataLength = 0;

	Ar >> UIndex(Index);

	if(Index < 0 || Index >= Ar.Names.Size())
	{
		Ar.Seek(-11);
		printf("ERROR: wrong property name index: %d\n", Index);
		printf("    10 bytes before index: ");
		Ar.TestDump(10, true);
		printf("\n");
		Ar.Seek(10);
		printf("    10 bytes after index (including index): ");
		Ar.TestDump(10, true);
		printf("\n");
		Ar.Seek(1);
		// this surely should not happen.. :(

		Name = UTIL_CopyString("None");

		// trick
		/*Ar.Seek(-2);
		int8 TestVal;
		Ar >> TestVal;
		if(TestVal >= 0 && TestVal < Ar.Names.Size() && UTIL_strcmpi(Ar.Names[TestVal].Name, "None") == 0)
			Ar.Seek(-1);
		else
			Ar.Seek(2);*/

		return Ar;
	}

	Name = Ar.Names[Index].Name;

	if(UTIL_strcmpi(Name, "None") == 0)
		return Ar;

	/*if(UTIL_strcmpi(Ar.m_Name, "Annihilation_Field_t") == 0)
	{
		int gg = 0;
	}*/

	/*if(UTIL_strcmpi(Name, "DensityMeshName") == 0)
	{
		uint32 t = Ar.Tell();

		for(uint32 u = 0; u < 5000; u++)
		{
			Ar.Seek(t + u, U_SEEK_SET);
			int32 Index;
			Ar >> UIndex(Index);
			//if(Index < 0 && -Index - 1 < Imports.Size())
			//	printf("Import: %d, %s (%d)\n", u, Imports[-Index - 1].Name, Index);
			//if(Index > 0 && Index - 1 < Exports.Size())
			//	printf("Export: %d, %s (%d)\n", u, Exports[Index - 1].Name, Index);
			if(Index >= 0 && Index < Ar.Names.Size())
				printf("Name: %d, %s (%d, 0x%02X)\n", u, Ar.Names[Index].Name, Index, (Index > 0 && Index < 256 ? (uint8)Index : 0));
		}

		int gg = 0;
	}*/
	
	Ar >> Info;

	IsArray = (Info & PROPERTY_ARRAY_MASK) != 0;
	Type = (int8)(Info & PROPERTY_TYPE_MASK);

	if(Type == UNP_StructProperty)
	{
		Ar >> UIndex(Index);
		StructType = Ar.Names[Index].Name;
	}

	switch (Info & PROPERTY_SIZE_MASK)
	{
		case    0x00:  Size = 1;                        break;
		case    0x10:  Size = 2;                        break;
		case    0x20:  Size = 4;                        break;
		case    0x30:  Size = 12;                       break;
		case    0x40:  Size = 16;                       break;
		case    0x50:  Ar >> (int8&)Size;				break;
		case    0x60:  Ar >> (int16&)Size;				break;
		case    0x70:  Ar >> (int32&)Size;				break;
	}

	if(IsArray && Type != UNP_BoolProperty)
		ReadArrayIndex(Ar);

	uint32 l;

	UObject *Object;
	UProperty Temp;
	UString str;

	uint32 TPos = 0;

	uint32 pos = 0;

	switch(Type)
	{
		case UNP_ByteProperty:
			Write(Ar, sizeof(int8));
			l = *(int8*)Data;
			break;
		case UNP_IntProperty:
			Write(Ar, sizeof(int32));
			l = *(int32*)Data;
			break;
		case UNP_BoolProperty:
			Data = new int8[1];
			Data[0] = IsArray;
			IsArray = false;
			break;
		case UNP_FloatProperty:
			Write(Ar, sizeof(float));
			break;
		case UNP_ObjectProperty:
			if(UTIL_strcmpi(Name, "StaticMeshLod01") == 0 || UTIL_strcmpi(Name, "StaticMeshLod02") == 0 || UTIL_strcmpi(Name, "PhysicsVolume") == 0)
			{
				//printf("Skipping object property: %s\n", Name);
				Ar >> UIndex(Index);
			}
			else
			{
				Ar >> Object;
			}
			Write(&Object, sizeof(UObject*));
			break;
		case UNP_NameProperty:
			Ar >> UIndex(Index);
			Write(Ar.Names[Index].Name, strlen(Ar.Names[Index].Name) + 1);
			//printf(" val: %s", pkg->names._elems[s_id < 0 ? pkg->imports._elems[-s_id - 1].name : pkg->exports._elems[s_id - 1].name]);
			break;
		case UNP_StrProperty:
			Ar >> str;
			//printf("oops :(");

			//Ar >> UIndex(Index);
			//Write(Ar, Index);

			//printf(" val: %d", readIndex());
			// FIXME: !!1
			break;
		case UNP_StringProperty:
			Write(Ar, Size);
			break;
		case UNP_StructProperty:
			ReadStruct(Ar);
			break;
		case UNP_ArrayProperty:
			//Ar >> UIndex(Index);
			//Write(&Index, sizeof(int32));

			pos = Ar.Tell();
			Ar >> UIndex(Index);
			ArrayLength = Index;
			l = ArrayLength;

			if(UTIL_strcmpi(Name, "Materials") == 0)
			{
				for(uint32 u = 0; u < l; u++)
				{
					/*while(true)
					{
						Ar >> Temp;
						if(UTIL_strcmpi(Temp.Name, "None") == 0)
							break;
					}*/

					//printf("\n============ Material %d ============\n", u);

					//Object = Ar.SerializeObject("MaterialContainer");
					Object = new UMaterialContainer();
					Ar >> *(UMaterialContainer*)Object;
					Write(&Object, sizeof(UObject*));
				}
			}
			else if(UTIL_strcmpi(Name, "Touching") == 0)
			{
				//Ar.TestDump(10, true);
				for(uint32 u = 0; u < l; u++)
				{
					Ar >> UIndex(Index);
				}
			}
			else if(UTIL_strcmpi(Name, "StateStaticMeshs") == 0)
			{
				//Ar.TestDump(10, true);
				for(uint32 u = 0; u < l; u++)
				{
					Ar >> UIndex(Index);
				}
			}
			/*else if(UTIL_strcmpi(Name, "AccessoryTypeList") == 0)
			{
				for(uint32 u = 0; u < l; u++)
				{
					Ar >> UIndex(Index);
				}
			}*/
			else if(UTIL_strcmpi(Name, "Skins") == 0)
			{
				//Ar.TestDump(10, true);
				for(uint32 u = 0; u < l; u++)
				{
					Ar >> UIndex(Index);
				}
			}
			else if(UTIL_strcmpi(Name, "Terrains") == 0)
			{
				//Ar.TestDump(10, true);
				for(uint32 u = 0; u < l; u++)
				{
					Ar >> UIndex(Index);
				}
			}
			else if(UTIL_strcmpi(Name, "DecoLayers") == 0)
			{
				for(uint32 u = 0; u < l; u++)
				{
					//Object = Ar.SerializeObject("DecorationLayer");
					Object = new UObject();
					Ar >> *(UObject*)Object;
					Write(&Object, sizeof(UObject*));
				}
			}
			else if(UTIL_strcmpi(Name, "QuadVisibilityBitmap") == 0
					|| UTIL_strcmpi(Name, "QuadVisibilityBitmapOrig") == 0
					|| UTIL_strcmpi(Name, "EdgeTurnBitmap") == 0
					|| UTIL_strcmpi(Name, "EdgeTurnBitmapOrig") == 0
					|| UTIL_strcmpi(Name, "RenderCombinations") == 0
					|| UTIL_strcmpi(Name, "VertexStreams") == 0)
			{
				Write(Ar, l * sizeof(int32));

				/*if(UTIL_strcmpi(Name, "QuadVisibilityBitmap") == 0)
				{
					char* tst = new char[1000000];
					sprintf(tst, "");
					for(int i = 0; i < l * sizeof(int32); i++)
						sprintf(tst, "%s\r\n%d", tst, Data[i]);
					int g = 0;
				}*/
			}
			else if(UTIL_strcmpi(Name, "TIntMap") == 0)
			{
				Write(Ar, Size - (Ar.Tell() - pos));

				/*Ar.Seek(-100);

				uint32 t = Ar.Tell();

				for(uint32 u = 0; u < 100; u++)
				{
					Ar.Seek(t + u, U_SEEK_SET);
					int32 Index;
					Ar >> UIndex(Index);
					//if(Index < 0 && -Index - 1 < Imports.Size())
					//	printf("Import: %d, %s (%d)\n", u, Imports[-Index - 1].Name, Index);
					//if(Index > 0 && Index - 1 < Exports.Size())
					//	printf("Export: %d, %s (%d)\n", u, Exports[Index - 1].Name, Index);
					if(Index >= 0 && Index < Ar.Names.Size())
						printf("Name: %d, %s (%d, 0x%02X)\n", u, Ar.Names[Index].Name, Index, (Index > 0 && Index < 256 ? (uint8)Index : 0));
				}*/

				/*for(uint32 u = 0; u < l; u++)
				{
					Ar >> UIndex(Index);
					Write(Ar, 2 * Index * sizeof(int32));
					Ar.TestDump(100, true);
				}*/
				for(uint32 u = 0; u < l; u++)
				{
					/*Ar >> UIndex(Index);
					for(uint32 h = 0; h < Index; h++)
					{
						int32 v1;
						int32 v2;
						Ar >> v1 >> v2;
					}*/
					/*int16 ardim;
					int16 elemsz;
					int32 flags;
					int32 cat;
					Ar >> ardim >> elemsz >> flags >> UIndex(cat);
					char *cat_str = Ar.Names[cat].Name;
					Write(Ar, ardim * elemsz); //  * sizeof(int32)
					*/
				}
				int gg = 0;
			}

			break;

		// ------------------------------------------------
		// These are considered as unknown and will throw
		// ------------------------------------------------
		case UNP_ClassProperty:
		case UNP_VectorProperty:
			Write(Ar, 3 * sizeof(float));
			break;
		case UNP_RotatorProperty:
			Write(Ar, 3 * sizeof(float));
			break;
		case UNP_MapProperty:
		case UNP_FixedArrayProperty:
			printf(" UNKNOWN PROPERTY!");
			/*SetUnknownProperty( aUn, aUf );*/
			break;
	}

	return Ar;
}

void UProperty::Write(UPackage &Ar, uint32 len)
{
	if(len < 1)
		return;

	if(DataLength == 0)
	{
		//if(Data)
		//	delete [] Data;
		Data = new int8[DataLength + len];
	}
	else
	{
		/*int8 *NewData = new int8[DataLength + len];
		memcpy(NewData, Data, DataLength);
		delete [] Data;
		Data = NewData;*/
		Data = new int8[DataLength + len];
	}

	Ar.Read(Data + DataLength, len);

	DataLength += len;
}

void UProperty::Write(const void *src, uint32 len)
{
	if(len < 1)
		return;

	if(DataLength == 0)
	{
		if(Data)
			delete [] Data;
		Data = new int8[DataLength + len];
	}
	else
	{
		int8 *NewData = new int8[DataLength + len];
		memcpy(NewData, Data, DataLength);
		delete [] Data;
		Data = NewData;
	}

	memcpy(Data + DataLength, src, len);

	DataLength += len;
}
