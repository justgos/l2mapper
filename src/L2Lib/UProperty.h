#pragma once

// --------------------------------------------------------------------
// Property types
// --------------------------------------------------------------------
#define UNP_ByteProperty        1
#define UNP_IntProperty         2
#define UNP_BoolProperty        3
#define UNP_FloatProperty       4
#define UNP_ObjectProperty      5
#define UNP_NameProperty        6
#define UNP_StringProperty      7
#define UNP_ClassProperty       8
#define UNP_ArrayProperty       9
#define UNP_StructProperty      10
#define UNP_VectorProperty      11
#define UNP_RotatorProperty     12
#define UNP_StrProperty         13
#define UNP_MapProperty         14
#define UNP_FixedArrayProperty  15

#define PROPERTY_TYPE_MASK  0x0f
#define PROPERTY_SIZE_MASK  0x70
#define PROPERTY_ARRAY_MASK 0x80

class UProperty
{
public:
	char *Name;
	char *StructType;
	uint32 Size;
	int32 ArrayIndex;
	bool IsArray;
	int32 ArrayLength;
	int8 Type;
	uint32 DataLength;
	int8 *Data;
private:
	void ReadArrayIndex(UPackage &Ar);
	void ReadStruct(UPackage &Ar);
	void Write(UPackage &Ar, uint32 len);
	void Write(const void *src, uint32 len);
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UProperty);
public:
	UProperty(void);
	~UProperty(void);
};

