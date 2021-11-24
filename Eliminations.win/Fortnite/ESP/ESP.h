#pragma once

namespace ESP {
	static BOOL Initialize(ImDrawList* FNDraw, AFortPawn* CurrentPawn, ImVec2 Head, ImVec2 Pelvis, bool IsVisible) {

		if (Addresses::LocalPawn and Memory::IsValidPointer(uintptr_t(Addresses::LocalPawn))) {
			//float WorldDistance = Addresses::LocalPawn->RootComponent()->RelativeLocation().Distance(CurrentPawn->GetBone(Bone_Head));
			//if (WorldDistance / 100.f > Settings::MaxESPDistance) return false;
		}


		ImColor Color, TextColor;

		if (IsVisible)
			Color = ImColor(255, 255, 0);
		else
			Color = ImColor(255, 0, 0);

		if (IsVisible)
			TextColor = ImColor(255, 255, 255);
		else
			TextColor = ImColor(255, 0, 0);

		ImVec2 Neck = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_Neck));
		ImVec2 ChestAtLeft = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_ChestAtLeft));
		ImVec2 ChestAtRight = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_ChestAtRight));
		ImVec2 Chest = ImVec2(ChestAtLeft.x + (ChestAtRight.x - ChestAtLeft.x) / 2, ChestAtLeft.y);
		ImVec2 LeftShoulder = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftShoulder));
		ImVec2 RightShoulder = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightShoulder));
		ImVec2 LeftElbow = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftElbow));
		ImVec2 RightElbow = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightElbow));
		ImVec2 LeftHand = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftHand));
		ImVec2 RightHand = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightHand));
		ImVec2 LeftLeg = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftLeg));
		ImVec2 RightLeg = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightLeg));
		ImVec2 LeftThigh = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftThigh));
		ImVec2 RightThigh = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightThigh));
		ImVec2 LeftFoot = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftFoot));
		ImVec2 RightFoot = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightFoot));
		ImVec2 LeftFeet = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftFeet));
		ImVec2 RightFeet = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightFeet));
		ImVec2 LeftFeetFinger = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_LeftFeetFinger));
		ImVec2 RightFeetFinger = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(Bone_RightFeetFinger));
		ImVec2 Bottom = Addresses::PlayerController->WorldToScreen(CurrentPawn->GetBone(0));

		if (Settings::BoxESP) {

			int array[20] = { Head.x, Neck.x, Pelvis.x, Chest.x, LeftShoulder.x, RightShoulder.x, LeftElbow.x, RightElbow.x, LeftHand.x, RightHand.x, LeftLeg.x, RightLeg.x, LeftThigh.x, RightThigh.x, LeftFoot.x, RightFoot.x, LeftFeet.x, RightFeet.x, LeftFeetFinger.x, RightFeetFinger.x };
			int MostRightBone = array[0];
			int MostLeftBone = array[0];

			for (int mostrighti = 0; mostrighti < 20; mostrighti++)
			{
				if (array[mostrighti] > MostRightBone)
					MostRightBone = array[mostrighti];
			}

			for (int mostlefti = 0; mostlefti < 20; mostlefti++)
			{
				if (array[mostlefti] < MostLeftBone)
					MostLeftBone = array[mostlefti];
			}


			float ActorHeight = (Head.y - Bottom.y);
			if (ActorHeight < 0) ActorHeight = ActorHeight * (-1.f);
			int ActorWidth = MostRightBone - MostLeftBone;

			float X = Head.x - (ActorWidth / 2);
			float Y = Head.y;
			float W = ActorWidth;
			float H = ActorHeight;


			FNDraw->AddLine(ImVec2(X, Y), ImVec2(X, Y + H / 3), Color, 2.f);
			FNDraw->AddLine(ImVec2(X, Y), ImVec2(X + W / 4, Y), Color, 2.f);
			FNDraw->AddLine(ImVec2(X + W - W / 4, Y), ImVec2(X + W, Y), Color, 2.f);
			FNDraw->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + H / 3), Color, 2.f);
			FNDraw->AddLine(ImVec2(X, Y + H - H / 3), ImVec2(X, Y + H), Color, 2.f);
			FNDraw->AddLine(ImVec2(X, Y + H), ImVec2(X + W / 4, Y + H), Color, 2.f);
			FNDraw->AddLine(ImVec2(X + W - W / 4, Y + H), ImVec2(X + W, Y + H), Color, 2.f);
			FNDraw->AddLine(ImVec2(X + W, Y + H - H / 3), ImVec2(X + W, Y + H), Color, 2.f);
		}

		if (Settings::SkeletonESP) {

			FNDraw->AddLine(Head, Neck, Color, 2.f);
			FNDraw->AddLine(Neck, Chest, Color, 2.f);
			FNDraw->AddLine(Chest, Pelvis, Color, 2.f);
			FNDraw->AddLine(Chest, LeftShoulder, Color, 2.f);
			FNDraw->AddLine(Chest, RightShoulder, Color, 2.f);
			FNDraw->AddLine(LeftShoulder, LeftElbow, Color, 2.f);
			FNDraw->AddLine(RightShoulder, RightElbow, Color, 2.f);
			FNDraw->AddLine(LeftElbow, LeftHand, Color, 2.f);
			FNDraw->AddLine(RightElbow, RightHand, Color, 2.f);
			FNDraw->AddLine(Pelvis, LeftLeg, Color, 2.f);
			FNDraw->AddLine(Pelvis, RightLeg, Color, 2.f);
			FNDraw->AddLine(LeftLeg, LeftThigh, Color, 2.f);
			FNDraw->AddLine(RightLeg, RightThigh, Color, 2.f);
			FNDraw->AddLine(LeftThigh, LeftFoot, Color, 2.f);
			FNDraw->AddLine(RightThigh, RightFoot, Color, 2.f);
			FNDraw->AddLine(LeftFoot, LeftFeet, Color, 2.f);
			FNDraw->AddLine(RightFoot, RightFeet, Color, 2.f);
			FNDraw->AddLine(LeftFeet, LeftFeetFinger, Color, 2.f);
			FNDraw->AddLine(RightFeet, RightFeetFinger, Color, 2.f);

		}

		if (Settings::LinesESP) {
			FNDraw->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height), Pelvis, Color, 1.2f);
		}

		if (Settings::EnemyWeaponESP) {
			FText* EnemyWeaponName = CurrentPawn->CurrentWeapon()->WeaponData()->DisplayName();
			if (EnemyWeaponName) {
				ImGui::DrawOutlinedText(EnemyWeaponName->c_str(), ImVec2(Bottom.x, Bottom.y + ImGui::CalcTextSize(EnemyWeaponName->c_str()).y / 2), TextColor);
			}
		}

		if (Settings::PlayerNameESP) {

			std::string EnemyName = CurrentPawn->PlayerState()->GetPlayerName();

			ImGui::DrawOutlinedText(EnemyName, ImVec2(Head.x, Head.y - ImGui::CalcTextSize(EnemyName.c_str()).y * 2), TextColor);
		}

		return TRUE;
	}
}