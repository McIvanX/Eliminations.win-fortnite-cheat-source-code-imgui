#pragma once

#define RVA(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define M_PI	3.14159265358979323846264338327950288419716939937510

#define FIRST_PERSON 1


#define Bone_Head 66
#define Bone_Neck 65
#define Bone_Pelvis 2
#define Bone_ChestAtLeft 36
#define Bone_ChestAtRight 8
#define Bone_LeftShoulder 9
#define Bone_RightShoulder 37
#define Bone_LeftElbow 10
#define Bone_RightElbow 38
#define Bone_LeftHand 11
#define Bone_RightHand 39
#define Bone_LeftLeg 67
#define Bone_RightLeg 74
#define Bone_LeftThigh 73
#define Bone_RightThigh 80
#define Bone_LeftFoot 68
#define Bone_RightFoot 75
#define Bone_LeftFeet 71
#define Bone_RightFeet 78
#define Bone_LeftFeetFinger 72
#define Bone_RightFeetFinger 79

namespace FN {

	static GObjects* objects = nullptr;

	static void FreeFN(__int64 address)
	{
		auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(Addresses::Free);

		SpoofThis(func, address);
	}

	static const char* GetObjectName(uintptr_t Object)
	{
		if (Object == NULL)
			return ("");

		auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(Addresses::GetNameByIndex);

		int index = *(int*)(Object + 0x18);

		FString result;
		SpoofThis(fGetObjName, &index, &result);

		if (result.c_str() == NULL)
			return ("");

		auto result_str = result.ToString();

		if (result.c_str() != NULL)
			FreeFN((__int64)result.c_str());

		return result_str.c_str();
	}

	static const char* GetUObjectNameLoop(UObject* Object) {
		std::string name("");

		for (auto i = 0; Object; Object = Object->Outer, ++i) {

			auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(Addresses::GetNameByIndex);

			int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);

			FString internalName;
			SpoofThis(fGetObjName, &index, &internalName);

			if (internalName.c_str() == NULL) {
				break;
			}

			auto objectName = internalName.ToString();


			name = objectName.c_str() + std::string(i > 0 ? xorstr(".") : xorstr("")) + name;
			FreeFN((__int64)internalName.c_str());
		}

