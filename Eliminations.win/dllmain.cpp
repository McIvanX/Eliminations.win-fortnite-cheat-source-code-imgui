#include "includes.h"
#include "Loot.h"

HRESULT(*PresentOriginal)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;


ID3D11Device*				device = nullptr;
ID3D11DeviceContext*		immediateContext = nullptr;
ID3D11RenderTargetView*		renderTargetView = nullptr;
bool						ShowMenu = true;
static int					tabs = 0;
ImFont*						FontTitle = nullptr;
ImFont*						FontTabs = nullptr;
ImFont*						FontNormal = nullptr;
ImFont*						FontDraw = nullptr;


BOOL TestLoop() {

	ImDrawList* FNDraw = ImGui::GetOverlayDrawList();

	if (Settings::ShowFovCircle and !Settings::fov360)
		FNDraw->AddCircle(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), Settings::FovCircle_Value, ImColor(1.f, 1.f, 1.f, 0.5f), 200, 0.5);

	AFortPawn* ClosestPawn = 0;
	float ClosestDistance = FLT_MAX;
	AFortPawn* TP_Pawn = 0;

	AFortPawn* ClosestPing = 0;
	AFortPawn* TargetBullet = 0;

	uintptr_t GWorld = *(uintptr_t*)(Addresses::UWorld); if (!GWorld) return false;
	UWorld* World = (UWorld*)GWorld;

	Addresses::PlayerController = World->OwningGameInstance()->LocalPlayers()->LocalPlayer()->PlayerController();

	APlayerCameraManager* PlayerCameraManager = Addresses::PlayerController->PlayerCameraManager();

	PlayerCameraManager->GetPlayerViewPoint(&Addresses::CameraLocation, &Addresses::CameraRotation);
	

	Addresses::FOVAngle = PlayerCameraManager->GetFOVAngle();

	Addresses::LocalPawn = Addresses::PlayerController->LocalPawn();

	ULevel* PersistentLevel = World->PersistentLevel();

	AActors* Actors = PersistentLevel->Actors();

	int ActorCount = PersistentLevel->ActorCount();






	for (int i = 0; i < ActorCount; i++) {
		AFortPawn* CurrentPawn = Actors->GetCurrentActor(i);
		
		if (CurrentPawn == Addresses::LocalPawn) continue;
		const char* CurrentPawnName = CurrentPawn->ObjectName();

		if (CurrentPawn->IsPlayer()) {

			bool IsVisible = true;
			

			ImColor Color;
			ImColor TextColor;


			ImVec2 Head = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_Head));
			ImVec2 Pelvis = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_Pelvis));

			


			if ((Head.x == 0 or Head.y == 0) or (Head.x == 0 and Head.y == 0)) continue;
			if ((Head.x == 0 or Head.y == 1080) or (Head.x == 0 and Head.y == 1080)) continue;
			if ((Head.x == 1920 or Head.y == 0) or (Head.x == 1920 and Head.y == 0)) continue;
			if ((Head.x == 1920 or Head.y == 1080) or (Head.x == 1920 and Head.y == 1080)) continue;

			if (Memory::IsValidPointer(uintptr_t(Addresses::LocalPawn))) {
				IsVisible = Addresses::PlayerController->IsVisible(CurrentPawn);
			}
			else IsVisible = true;

			if (IsVisible)
				Color = ImColor(255, 255, 0);
			else
				Color = ImColor(255, 0, 0);

			if (IsVisible)
				TextColor = ImColor(255, 255, 255);
			else
				TextColor = ImColor(255, 0, 0);

			if (Settings::BotAIESP && strstr(CurrentPawnName, xorstr("PlayerPawn_Athena_Phoebe_C")))
			{
				ImVec2 Head = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_Head));

				ImGui::DrawOutlinedText("BOT / AI", ImVec2(Head.x, Head.y + 15 - ImGui::CalcTextSize("BOT / AI").y * 2), TextColor);
			}

			ESP::Initialize(FNDraw, CurrentPawn, Head, Pelvis, IsVisible);


			if (Memory::IsValidPointer(uintptr_t(Addresses::LocalPawn))) {

				TP_Pawn = CurrentPawn;

				if (Settings::SilentAim) {
					if (!Mods::CalculateShotInitialized) {
						Mods::InitializeHooks(false, true);
					}
				}

				if (Settings::MagicBullet) {
					if (!Mods::CalculateShotInitialized) {
						Mods::InitializeHooks(false, true);
					}
				}

				auto dx = Head.x - (Renderer_Defines::Width / 2);
				auto dy = Head.y - (Renderer_Defines::Height / 2);
				auto dist = SpoofThis(sqrtf, dx * dx + dy * dy);

				if (Settings::fov360) {
					if (dist < ClosestDistance) {
						ClosestDistance = dist;
						ClosestPawn = CurrentPawn;
					}
					if (FN::IsNotInScreen(Head)) {
						ClosestPawn = CurrentPawn;
					}
				}
				else {
					if (dist < Settings::FovCircle_Value && dist < ClosestDistance) {
						ClosestDistance = dist;
						ClosestPawn = CurrentPawn;
					}
				}



				
			}


		}

		if (strstr(CurrentPawnName, xorstr("AthenaPlayerMarker_WithCustomization")))
		{
			ClosestPing = CurrentPawn;
		}

	}

	if (Memory::IsValidPointer(uintptr_t(Addresses::LocalPawn))) {

		if (ClosestPawn) {
			bool closestPawnVisible = Addresses::PlayerController->IsVisible(ClosestPawn);
			if (Settings::VisibleCheck) {
				if (closestPawnVisible)
					Aimbot::Initialize(ClosestPawn);
			}
			else
			{
				Aimbot::Initialize(ClosestPawn);
			}
		}



		if (NtGetAsyncKeyState(VK_RBUTTON) and ClosestPawn) {
			bool closestPawnVisible = Addresses::PlayerController->IsVisible(ClosestPawn);

			//Silent
			if (Settings::SilentAim and closestPawnVisible) {
				Mods::TargetPawn = ClosestPawn;
			}
			else if (Settings::SilentAim and !closestPawnVisible) {
				Mods::TargetPawn = 0;
			}

			if (Settings::MagicBullet) {
				Mods::TargetPawn = ClosestPawn;
			}
		}
		else {
			Mods::TargetPawn = 0;
		}


	}


	Mods::Initialize(TP_Pawn, ClosestPing, ClosestPawn, TargetBullet);

	return TRUE;
}



