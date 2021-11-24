#pragma once

namespace Aimbot {

	static BOOL Initialize(AFortPawn* TargetPawn) {

		if (Addresses::LocalPawn and TargetPawn) {

			Vector3 LocalPawnRelativeLocation = Addresses::LocalPawn->RootComponent()->RelativeLocation();

			uintptr_t LocalTeamIndex = Addresses::LocalPawn->PlayerState()->TeamIndex();

			uintptr_t CurrentPawnTeamIndex = TargetPawn->PlayerState()->TeamIndex();


			if (Settings::TPToAimingPlayer and NtGetAsyncKeyState(VK_F8)) {
				Addresses::LocalPawn->K2_SetActorLocation(TargetPawn->RootComponent()->RelativeLocation(), false, false);
			}


			if (Settings::MemoryAim and NtGetAsyncKeyState(VK_RBUTTON) and (LocalTeamIndex != CurrentPawnTeamIndex)) {
				Vector3 AimAtRotation = TargetPawn->AimAtRotation(LocalPawnRelativeLocation);

				Addresses::PlayerController->SetControlRotationTo(AimAtRotation);
			}
		}

		return TRUE;
	}
}