		return name.c_str();
	}

	static PVOID FindObject(const char* name) {

		for (auto array : objects->ObjectArray->Objects) {
			auto fuObject = array;
			std::cout << "";
			for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
			{
				std::cout << "";
				auto object = fuObject->Object;

				if (object->ObjectFlags != 0x41) {
					continue;
				}
				std::cout << "";

				if (strstr(GetUObjectNameLoop(object), name)) {
					return object;
				}
			}
		}

		return 0;
	}

	static PVOID FindObject2(const char* name, const char* name2) {

		for (auto array : objects->ObjectArray->Objects) {
			auto fuObject = array;
			std::cout << "";
			for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject)
			{
				std::cout << "";
				auto object = fuObject->Object;

				if (object->ObjectFlags != 0x41) {
					continue;
				}
				std::cout << "";

				if ((strstr(GetUObjectNameLoop(object), name)) and (strstr(GetUObjectNameLoop(object), name2))) {
					return object;
				}
			}
		}

		return 0;
	}

	static BOOL IsNotInScreen(ImVec2 Pos) {
		if (((Pos.x <= 0 or Pos.x > Renderer_Defines::Width) and (Pos.y <= 0 or Pos.y > Renderer_Defines::Height)) or ((Pos.x <= 0 or Pos.x > Renderer_Defines::Width) or (Pos.y <= 0 or Pos.y > Renderer_Defines::Height))) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	static ImVec2 ProjectWorldLocationToScreen(Vector3 world_location)
	{
		if (!world_location.IsAValidVector3()) return  { 0, 0 };
		if (!Addresses::CameraLocation.IsAValidVector3()) return  { 0, 0 };
		if (!Addresses::CameraRotation.IsAValidVector3()) return  { 0, 0 };
		if (!Addresses::FOVAngle) return  { 0, 0 };
		ImVec2 screen_location;
		float delta[3];
		float sp = 0, cp = 0, sy = 0, cy = 0, sr = 0, cr = 0;
		float axisx[3];
		float axisy[3];
		float axisz[3];
		float transformed[3];

		screen_location.x = 0;
		screen_location.y = 0;

		delta[0] = world_location.x - Addresses::CameraLocation.x;
		delta[1] = world_location.y - Addresses::CameraLocation.y;
		delta[2] = world_location.z - Addresses::CameraLocation.z;

		sp = SpoofThis(sinf, float(Addresses::CameraRotation.x * M_PI / 180));
		cp = SpoofThis(cosf, float(Addresses::CameraRotation.x * M_PI / 180));
		sy = SpoofThis(sinf, float(Addresses::CameraRotation.y * M_PI / 180));
		cy = SpoofThis(cosf, float(Addresses::CameraRotation.y * M_PI / 180));
		sr = SpoofThis(sinf, float(Addresses::CameraRotation.z * M_PI / 180));
		cr = SpoofThis(cosf, float(Addresses::CameraRotation.z * M_PI / 180));

		axisx[0] = cp * cy;
		axisx[1] = cp * sy;
		axisx[2] = sp;

		axisy[0] = sr * sp * cy - cr * sy;
		axisy[1] = sr * sp * sy + cr * cy;
		axisy[2] = -sr * cp;

		axisz[0] = -(cr * sp * cy + sr * sy);
		axisz[1] = cy * sr - cr * sp * sy;
		axisz[2] = cr * cp;

		transformed[0] = delta[0] * axisy[0] + delta[1] * axisy[1] + delta[2] * axisy[2];
		transformed[1] = delta[0] * axisz[0] + delta[1] * axisz[1] + delta[2] * axisz[2];
		transformed[2] = delta[0] * axisx[0] + delta[1] * axisx[1] + delta[2] * axisx[2];

		if (transformed[2] < 1)
			transformed[2] = 1;

		if (transformed[2] / 100 > 0)
		{
			float tmpx = 0, tmpy = 0;

			tmpx = (float)(Renderer_Defines::Width / 2);
			tmpy = (float)(Renderer_Defines::Height / 2);

			screen_location.x = tmpx + transformed[0] * tmpx / SpoofThis(tanf, float(Addresses::FOVAngle * M_PI / 360)) / transformed[2];
			screen_location.y = tmpy - transformed[1] * tmpx / SpoofThis(tanf, float(Addresses::FOVAngle * M_PI / 360)) / transformed[2];

			return screen_location;
		}

		return { 0, 0 };
	}

	static BOOL ProcessEvent(uintptr_t address, void* fnobject, void* parms)
	{
		if (!Memory::IsValidPointer(address)) return FALSE;
		auto index = *reinterpret_cast<void***>(address); if (!index) return FALSE;
		auto PEFN = static_cast<void(*)(void*, void*, void*)>(index[0x44]); if (!PEFN) return FALSE;
		SpoofThis(PEFN, (void*)address, (void*)fnobject, (void*)parms);
		return TRUE;
	}
}

class APlayerState {
public:

	uintptr_t TeamIndex() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return *(uintptr_t*)(this + 0xED0);
	}

	std::string GetPlayerName() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return "";
		FString PlayerName;
		FN::ProcessEvent((uintptr_t)this, ObjectsAddresses::GetPlayerName, &PlayerName);
		if (!PlayerName.IsValid()) return "";
		return PlayerName.ToString();
	}
};

class ARootComponent {
public:

	Vector3 RelativeLocation() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return { 0, 0, 0 };
		return *(Vector3*)((uintptr_t)this + 0x11c);
	}

	Vector3 RelativeRotation() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return { 0, 0, 0 };
		return *(Vector3*)((uintptr_t)this + 0x128);
	}
};

class AFortItemDefinition {
public:

	FText* DisplayName() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return reinterpret_cast<FText*>(*(uintptr_t*)(this + 0x88));
	}
};

class AFortWeapon {
public:

	AFortItemDefinition* WeaponData() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (AFortItemDefinition*)(*(uintptr_t*)(this + 0x378));
	}
};


class AFortPawn {
public:
	