namespace ImGui {
	IMGUI_API VOID CustomCheckbox(const char* text, bool& b) {

		ImVec4* colors = ImGui::GetStyle().Colors;

		ImColor FrameBg_Original = colors[ImGuiCol_FrameBg];
		ImColor FrameBgHovered_Original = colors[ImGuiCol_FrameBgHovered];
		ImColor FrameBgActive_Original = colors[ImGuiCol_FrameBgActive];

		colors[ImGuiCol_FrameBg] = ImColor(29, 34, 37, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(29, 34, 37, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(29, 34, 37, 255);

		std::string space = xorstr(" ");

		ImGui::Checkbox((space + text).c_str(), &b);

		colors[ImGuiCol_FrameBg] = FrameBg_Original;
		colors[ImGuiCol_FrameBgHovered] = FrameBgHovered_Original;
		colors[ImGuiCol_FrameBgActive] = FrameBgActive_Original;

	}
}

static const char* TP_Locations_TypeItems[]{
	"   Lazy Lake",
	"   Weeping Woods",
	"   Retail Row",
	"   Slurpy Swamp",
	"   Boney Burbs",
	"   Pleasant Park",
	"   Pre Lobby",
	"   MiniGame Loot"
};


BOOL Menu() {

	if (ShowMenu) {
		ImGui::SetNextWindowPos({ 78, 487 }, ImGuiCond_FirstUseEver | ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(710, 390));
		//ImGui::SetNextWindowSize({ 400, 300 });



		ImGui::GetOverlayDrawList()->AddCircleFilled(ImGui::GetIO().MousePos, 3, ImColor(255, 255, 255, 255), 20);      

		ImVec2 pos;
		ImDrawList* draw;

		ImGui::Begin(xorstr("##mainmenu"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
		{
			pos = ImGui::GetWindowPos();
			draw = ImGui::GetWindowDrawList();

	


			//Main Background
			draw->AddRectFilled(ImVec2(pos.x - 4, pos.y), ImVec2(pos.x + 710, pos.y + 390), ImColor(29, 34, 37), 8.f, 10);


			//Line for Tabs
			draw->AddLine(ImVec2(pos.x + 197, pos.y + 55), ImVec2(pos.x + 197, pos.y + 390 - 20), ImColor(255, 255, 255, 60));

			//Title
			ImGui::PushFont(FontTitle);
			draw->AddText(ImVec2(pos.x + 19, pos.y + 26), ImColor(255, 255, 255), xorstr("ELIMINATIONS.WIN"));
			ImGui::PopFont();

			ImGui::PushFont(FontTabs);

			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Button] = ImColor(29, 34, 37, 0);
			colors[ImGuiCol_ButtonHovered] = ImColor(29, 34, 37, 0);
			colors[ImGuiCol_ButtonActive] = ImColor(29, 34, 37, 0);


			//Aimbot
			ImGui::SetCursorPos(ImVec2(12, 62));
			if (ImGui::Button(xorstr("##aimbot"), ImVec2(172, 31))) tabs = 0;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 69), ImColor(255, 255, 255), xorstr("Aimbot"));

			//Visuals
			ImGui::SetCursorPos(ImVec2(12, 98));
			if (ImGui::Button(xorstr("##visuals"), ImVec2(172, 31))) tabs = 1;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 105), ImColor(255, 255, 255), xorstr("Visuals"));


			//Exploits
			ImGui::SetCursorPos(ImVec2(12, 134));
			if (ImGui::Button(xorstr("##exploits"), ImVec2(172, 31))) tabs = 2;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 141), ImColor(255, 255, 255), xorstr("Exploits"));

			//Teleport
			ImGui::SetCursorPos(ImVec2(12, 170));
			if (ImGui::Button(xorstr("##teleport"), ImVec2(172, 31))) tabs = 3;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 177), ImColor(255, 255, 255), xorstr("Teleport"));

			//Misc
			ImGui::SetCursorPos(ImVec2(12, 206));
			if (ImGui::Button(xorstr("##misc"), ImVec2(172, 31))) tabs = 4;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 213), ImColor(255, 255, 255), xorstr("Misc"));

			//Colors
			ImGui::SetCursorPos(ImVec2(12, 242));
			if (ImGui::Button(xorstr("##colors"), ImVec2(172, 31))) tabs = 5;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 249), ImColor(255, 255, 255), xorstr("Colors"));

			//Settings
			ImGui::SetCursorPos(ImVec2(12, 278));
			if (ImGui::Button(xorstr("##settings"), ImVec2(172, 31))) tabs = 6;
			draw->AddText(ImVec2(pos.x + 22, pos.y + 285), ImColor(255, 255, 255), xorstr("Settings"));

			ImGui::PopFont();

			colors[ImGuiCol_Button] = ImColor(163, 163, 160, 60);
			colors[ImGuiCol_ButtonHovered] = ImColor(163, 163, 160, 100);
			colors[ImGuiCol_ButtonActive] = ImColor(163, 163, 160, 150);


			ImGui::PushFont(FontNormal);

			//Into Tabs
			if (tabs == 0) //Aimbot
			{
				ImGui::BeginGroup();
				draw->AddText(ImVec2(pos.x + 230, pos.y + 34), ImColor(255, 255, 255), xorstr("Aimbot"));

				ImGui::SetCursorPos(ImVec2(230, 70));
				ImGui::CustomCheckbox(xorstr("Memory Aimbot"), Settings::MemoryAim);
				ImGui::SetCursorPos(ImVec2(230, 100));
				ImGui::CustomCheckbox(xorstr("Silent Aimbot"), Settings::SilentAim);
				ImGui::SetCursorPos(ImVec2(230, 130));
				ImGui::CustomCheckbox(xorstr("Magic Bullet (For a long distance only in Vehicle) (F6)"), Settings::MagicBullet);
				ImGui::SetCursorPos(ImVec2(230, 170));
				ImGui::SliderFloat(xorstr("Memory Aimbot Smooth"), &Settings::MemoryAimSmooth, 0.0f, 10.f, xorstr("%.1f"));

				ImGui::SetCursorPos(ImVec2(230, 200));
				ImGui::CustomCheckbox(xorstr("360 Fov Circle"), Settings::fov360);

				if (!Settings::fov360) {
					ImGui::SetCursorPos(ImVec2(230, 240));
					ImGui::SliderFloat(xorstr("Fov Circle"), &Settings::FovCircle_Value, 20.f, 800.f, xorstr("%.1f"));
				}
				
				
				ImGui::EndGroup();
			}

			if (tabs == 1) //Visuals
			{
				ImGui::BeginGroup();
				draw->AddText(ImVec2(pos.x + 230, pos.y + 34), ImColor(255, 255, 255), xorstr("Visuals"));

				ImGui::SetCursorPos(ImVec2(230, 70));
				ImGui::CustomCheckbox(xorstr("Box"), Settings::BoxESP);
				ImGui::SetCursorPos(ImVec2(360, 70));
				ImGui::CustomCheckbox(xorstr("BOT / AI"), Settings::BotAIESP);
				ImGui::SetCursorPos(ImVec2(230, 100));
				ImGui::CustomCheckbox(xorstr("Skeleton"), Settings::SkeletonESP);
				ImGui::SetCursorPos(ImVec2(230, 130));
				ImGui::CustomCheckbox(xorstr("Lines"), Settings::LinesESP);
				ImGui::SetCursorPos(ImVec2(230, 160));
				ImGui::CustomCheckbox(xorstr("Enemy Username"), Settings::PlayerNameESP);
				ImGui::SetCursorPos(ImVec2(230, 190));
				ImGui::CustomCheckbox(xorstr("Enemy Weapon"), Settings::EnemyWeaponESP);
				ImGui::EndGroup();
			}

			if (tabs == 2) //Exploits
			{
				ImGui::BeginGroup();
				draw->AddText(ImVec2(pos.x + 230, pos.y + 34), ImColor(255, 255, 255), xorstr("Exploits"));

				ImGui::SetCursorPos(ImVec2(230, 70));
				ImGui::CustomCheckbox(xorstr("No Spread"), Settings::NoSpread);

				ImGui::SetCursorPos(ImVec2(230, 100));
				if (ImGui::Button(xorstr("First Camera"), ImVec2(ImGui::CalcTextSize(xorstr("First Camera")).x + 10, 27))) Settings::FirstCamera = true;

				ImGui::SetCursorPos(ImVec2(230, 140));
				ImGui::CustomCheckbox(xorstr("Vehicle Fly"), Settings::CarFly);

				ImGui::SetCursorPos(ImVec2(230, 170));
				ImGui::CustomCheckbox(xorstr("Instant Revive"), Settings::InstantRevive);

				ImGui::SetCursorPos(ImVec2(230, 200));
				ImGui::CustomCheckbox(xorstr("Nothing For Now"), Settings::BulletTP);

				// SAMELINE() FROM THE START
				ImGui::SetCursorPos(ImVec2(330, 70));
				ImGui::CustomCheckbox(xorstr("Self Revive (F5)"), Settings::SelfRevive);
				/// /////////////

				ImGui::SetCursorPos(ImVec2(230, 230));
				ImGui::CustomCheckbox(xorstr("Fov Changer"), Settings::FovChanger);
				if (Settings::FovChanger) {
					ImGui::SetCursorPos(ImVec2(230, 270));
					ImGui::SliderFloat(xorstr("Fov Changer Value"), &Settings::FovChanger_Value, 30.f, 170.f, xorstr("%.1f"));
				}



				ImGui::EndGroup();
			}

			if (tabs == 3) //Teleport
			{

				ImGui::BeginGroup();
				draw->AddText(ImVec2(pos.x + 230, pos.y + 34), ImColor(255, 255, 255), xorstr("Teleport (Only In Vehicles)"));

				ImGui::SetCursorPos(ImVec2(230, 70));
				ImGui::CustomCheckbox(xorstr("Teleport To Waypoint (F6) (Only in Vehicles)"), Settings::CarTpToPing);

				ImGui::SetCursorPos(ImVec2(230, 100));
				ImGui::CustomCheckbox(xorstr("Teleport To Random Player (F7) (Only in Vehicles)"), Settings::CarTpToPlayer);

				ImGui::SetCursorPos(ImVec2(230, 135));
				ImGui::Combo(xorstr("##tptolocation"), &Settings::TP_Locations, TP_Locations_TypeItems, sizeof(TP_Locations_TypeItems) / sizeof(*TP_Locations_TypeItems));

				//ImGui::SetCursorPos(ImVec2(230, 170));
				ImGui::SameLine();
				if (ImGui::Button(xorstr("TP"))) {
					Settings::TP_Locations_Start = true;
				}

				draw->AddText(ImVec2(pos.x + 230, pos.y + 180), ImColor(255, 255, 255), xorstr("Teleport Without Vehicles"));

				ImGui::SetCursorPos(ImVec2(230, 220));
				ImGui::CustomCheckbox(xorstr("Teleport To Aiming Player (F8)"), Settings::TPToAimingPlayer);


				ImGui::EndGroup();
			}

			if (tabs == 4) //Misc
			{
				ImGui::BeginGroup();
				draw->AddText(ImVec2(pos.x + 230, pos.y + 34), ImColor(255, 255, 255), xorstr("Misc"));

				ImGui::SetCursorPos(ImVec2(230, 70));
				ImGui::CustomCheckbox(xorstr("Show Fov Circle"), Settings::ShowFovCircle);

				ImGui::SetCursorPos(ImVec2(230, 100));
				ImGui::CustomCheckbox(xorstr("Visible Check"), Settings::VisibleCheck);

				ImGui::SetCursorPos(ImVec2(230, 150));
				ImGui::SliderFloat(xorstr("Max ESP Distance"), &Settings::MaxESPDistance, 30.f, 300.f, xorstr("%.1f"));

				ImGui::EndGroup();
			}

			if (tabs == 5) //Colors
			{
			}

			if (tabs == 6) //Settings
			{
			}

			ImGui::PopFont();
			ImGui::End();
		}
	}


	

	return TRUE;
}

