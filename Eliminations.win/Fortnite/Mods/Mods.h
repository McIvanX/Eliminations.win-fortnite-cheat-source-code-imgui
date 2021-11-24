#pragma once

namespace Mods {

	static bool NoSpreadInitialized = false;
	static bool CalculateShotInitialized = false;

	static AFortPawn* TargetPawn = 0;
	static uintptr_t SpreadCaller = 0;
	static BOOL(*SpreadOriginal)(uintptr_t a1, float* a2, float* a3);
	static BOOL SpreadHook(uintptr_t a1, float* a2, float* a3)
	{
		if (Settings::NoSpread && _ReturnAddress() == (void*)SpreadCaller && NtGetAsyncKeyState(VK_RBUTTON)) {
			return 0;
		}

		return SpoofThis(SpreadOriginal, a1, a2, a3);
	}

	static float* (*CalculateShotOriginal)(uintptr_t, uintptr_t, uintptr_t) = nullptr;


	static float* CalculateShotHook(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2) {
		auto ret = SpoofThis(CalculateShotOriginal, arg0, arg1, arg2);

		if (ret && Settings::SilentAim && TargetPawn && Addresses::LocalPawn)
		{

			Vector3 head = TargetPawn->GetBone(Settings::AimBone);

			
			Vector3 LocalPawnRelativeLocation = Addresses::LocalPawn->RootComponent()->RelativeLocation();


			Vector3* LocalPawnRelativeLocation_check = &LocalPawnRelativeLocation;
			if (!LocalPawnRelativeLocation_check) return ret;

			auto dx = head.x - LocalPawnRelativeLocation.x;
			auto dy = head.y - LocalPawnRelativeLocation.y;
			auto dz = head.z - LocalPawnRelativeLocation.z;
			if (dx * dx + dy * dy + dz * dz < 125000.0f) {
				ret[4] = head.x;
				ret[5] = head.y;
				ret[6] = head.z;
			}
			else {
				head.z -= 16.0f;
				LocalPawnRelativeLocation.z += 45.0f;

				auto y = SpoofThis(atan2f, head.y - LocalPawnRelativeLocation.y, head.x - LocalPawnRelativeLocation.x);

				LocalPawnRelativeLocation.x += SpoofThis(cosf, y + 1.5708f) * 32.0f;
				LocalPawnRelativeLocation.y += SpoofThis(sinf, y + 1.5708f) * 32.0f;

				auto length = SpoofThis(sqrtf, SpoofThis(powf, head.x - LocalPawnRelativeLocation.x, (float)2) + SpoofThis(powf, head.y - LocalPawnRelativeLocation.y, (float)2));
				auto x = -SpoofThis(atan2f, head.z - LocalPawnRelativeLocation.z, length);
				y = SpoofThis(atan2f, head.y - LocalPawnRelativeLocation.y, head.x - LocalPawnRelativeLocation.x);

				x /= 2.0f;
				y /= 2.0f;

				ret[0] = -(SpoofThis(sinf, x) * SpoofThis(sinf, y));
				ret[1] = SpoofThis(sinf, x) * SpoofThis(cosf, y);
				ret[2] = SpoofThis(cosf, x) * SpoofThis(sinf, y);
				ret[3] = SpoofThis(cosf, x) * SpoofThis(cosf, y);
			}
		}

		static bool MagicBulletEnabled = true;

		if (MagicBulletEnabled && NtGetAsyncKeyState(VK_F6)) {
			Settings::MagicBullet = true;	
			MagicBulletEnabled = true;
		}

		if (!MagicBulletEnabled && NtGetAsyncKeyState(VK_F6)) {
			Settings::MagicBullet = false;
			MagicBulletEnabled = false;
		}


		if (ret && Settings::MagicBullet && Addresses::LocalPawn && TargetPawn) {

			Vector3 head = TargetPawn->GetBone(Settings::AimBone);

			Vector3 LocalPawnRelativeLocation = Addresses::LocalPawn->RootComponent()->RelativeLocation();


			Vector3* LocalPawnRelativeLocation_check = &LocalPawnRelativeLocation;
			if (!LocalPawnRelativeLocation_check) return ret;


			//head -= {15.0f, 15.0f, 15.0f};

			auto y_tmp = SpoofThis(atan2f, head.y - LocalPawnRelativeLocation.y, head.x - LocalPawnRelativeLocation.x);

			LocalPawnRelativeLocation.x += SpoofThis(cosf, y_tmp + 1.5708f) * 32.0f;
			LocalPawnRelativeLocation.y += SpoofThis(sinf, y_tmp + 1.5708f) * 32.0f;
			LocalPawnRelativeLocation.z += 45.0f;

			auto length = SpoofThis(sqrtf, SpoofThis(powf, head.x - LocalPawnRelativeLocation.x, (float)2) + SpoofThis(powf, head.y - LocalPawnRelativeLocation.y, (float)2));
			Vector3 NewPos;

			NewPos.x = -SpoofThis(atan2f, head.z - LocalPawnRelativeLocation.z, length);
			NewPos.y = SpoofThis(atan2f, head.y - LocalPawnRelativeLocation.y, head.x - LocalPawnRelativeLocation.x);
			NewPos.z = 0;

			NewPos.x /= 2.0f;
			NewPos.y /= 2.0f;

			ImVec2 HeadAfterEdit_W2S = Addresses::PlayerController->WorldToScreen(head);

			auto p_dx = HeadAfterEdit_W2S.x - (Renderer_Defines::Width / 2);
			auto p_dy = HeadAfterEdit_W2S.y - (Renderer_Defines::Height / 2);
			auto p_dist = SpoofThis(sqrtf, p_dx * p_dx + p_dy * p_dy);

			if (p_dist < Settings::FovCircle_Value) {
				ret[0] = -(SpoofThis(sinf, NewPos.x) * SpoofThis(sinf, NewPos.y));
				ret[1] = SpoofThis(sinf, NewPos.x) * SpoofThis(cosf, NewPos.y);
				ret[2] = SpoofThis(cosf, NewPos.x) * SpoofThis(sinf, NewPos.y);
				ret[3] = SpoofThis(cosf, NewPos.x) * SpoofThis(cosf, NewPos.y);
				ret[4] = head.x;
				ret[5] = head.y;
				ret[6] = head.z;
			}

		}

		return ret;
	}