	const char* ObjectName() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return "";
		return SpoofThis(FN::GetObjectName, (uintptr_t)this);
	}

	uintptr_t Mesh() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return *(uintptr_t*)((uintptr_t)this + 0x280);
	}

	AFortWeapon* CurrentWeapon() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (AFortWeapon*)(*(uintptr_t*)(this + 0x600));
	}

	ARootComponent* RootComponent() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (ARootComponent*)(*(uintptr_t*)(this + 0x130));
	}


	APlayerState* PlayerState() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (APlayerState*)(*(uintptr_t*)(this + 0x240));
	}

	BOOL IsDBNO() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return *(bool*)((uintptr_t)this + 0x57E);
	}

	BOOL IsDying() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return *(bool*)((uintptr_t)this + 0x540);
	}

	BOOL IsPlayer() {
		if (strstr(ObjectName(), xorstr("PlayerPawn_Athena_"))) return TRUE;
		else return FALSE;
	}

	Vector3 GetBone(int boneidx)
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return { 0, 0, 0 };
		Vector3 result;
		auto fGetBoneMatrix = ((FMatrix * (__fastcall*)(uintptr_t, FMatrix*, int))(Addresses::BoneMatrix));
		SpoofThis(fGetBoneMatrix, Mesh(), myMatrix, boneidx);

		result.x = myMatrix->M[3][0];
		result.y = myMatrix->M[3][1];
		result.z = myMatrix->M[3][2];

		return result;
	}

	Vector3 AimAtRotation(Vector3 LocalRelativeLocation)
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return { 0, 0, 0 };
		Vector3 RetVector = { 0,0,0 };

		Vector3 TargetBone = GetBone(Settings::AimBone);

		if (TargetBone.x == 0 && TargetBone.y == 0) return Vector3(0, 0, 0);

		Vector3 VectorPos = TargetBone - Addresses::CameraLocation;

		RetVector.x = -((SpoofThis(acosf, VectorPos.z / (float)(VectorPos.Length())) * (float)(180.0f / M_PI)) - 90.f);
		RetVector.y = SpoofThis(atan2f, VectorPos.y, VectorPos.x) * (float)(180.0f / M_PI);

		return RetVector;
	}
	
	AFortPawn* GetVehicle() {

		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		uintptr_t result;

		FN::ProcessEvent((uintptr_t)this, ObjectsAddresses::GetVehicle, &result);

		return (AFortPawn*)result;
	}

	struct AFortPlayerPawn_ServerReviveFromDBNO_Params // need to put this out of structparams cuz the AController would be StructParams::AController and will give an error in the function code.
	{
		const class APlayerController* EventInstigator;                                          // (Parm, ZeroConstructor, IsPlainOldData)
	};

	void ServerReviveFromDBNO(const class APlayerController* EventInstigator)
	{
		if (Memory::IsValidPointer((uintptr_t)this)) return;
		AFortPlayerPawn_ServerReviveFromDBNO_Params params;
		params.EventInstigator = EventInstigator;
		FN::ProcessEvent((uintptr_t)this, ObjectsAddresses::ServerReviveFromDBNO, &params);
	}

	BOOL K2_SetActorLocation(const Vector3& NewLocation, bool bSweep, bool bTeleport)
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		StructsParams::AActor_K2_SetActorLocation_Params params;
		params.NewLocation = NewLocation;
		params.bSweep = bSweep;
		params.bTeleport = bTeleport;

		FN::ProcessEvent((uintptr_t)this, ObjectsAddresses::K2_SetActorLocation, &params);

		return params.ReturnValue;
	}

	BOOL K2_TeleportTo(const Vector3& DestLocation, const Vector3& DestRotation)
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		StructsParams::AActor_K2_TeleportTo_Params params;
		params.DestLocation = DestLocation;
		params.DestRotation = DestRotation;

		FN::ProcessEvent((uintptr_t)this, ObjectsAddresses::K2_TeleportTo, &params);

		return params.ReturnValue;
	}

};

class APlayerCameraManager {
public:

