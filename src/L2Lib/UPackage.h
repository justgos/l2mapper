#pragma once

#include "UBase.h"

#include "_shader.h"

#define g_shader ::Singleton<_shader>::get_instance()

#define U_SEEK_SET SEEK_SET
#define U_SEEK_CUR SEEK_CUR
#define U_SEEK_END SEEK_END

struct UGeneration
{
	uint32 ExportCount;
	uint32 NameCount;
};

struct UName
{
	char *Name;
	int32 Flags;
};

struct UExport
{
	int32 iClass;
	int32 iSuper;
	char *BaseClass;
	int32 Package;
	char *Name;
	int32 Flags;
	int32 Size;
	int32 Offset;

	UObject *ObjectHandle;

	UExport()
	{
		ObjectHandle = 0;
	}
};

struct UImport
{
	int32 ClassPackage;
	int32 ClassName;
	int32 Package;
	char *BasePackage;
	char *Name;
};

class DLLEXPORT UIndex
{
public:
	int32 &Reference;
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UIndex);
public:
	UIndex(int32 &idx) : Reference(idx) {};
	~UIndex(void) {};
};

template <typename T, typename T1>
class DLLEXPORT jfArray
{
public:
	__forceinline void add(T el)
	{
		realloc(num_elems + 1);
		_elems[num_elems] = el;
		num_elems++;
	}
	__forceinline void set(T1 n, T el)
	{
		if(n >= _size)
		{
			realloc(n + 1);
		}
		_elems[n] = el;
		num_elems++;
	}
	__forceinline void alloc(T1 size)
	{
		_elems = new T[size];
		num_elems = 0;
		_size = size;
	}
	__forceinline void realloc(T1 size)
	{
		T *new_elems = new T[size];
		if(_size > 0)
			memcpy(new_elems, _elems, (size < _size ? size : _size) * sizeof(T));
			//memcpy(new_elems, _elems, min(_size, size) * sizeof(T));
		delete [] _elems;
		_elems = new_elems;
		_size = size;
	}
	void clone(jfArray<T, T1> *target)
	{
		target->alloc(num_elems);
		memcpy(target->_elems, _elems, num_elems * sizeof(T));
		target->num_elems = num_elems;
	}
	void clear()
	{
		if(_elems)
			delete [] _elems;
		_elems = 0;
		num_elems = 0;
		_size = 0;
	}
	void remove(T1 n)
	{
		for(T1 i = n; i < num_elems - 1; i++)
			_elems[i] = _elems[i + 1];
		num_elems--;
	}
	__forceinline T& operator[](T1 n)
	{
		return *(_elems + n);
	}
	__forceinline T1 Size()
	{
		return _size;
	}
public:
	UPackage& Serialize(UPackage& Ar)
	{
		int32 Count;
		Ar >> UIndex(Count);
		alloc(Count);
		for(int32 i = 0; i < Count; i++)
			Ar >> _elems[i];
		return Ar;
	}
	IMPLEMENT_USERIALIZER(jfArray);
public:
	T *_elems;
	T1 num_elems;
	T1 _size;
public:
	jfArray(void)
	{
		_elems = 0;
		num_elems = 0;
		_size = 0;
	}
	~jfArray(void)
	{
		if(_elems)
			delete [] _elems;
	};
};

