#pragma once

namespace Addresses {
	static uintptr_t UWorld = 0;
	static uintptr_t Free = 0;
	static uintptr_t GetNameByIndex = 0;
	static uintptr_t BoneMatrix = 0;
	static uintptr_t ProjectWorldToScreen = 0;
	static uintptr_t LineOfSightTo = 0;

	static Vector3 CameraLocation;
	static Vector3 CameraRotation;
	static float FOVAngle;
}

namespace ObjectsAddresses {
	static PVOID FOV;
	static PVOID GetPlayerName;
	static PVOID SetPawnVisibility;
	static PVOID IsInVehicle;
	static PVOID GetVehicle;
	static PVOID K2_SetActorLocation;
	static PVOID K2_TeleportTo;
	static PVOID ServerReviveFromDBNO;
}