	float GetFOVAngle()
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		auto GetFOVAngle = reinterpret_cast<float(*)(UINT64, char*)>(*(uintptr_t*)(*(uintptr_t*)uintptr_t(this) + 0x6D0));
		return SpoofThis(GetFOVAngle, (uintptr_t)this, (char*)0);
	}



	bool GetPlayerViewPoint(Vector3* vCameraPos, Vector3* vCameraRot)
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;

		static uintptr_t pGetPlayerViewPoint = 0;
		if (!pGetPlayerViewPoint)
		{
			uintptr_t VTable = *(uintptr_t*)this;
			if (!VTable)  return false;

			pGetPlayerViewPoint = *(uintptr_t*)(VTable + 0x708);
			if (!pGetPlayerViewPoint)  return false;
		}

		auto GetPlayerViewPoint = reinterpret_cast<void(__fastcall*)(uintptr_t, Vector3*, Vector3*)>(pGetPlayerViewPoint);

		SpoofThis(GetPlayerViewPoint, (uintptr_t)this, vCameraPos, vCameraRot);

		return true;
	}
};

class APlayerController {
public:

	AFortPawn* LocalPawn() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (AFortPawn*)(*(uintptr_t*)(this + 0x2A0));
	}

	APlayerCameraManager* PlayerCameraManager() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (APlayerCameraManager*)(*(uintptr_t*)(this + 0x2B8));
	}

	BOOL IsAiming() {
		return NtGetAsyncKeyState(VK_RBUTTON);
	}

	BOOL IsVisible(PVOID TargetActor) {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		Vector3 ViewPoint;
		auto LineOfSight = reinterpret_cast<bool(__fastcall*)(PVOID PlayerController, PVOID TargetActor, Vector3 * ViewPoint)>(Addresses::LineOfSightTo);
		return SpoofThis(LineOfSight, (PVOID)this, TargetActor, &ViewPoint);
	}

	BOOL ClientSetCameraMode(int CameraMode) {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		auto ClientSetCameraMode = (*(void(__fastcall**)(uintptr_t PlayerController, int h))(*(uintptr_t*)(uintptr_t)(this) + 0x940));
		SpoofThis(ClientSetCameraMode, (uintptr_t)this, CameraMode);
		return TRUE;
	}

	ImVec2 WorldToScreen(Vector3 WorldLocation) {
		if (!Memory::IsValidPointer((uintptr_t)this)) return { 0, 0 };
		return FN::ProjectWorldLocationToScreen(WorldLocation);
	}

	BOOL SetControlRotationTo(Vector3 AimAtRotation)
	{
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(*(uintptr_t*)(uintptr_t)(this) + 0x688));
		SpoofThis(SetControlRotation_, (uintptr_t)this, AimAtRotation, false);
		return TRUE;
	}

	BOOL FOV(float NewFOV) {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		FN::ProcessEvent((uintptr_t)this, ObjectsAddresses::FOV, &NewFOV);
		return TRUE;
	}
};

class ULocalPlayer {
public:
	APlayerController* PlayerController() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (APlayerController*)(*(uintptr_t*)(this + 0x30));
	}
};

class ULocalPlayers {
public:
	ULocalPlayer* LocalPlayer() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (ULocalPlayer*)(*(uintptr_t*)(this));
	}
};

class UGameInstance {
public:
	ULocalPlayers* LocalPlayers() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (ULocalPlayers*)(*(uintptr_t*)(this + 0x38));
	}
};

class AActors {
public:

	AFortPawn* GetCurrentActor(int i) {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (AFortPawn*)(*(uintptr_t*)(this + i * sizeof(uintptr_t)));
	}
};

class ULevel {
public:
	AActors* Actors() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (AActors*)(*(uintptr_t*)(this + 0x98));
	}

	int ActorCount() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (int)(*(uintptr_t*)(this + 0xA0));
	}
};

class UWorld {
public:

	UGameInstance* OwningGameInstance() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (UGameInstance*)(*(uintptr_t*)(this + 0x180));
	}

	ULevel* PersistentLevel() {
		if (!Memory::IsValidPointer((uintptr_t)this)) return 0;
		return (ULevel*)(*(uintptr_t*)(this + 0x30));
	}
};


namespace Addresses {
	static APlayerController* PlayerController;
	static AFortPawn* LocalPawn;
}