
#include "events.h"

namespace events {

	void Dispatcher::Register()
	{
		static bool isRegistered{};
	
		if (isRegistered) return;
		
		auto playerCtrl = (*g_playerControls);

		if (playerCtrl) {

			tArray<PlayerInputHandler*>* inputEvents = &(playerCtrl->inputEvents1);
			PlayerInputHandler* inputHandler = &instance;

			if (inputEvents && inputEvents->GetItemIndex(inputHandler) == -1) 
				inputEvents->Push(inputHandler);
		}

		(*g_ui)->menuOpenCloseEventSource.AddEventSink(&instance);

		HookShowRaceMenu();

		_DMESSAGE("Events registered successfully!");

		isRegistered = true;
	}

	void Dispatcher::OnCursorMoveEvent(CursorMoveEvent* inputEvent)
	{
		if (!inputEvent || isNewGame || !isLMOpened)
			return;

		if (IsCameraEnabled())
			EnableCamera(false);

		ReposCharacter();

		std::int32_t deltaX = inputEvent->unk28[2] - prevPos.x;
		std::int32_t deltaY = inputEvent->unk28[3] - prevPos.y;

		prevPos.x = inputEvent->unk28[2];
		prevPos.y = inputEvent->unk28[3];

		if (enableRotation) {

			float fSensivity = Settings::Ini::GetInstance().GetSensivity();

			if (deltaX > 0) {

				GetCharacter().rot.z -= fSensivity;
			}
			else if (deltaX < 0) {

				GetCharacter().rot.z += fSensivity;
			}
		}

		if (enableLeftRight) {

			auto angle = cameraAngle + Pi / 2;

			if (deltaX > 2) {

				GetCharacter().pos.x += std::sin(angle);
				GetCharacter().pos.y += std::cos(angle);
			}
			else if (deltaX < -2) {

				GetCharacter().pos.x -= std::sin(angle);
				GetCharacter().pos.y -= std::cos(angle);
			}
		}

		if (enableUpDown) {

			if (deltaY > 2) {

				GetCharacter().pos.z -= 1.5;
			}
			else if (deltaY < -2) {

				GetCharacter().pos.z += 1.5;
			}
		}

		UpdateCharacter();
	}

	void Dispatcher::OnThumbstickEvent(ThumbstickEvent* inputEvent)
	{
		if (!inputEvent || isNewGame || !isLMOpened)
			return;

		if (IsCameraEnabled())
			EnableCamera(false);

		ReposCharacter();

		float fSensivity = Settings::Ini::GetInstance().GetSensivity();

		std::uint32_t posThumbStick = inputEvent->unk20[5];

		if (inputEvent->unk20[0] != 12)
			return;

		if (joySwitch) {

			switch (posThumbStick) {

			case 2:

				GetCharacter().rot.z -= fSensivity;
				break;

			case 4:

				GetCharacter().rot.z += fSensivity;
				break;

			case 1:

				GetCharacter().pos.x += std::sin(cameraAngle);
				GetCharacter().pos.y += std::cos(cameraAngle);
				break;

			case 3:

				GetCharacter().pos.x -= std::sin(cameraAngle);
				GetCharacter().pos.y -= std::cos(cameraAngle);
				break;

			}
		}
		else {

			auto angle = cameraAngle + Pi / 2;

			switch (posThumbStick) {

			case 2:

				GetCharacter().pos.x += std::sin(angle);
				GetCharacter().pos.y += std::cos(angle);
				break;

			case 4:

				GetCharacter().pos.x -= std::sin(angle);
				GetCharacter().pos.y -= std::cos(angle);
				break;

			case 1:

				GetCharacter().pos.z += 1.0;
				break;

			case 3:

				GetCharacter().pos.z -= 1.0;
				break;

			}
		}

		UpdateCharacter();
	}

	void Dispatcher::OnButtonEvent(ButtonEvent* inputEvent)
	{
		if (!inputEvent || isNewGame || !isLMOpened)
			return;

		if (IsCameraEnabled())
			EnableCamera(false);

		ReposCharacter();
		
		bool isDown = inputEvent->isDown == 1.0f;
		std::uint32_t keyCode{ inputEvent->keyMask };

		if (inputEvent->deviceType == ButtonEvent::kDeviceType_Gamepad) {

			if (keyCode == 64)
				joySwitch = isDown;
		}

		if (inputEvent->deviceType == ButtonEvent::kDeviceType_Mouse ||
			inputEvent->deviceType == ButtonEvent::kDeviceType_Keyboard) {

			switch (keyCode) {

			case 2048: // mouse wheel up

				if (enableZoom) {

					GetCharacter().pos.x += std::sin(cameraAngle);
					GetCharacter().pos.y += std::cos(cameraAngle);
				}

				break;

			case 2304: // mouse wheel down

				if (enableZoom) {

					GetCharacter().pos.x -= std::sin(cameraAngle);
					GetCharacter().pos.y -= std::cos(cameraAngle);
				}

				break;

			default:
				
				auto& settings = Settings::Ini::GetInstance();

				if (keyCode == settings.GetKeyRotate())
					enableRotation = isDown;

				if (keyCode == settings.GetKeyLeftRight())
					enableLeftRight = isDown;

				if (keyCode == settings.GetKeyUpDown())
					enableUpDown = isDown;

				if (keyCode == settings.GetKeyZoom())
					enableZoom = isDown;
				
				break;
			}
		}

		UpdateCharacter();
	}

