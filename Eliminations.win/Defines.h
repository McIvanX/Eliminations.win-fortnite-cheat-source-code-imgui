#pragma once

typedef int (WINAPI* NtGetAsyncKeyState_)(DWORD);
static NtGetAsyncKeyState_ NtGetAsyncKeyState;

namespace Renderer_Defines {
	static float Width;
	static float Height;
}



namespace Settings
{
	//Aim
	static bool MemoryAim = 1; //Done
	static bool SilentAim = 0; //Done
	static bool MagicBullet = 0; //Done
	static int AimBone = 66;
	static float MemoryAimSmooth = 0; //Done

	static bool fov360 = 0; //Done
	static float FovCircle_Value = 430; //Done
	
	

	//Player ESP
	static bool BoxESP = 0; //Done
	static bool SkeletonESP = 1; //Done
	static bool LinesESP = 0; //Done
	static bool PlayerNameESP = 0; //Done
	static bool EnemyWeaponESP = 0; //Done
	static bool RadarESP = 0;
	static bool BotAIESP = 0; //Done
	static float RadarESPRange = 7000.f;
	static int RadarESPOpacity = 255;


	//Exploits
	static bool NoSpread = 0; //Done
	static bool InstantRevive = 0;
	static bool SelfRevive = 0;
	static bool AimWhileJumping = 0;
	static bool AirStuck = 0;
	static bool RapidFire = 0;
	static float RapidFireValue = 0.001f;
	
	static bool FirstCamera = 0; //Done

	//Fov Changer
	static bool FovChanger = 0; //Done
	static float FovChanger_Value = 50.f; //Done
	
	//Boat Speed
	static bool VehicleSpeed = 0;
	static float VehicleSpeedValue = 30.f;

	//Car Exploits
	static bool CarFly = 0; //Done
	static bool CarTpToPing = 0; //Done
	static bool CarTpToPlayer = 0; //Done
	static bool TP_Locations_Start = 0; //Done
	static int TP_Locations = 0; //Done


	static bool TPToAimingPlayer = 0; //Done

	static bool BulletTP = 0;


	//Misc

	static float MaxESPDistance = 300; //Done
	static bool ShowFovCircle = true; //Done
	static bool VisibleCheck = true;
	


}