	static BOOL InitializeHooks(bool NoSpread, bool CalcShot) {
		if (!NoSpreadInitialized) {
			if (NoSpread) {
				auto SpreadAddr = Memory::PatternScan(xorstr("E8 ? ? ? ? 48 8D 4B 28 E8 ? ? ? ? 48 8B C8"));
				SpreadAddr = RVA(SpreadAddr, 5);
				HukHelper::InsertHook(SpreadAddr, (uintptr_t)SpreadHook, (uintptr_t)&SpreadOriginal);
				SpreadCaller = Memory::PatternScan(xorstr("0F 57 D2 48 8D 4C 24 ? 41 0F 28 CC E8 ? ? ? ? 48 8B 4D B0 0F 28 F0 48 85 C9"));
				NoSpreadInitialized = true;
			}
		}
		if (!CalculateShotInitialized) {
			if (CalcShot) {
				auto CalcShotAddr = Memory::PatternScan(xorstr("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F1"));
				HukHelper::InsertHook(CalcShotAddr, (uintptr_t)CalculateShotHook, (uintptr_t)&CalculateShotOriginal);
				CalculateShotInitialized = true;
			}
		}
		return TRUE;
	}

	static bool DBNO_AlreadyDisabled = true;
	static bool InstantRevive_AlreadyDisabled;

