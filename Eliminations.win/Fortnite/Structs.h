#pragma once

struct FVector {
	float X;
	float Y;
	float Z;
};

struct FMatrix
{
	float M[4][4];
};
static FMatrix* myMatrix = new FMatrix();

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

class FText {
private:
	char _padding_[0x28];
	PWCHAR Name;
	DWORD Length;
public:


	inline PWCHAR c_wstr() {
		return Name;
	}

	inline char* c_str()
	{

		char sBuff[255];
		wcstombs((char*)sBuff, (const wchar_t*)this->Name, sizeof(sBuff));
		return sBuff;
	}
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? SpoofThis(std::wcslen, other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

class FUObjectItem {
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray {
public:
	FUObjectItem* Objects[9];
};

class GObjects {
public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

namespace StructsParams {
	struct AActor_K2_SetActorLocation_Params
	{
		Vector3										NewLocation;                                              // (Parm, ZeroConstructor, IsPlainOldData)
		bool										bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		int											SweepHitResult;                                           // (Parm, OutParm, IsPlainOldData)
		bool										bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		bool										ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};

	struct AActor_K2_TeleportTo_Params
	{
		Vector3										DestLocation;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		Vector3										DestRotation;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		bool										ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
	};
}