VOID CustomWndProc(HWND TargethWnd, ImGuiIO& io) {
	if (NtGetAsyncKeyState(VK_INSERT) & 1)
	{
		ShowMenu = !ShowMenu;
	}


	if (ShowMenu and SpoofThis(GetForegroundWindow) == TargethWnd) {


		POINT MousePos = {0, 0};
		SpoofThis(GetCursorPos, &MousePos);
		io.MousePos.x = MousePos.x;
		io.MousePos.y = MousePos.y;


		if (NtGetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

	}
}


HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{
	static HWND TargethWnd = 0;
	if (!device)
	{


		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);
		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView); renderTarget->Release();

		FontTitle = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(SF_Font, SF_Font_Size, 20.0f);
		FontTabs = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(SF_Font, SF_Font_Size, 18.0f);
		FontNormal = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(SF_Font, SF_Font_Size, 15.0f);
		FontDraw = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(SF_Font, SF_Font_Size, 15.0f);
		//ImGui::GetIO().Fonts->AddFontFromFileTTF(xorstr("C:\\Windows\\Fonts\\Tahoma.ttf"), 14.0f);
		ImGuiStyle* style = &ImGui::GetStyle();

		style->Alpha = 1.f;
		style->WindowRounding = 8.f;
		style->FramePadding = ImVec2(4, 3);
		style->WindowPadding = ImVec2(8, 8);
		style->ItemInnerSpacing = ImVec2(4, 4);
		style->ItemSpacing = ImVec2(8, 4);
		style->FrameRounding = 0.f;
		style->ScrollbarSize = 2.f;
		style->ScrollbarRounding = 12.f;
		style->GrabMinSize = 1.f;

		ImVec4* colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_ChildBg] = ImColor(24, 29, 59, 0);
		colors[ImGuiCol_Border] = ImVec4(0.07f, 0.07f, 0.11f, 0.50f);
		colors[ImGuiCol_FrameBg] = ImColor(222, 70, 0, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(222, 70, 0, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(222, 70, 0, 255);
		colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
		colors[ImGuiCol_SliderGrab] = ImColor(222, 70, 0, 255);
		colors[ImGuiCol_SliderGrabActive] = ImColor(222, 70, 0, 255);
		colors[ImGuiCol_Header] = ImColor(25, 25, 33, 255);
		colors[ImGuiCol_HeaderActive] = ImColor(28, 28, 36, 255);
		colors[ImGuiCol_HeaderHovered] = ImColor(30, 30, 38, 255);
		colors[ImGuiCol_PopupBg] = ImColor(35, 35, 45, 255);


		TargethWnd = SpoofThis(FindWindowA, (LPCSTR)xorstr("UnrealWindow"), (LPCSTR)xorstr("Fortnite  "));
		RECT rect;
		if (SpoofThis(GetWindowRect, TargethWnd, &rect))
		{
			Renderer_Defines::Width = rect.right - rect.left;
			Renderer_Defines::Height = rect.bottom - rect.top;
		}


		ImGui_ImplDX11_Init(TargethWnd, device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();

	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	ImGui_ImplDX11_NewFrame();

	CustomWndProc(TargethWnd, ImGui::GetIO());

	ImGui::PushFont(FontDraw);
	TestLoop();
	ImGui::PopFont();

	Menu();

	ImGui::Render();

	return SpoofThis(PresentOriginal, swapChain, syncInterval, flags);
}


bool InitializeHack()
{
	//AllocConsole();
	//freopen("CONIN$", "r", stdin);
	//freopen("CONOUT$", "w", stderr);
	//freopen("CONOUT$", "w", stdout);

	//Renderer_Defines::Width = SpoofThis(GetSystemMetrics, SM_CXSCREEN);
	//Renderer_Defines::Height = SpoofThis(GetSystemMetrics, SM_CYSCREEN);

	NtGetAsyncKeyState = (NtGetAsyncKeyState_)LI_FN(GetProcAddress)((HMODULE)customruntimes::GetBaseAddress(xorstr(L"win32u.dll")), xorstr("NtUserGetAsyncKeyState"));

	

	Addresses::UWorld = Memory::PatternScan(xorstr("48 8B 05 ? ? ? ? 4D 8B C2"));
	Addresses::UWorld = RVA(Addresses::UWorld, 7);

	Addresses::Free = Memory::PatternScan(xorstr("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));

	Addresses::GetNameByIndex = Memory::PatternScan(xorstr("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24"));
	
	Addresses::BoneMatrix = Memory::PatternScan(xorstr("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45"));
	Addresses::BoneMatrix = RVA(Addresses::BoneMatrix, 5);

	Addresses::ProjectWorldToScreen = Memory::PatternScan(xorstr("E8 ? ? ? ? 41 88 07 48 83 C4 30"));
	Addresses::ProjectWorldToScreen = RVA(Addresses::ProjectWorldToScreen, 5);

	Addresses::LineOfSightTo = Memory::PatternScan(xorstr("E8 ? ? ? ? 48 8B 0D ? ? ? ? 33 D2 40 8A F8"));
	Addresses::LineOfSightTo = RVA(Addresses::LineOfSightTo, 5);

	auto UObjectPtr = Memory::PatternScan(xorstr("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"));
	FN::objects = decltype(FN::objects)(RVA(UObjectPtr, 7));

	ObjectsAddresses::FOV = SpoofThis(FN::FindObject, (const char*)xorstr("FOV"));
	ObjectsAddresses::GetPlayerName = SpoofThis(FN::FindObject, (const char*)xorstr("GetPlayerName"));
	ObjectsAddresses::IsInVehicle = SpoofThis(FN::FindObject, (const char*)xorstr("IsInVehicle"));
	ObjectsAddresses::GetVehicle = SpoofThis(FN::FindObject2, (const char*)xorstr("FortPlayerPawn"), (const char*)xorstr("GetVehicle"));
	ObjectsAddresses::K2_SetActorLocation = SpoofThis(FN::FindObject, (const char*)xorstr("K2_SetActorLocation"));
	ObjectsAddresses::K2_TeleportTo = SpoofThis(FN::FindObject, (const char*)xorstr("K2_TeleportTo"));
	ObjectsAddresses::ServerReviveFromDBNO = SpoofThis(FN::FindObject, (const char*)xorstr("ServerReviveFromDBNO"));

	
	uintptr_t sceneAddr = Memory::PatternScanEx(HukHelper::DiscordModule, xorstr("56 57 53 48 83 EC 30 44 89 C6"));

	HukHelper::InsertHook(sceneAddr, (uintptr_t)PresentHook, (uintptr_t)&PresentOriginal);

	return true;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeHack();
	}
	return TRUE;
}