	static BOOL Initialize(AFortPawn* TP_Pawn, AFortPawn* ClosestPing, AFortPawn* ClosestPawn, AFortPawn* TargetBullet) {

		if (Settings::FovChanger) {
			Addresses::PlayerController->FOV(Settings::FovChanger_Value);
		}

		if (SpoofThis(Memory::IsValidPointer, uintptr_t(Addresses::LocalPawn))) {


			if (Settings::FirstCamera) {
				Addresses::PlayerController->ClientSetCameraMode(FIRST_PERSON);
				Settings::FirstCamera = false;
			}

			if (Settings::InstantRevive) {
				*(float*)(PBYTE)(Addresses::LocalPawn + 0x3588) = 0.101f;
				InstantRevive_AlreadyDisabled = false;
			}
			else {
				if (!InstantRevive_AlreadyDisabled) {
					*(float*)(PBYTE)(Addresses::LocalPawn + 0x3588) = 10.0f;
					InstantRevive_AlreadyDisabled = true;
				}
			}

				if (Settings::SelfRevive) {
				Addresses::LocalPawn->ServerReviveFromDBNO(Addresses::PlayerController);
			}
			

			if (Settings::NoSpread) {
				if (!NoSpreadInitialized) {
					InitializeHooks(true, false);
				}
			}




			AFortPawn* CurrentVehicle = Addresses::LocalPawn->GetVehicle();
			

			if (Settings::CarFly) {
				if (!CurrentVehicle) return false;
				if (NtGetAsyncKeyState(0x57)) {
					Vector3 CarRelativeLocation = CurrentVehicle->RootComponent()->RelativeLocation();

					CurrentVehicle->K2_TeleportTo(CarRelativeLocation, Addresses::CameraRotation);
				}
			}

			if (Settings::TP_Locations_Start) {
				if (!CurrentVehicle) return false;

				Vector3 LazyLake = { -49784.277344, 52951.078125, 3965.149902 };
				Vector3 WeepingWoods = { -27864.421875, -22980.433594, -869.825256 };
				Vector3 RetailRow = { -38222.121094, 98802.531250, 5805.213379 };
				Vector3 SlurpySwamp = { -66213.726562, -44544.425781, -1883.901611 };
				Vector3 BoneyBurbs = { 13133.120117, -20437.457031, -338.191010 };
				Vector3 PleasantPark = { 58242.730469, -19971.255859, -1146.243042 };
				Vector3 PreLobby = { -169557.593750, -179321.203125, -3126.227539 };
				Vector3 MiniGameLoot = { -59077.390625, 368913.875000, 47701.375000 }; //OLD = -59486.101562, 417433.500000, 50701.371094

				if (Settings::TP_Locations == 0) {
					CurrentVehicle->K2_TeleportTo(LazyLake, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 1) {
					CurrentVehicle->K2_TeleportTo(WeepingWoods, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 2) {
					CurrentVehicle->K2_TeleportTo(RetailRow, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 3) {
					CurrentVehicle->K2_TeleportTo(SlurpySwamp, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 4) {
					CurrentVehicle->K2_TeleportTo(BoneyBurbs, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 5) {
					CurrentVehicle->K2_TeleportTo(PleasantPark, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 6) {
					CurrentVehicle->K2_TeleportTo(PreLobby, Addresses::CameraRotation);
				}
				else if (Settings::TP_Locations == 7) {
					CurrentVehicle->K2_TeleportTo(MiniGameLoot, Addresses::CameraRotation);
				}
				Settings::TP_Locations_Start = false;
			}

			if (Settings::CarTpToPing) {
				if (!CurrentVehicle) return false;
				if (ClosestPing)
				{
					if (NtGetAsyncKeyState(VK_F6))
					{
						Vector3 PingRelativeLocation = ClosestPing->RootComponent()->RelativeLocation();
						CurrentVehicle->K2_TeleportTo(PingRelativeLocation, Addresses::CameraRotation);
					}
				}
			}

			if (Settings::CarTpToPlayer) {
				if (!CurrentVehicle) return false;
				if (TP_Pawn) {
					if (NtGetAsyncKeyState(VK_F7))
					{
						Vector3 TargetPawnRelativeLocation = TP_Pawn->RootComponent()->RelativeLocation();
						CurrentVehicle->K2_TeleportTo(TargetPawnRelativeLocation, Addresses::CameraRotation);
					}
				}
			}

		}


		return TRUE;
	}
}