template <typename T, typename T1>
class DLLEXPORT jfArray2
{
public:
	__forceinline void add(T el)
	{
		realloc(num_elems + 1);
		_elems[num_elems] = el;
		num_elems++;
	}
	__forceinline void set(T1 n, T el)
	{
		if(n >= _size)
		{
			realloc(n + 1);
		}
		_elems[n] = el;
		num_elems++;
	}
	__forceinline void alloc(T1 size)
	{
		_elems = new T[size];
		num_elems = 0;
		_size = size;
	}
	__forceinline void realloc(T1 size)
	{
		T *new_elems = new T[size];
		if(_size > 0)
			memcpy(new_elems, _elems, min(_size, size) * sizeof(T));
		delete [] _elems;
		_elems = new_elems;
		_size = size;
	}
	void clone(jfArray<T, T1> *target)
	{
		target->alloc(num_elems);
		memcpy(target->_elems, _elems, num_elems * sizeof(T));
		target->num_elems = num_elems;
	}
	void clear()
	{
		if(_elems)
			delete [] _elems;
		_elems = 0;
		num_elems = 0;
		_size = 0;
	}
	void remove(T1 n)
	{
		for(T1 i = n; i < num_elems - 1; i++)
			_elems[i] = _elems[i + 1];
		num_elems--;
	}
	__forceinline T& operator[](T1 n)
	{
		return *(_elems + n);
	}
	__forceinline T1 Size()
	{
		return _size;
	}
public:
	UPackage& Serialize(UPackage& Ar)
	{
		int32 Count;
		Ar >> UIndex(Count);
		alloc(Count);
		Ar.Read(_elems, Count * sizeof(T));
		return Ar;
	}
	IMPLEMENT_USERIALIZER(jfArray2);
public:
	T *_elems;
	T1 num_elems;
	T1 _size;
public:
	jfArray2(void)
	{
		_elems = 0;
		num_elems = 0;
		_size = 0;
	}
	~jfArray2(void)
	{
		if(_elems)
			delete [] _elems;
	};
};

class DLLEXPORT UString
{
public:
	char *Txt;
public:
	int32 Length()
	{
		if(!Txt)
			return 0;
		return strlen(Txt);
	}
public:
	UPackage& Serialize(UPackage& Ar);
	IMPLEMENT_USERIALIZER(UString);
public:
	UString()
	{
		Txt = 0;
	}

	~UString()
	{
		if(Txt)
			delete Txt;
	}
};

class DLLEXPORT UPackage
{
private:
	struct UPkgHeader
	{
		uint32 Version;
		int32 Flags;

		uint32 NameCount;
		uint32 NameOffset;
		uint32 ExportCount;
		uint32 ExportOffset;
		uint32 ImportCount;
		uint32 ImportOffset;

		uint32 HeritageCount;
		uint32 HeritageOffset;

		//DWORD generation_count;
		jfArray<UGeneration, uint32> Generations;

		int8 GUID[16];
	};
public:
	char *m_Name;
	char *m_FileAddr;
	UPkgHeader m_Header;
	bool IsEncrypted;
	bool m_IsLoaded;
	int8 CryptKey;
	uint32 ContentOffset;
public:
	jfArray<UName, uint32> Names;
	jfArray<UExport, uint32> Exports;
	jfArray<UImport, uint32> Imports;

	class UPackageManager *PkgMgr;
private:
	FILE *_f;
public:
	void SetFile(char *fileName);
	void Load();
	void Open(char *fileName);
	void Read(void *dest, uint32 len);
	uint32 Tell();
	void Seek(uint32 offset, int8 origin = U_SEEK_CUR);
	int32 Ver() { return m_Header.Version; }
	const char *Name() { return m_Name; }
	bool InLoaded() { return m_IsLoaded; }
	char *IndexToName(int32 idx);
	UObject *GetUObject(char *name);

	void TestDump(uint32 Len, bool ToScreen);
public:
	UObject *SerializeObject(UExport &ObjExport);

	UPackage& operator >> (class UObject*& Res);

	friend inline UPackage& operator >> (UPackage& Ar, int8& C)
	{
		Ar.Read(&C, 1);
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, uint8& B)
	{
		Ar.Read(&B, 1);
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, uint16& W)
	{
		Ar.Read(&W, sizeof(uint16));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, int16& S)
	{
		Ar.Read(&S, sizeof(int16));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, uint32& D)
	{
		Ar.Read(&D, sizeof(uint32));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, int32& I)
	{
		Ar.Read(&I, sizeof(int32));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, float& F)
	{
		Ar.Read(&F, sizeof(float));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage &Ar, uint64& Q)
	{
		Ar.Read(&Q, sizeof(uint64));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, int64& S)
	{
		Ar.Read(&S, sizeof(int64));
		return Ar;
	}
	friend inline UPackage& operator >> (UPackage& Ar, char **S)
	{
		//uint8 Len;
		int32 Len;
		Ar >> UIndex(Len);
		if(Len <= 0)
			return Ar;
		*S = new char[Len];
		Ar.Read(*S, Len);
		return Ar;
	}
public:
	UPackage(void);
	~UPackage(void);
};