	const bool Dispatcher::IsPlayer() const
	{
		if (infoLM.actor && (*g_player))
			return infoLM.actor == (*g_player);

		return false;
	}

	void Dispatcher::ReposCharacter()
	{
		if (!reposChar)
			return;

		auto& settings = Settings::Ini::GetInstance();

		if (!settings.GetReposCharacter())
			return;

		if (GetUIMode() >= UiMode::HairCut && settings.GetNoSurgeryRepos())
			return;

		GetCharacter().rot.z = cameraAngle + Pi;
		GetCharacter().pos.x = cameraPos.x + 100.0f * std::sin(cameraAngle);
		GetCharacter().pos.y = cameraPos.y + 100.0f * std::cos(cameraAngle);

		UpdateCharacter();

		reposChar = false;
	}

	void Dispatcher::UpdateCharacter()
	{
		auto actor = infoLM.actor;

		if (actor && actor->middleProcess)
			CALL_MEMBER_FN(actor->middleProcess, UpdateEquipment)(actor, 0x11);
	}

	bool Dispatcher::IsApplyCameraNodeAnimations()
	{
		Setting* stg = GetINISetting("bApplyCameraNodeAnimations:Camera");

		double isApply{ false };

		if (stg && stg->GetDouble(&isApply) && isApply && Settings::Ini::GetInstance().GetCheckApplyCameraNodeAnimations())
			return true;

		return false;
	}

	void Dispatcher::EnableCamera(bool enabled)
	{
		auto camera = (*g_playerCamera);

		if (camera && Settings::Ini::GetInstance().GetLockCamera()) {

			camera->SetEnabled(enabled);
			camera->Update();
		}
	}

	bool Dispatcher::IsCameraEnabled()
	{
		auto camera = (*g_playerCamera);

		if (camera)
			return camera->unk30 != 0;

		return false;
	}

	const float Dispatcher::GetCameraAngle()
	{
		auto camera = (*g_playerCamera);

		if (GetUIMode() >= UiMode::HairCut && IsPlayer())
			return rot.z + (IsApplyCameraNodeAnimations() ? Pi : 0);

		return camera ? camera->unk194 : 0.0f;
	}

	const NiPoint3 Dispatcher::GetCameraPos() const 
	{
		auto camera = (*g_playerCamera);

		return camera && camera->cameraNode ? camera->cameraNode->m_localTransform.pos : NiPoint3();
	}

	EventResult Dispatcher::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher)
	{
		if (!evn)
			return kEvent_Continue;

		if (evn->menuName == "LooksMenu") {

			auto& settings = Settings::Ini::GetInstance();

			isLMOpened = evn->isOpen;

			if (isNewGame && settings.GetDisabledInNewGame()) {

				if (!isLMOpened)
					isNewGame = false;

				return kEvent_Continue;
			}

			if (isLMOpened) {

				reposChar = true;

				rot = GetCharacter().rot;
				pos = GetCharacter().pos;

				cameraPos = GetCameraPos();
				cameraAngle = GetCameraAngle();

				SetCollision(false);
			}
			else {

				GetCharacter().rot = rot;
				GetCharacter().pos = pos;

				EnableCamera(true);
				SetCollision(true);

				UpdateCharacter();
			}
		}

		return kEvent_Continue;
	}

	void Dispatcher::ShowRaceMenuHook(TESObjectREFR* akMenuTarget, SInt32 uiMode, TESObjectREFR* akMenuSpouseFemale, TESObjectREFR* akMenuSpouseMale, TESObjectREFR* akVendor)
	{
		infoLM.actor = !akMenuTarget ? (*g_player) : DYNAMIC_CAST(akMenuTarget, TESObjectREFR, Actor);

		if (!infoLM.actor)
			infoLM.actor = (*g_player);

		infoLM.mode = static_cast<UiMode>(uiMode);

		_DMESSAGE("Selected Character ID: %.8X", infoLM.actor->formID);

		ShowRaceMenuOriginal(akMenuTarget, uiMode, akMenuSpouseFemale, akMenuSpouseMale, akVendor);
	}

	bool Dispatcher::HookShowRaceMenu()
	{
		if (!g_branchTrampoline.Create(1024 * 64))
			return false;

		if (!g_localTrampoline.Create(1024 * 64, g_moduleHandle))
			return false;

		struct ShowRaceMenu_Code : Xbyak::CodeGenerator {
			
			ShowRaceMenu_Code(void* buf)
				: Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp + 0x18], rbx);
				push(rdi);
	
				jmp(ptr[rip + retnLabel]);

				L(retnLabel);

				dq(ShowRaceMenu.GetUIntPtr() + 6);
			}
		};

		void* codebuf = g_localTrampoline.StartAlloc();
		ShowRaceMenu_Code code(codebuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		ShowRaceMenuOriginal = (_ShowRaceMenu)codebuf;

		g_branchTrampoline.Write6Branch(ShowRaceMenu.GetUIntPtr(), (std::uintptr_t)ShowRaceMenuHook);

		return true;
	}

	ObScript_Execute Dispatcher::ShowLooksMenuOld{ nullptr };

	Dispatcher Dispatcher::instance;

	Dispatcher::InfoLooksMenu Dispatcher::infoLM;
}
