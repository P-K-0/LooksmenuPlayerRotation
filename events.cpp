
#include "events.h"

namespace events {

	void Dispatcher::Register()
	{
		static bool isRegistered{};
	
		if (isRegistered) return;

		static Dispatcher instance;
		
		auto playerCtrl = (*g_playerControls);

		if (playerCtrl) {

			tArray<PlayerInputHandler*>* inputEvents = &(playerCtrl->inputEvents1);
			PlayerInputHandler* inputHandler = &instance;

			if (inputEvents && inputEvents->GetItemIndex(inputHandler) == -1) 
				inputEvents->Push(inputHandler);
		}

		(*g_ui)->menuOpenCloseEventSource.AddEventSink(&instance);

		GetEventDispatcher<TESFurnitureEvent>()->AddEventSink(&instance);

		HookShowLooksMenu();

		_DMESSAGE("Events registered successfully!");

		isRegistered = true;
	}

	EventResult Dispatcher::ReceiveEvent(TESFurnitureEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->actor)
			return kEvent_Continue;

		//if (isLMOpened && evn->actor == lmActor) {

		//	GetCharacter().rot = rot;
		//	GetCharacter().pos = pos;

		//	EnableCamera(true);
		//}

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher)
	{ 
		if (!evn) 
			return kEvent_Continue;

		if (evn->menuName == "LooksMenu") {

			isLMOpened = evn->isOpen;

			if (isLMOpened) {

				ParseCmdLine();

				rot = GetCharacter().rot;
				pos = GetCharacter().pos;

				reposChar = Settings::Ini::GetInstance().GetReposCharacter();

				EnableCamera(false);
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

	void Dispatcher::OnCursorMoveEvent(CursorMoveEvent* inputEvent)
	{
		if (!inputEvent || !isLMOpened)
			return;

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

			auto cameraAngle = GetCameraAngle() + Pi / 2;

			if (deltaX > 2) {

				GetCharacter().pos.x += std::sin(cameraAngle);
				GetCharacter().pos.y += std::cos(cameraAngle);
			}
			else if (deltaX < -2) {

				GetCharacter().pos.x -= std::sin(cameraAngle);
				GetCharacter().pos.y -= std::cos(cameraAngle);
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
		if (!inputEvent || !isLMOpened)
			return;

		ReposCharacter();

		float fSensivity = Settings::Ini::GetInstance().GetSensivity();
		auto cameraAngle = GetCameraAngle();

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

			cameraAngle += Pi / 2;

			switch (posThumbStick) {

			case 2:

				GetCharacter().pos.x += std::sin(cameraAngle);
				GetCharacter().pos.y += std::cos(cameraAngle);
				break;

			case 4:

				GetCharacter().pos.x -= std::sin(cameraAngle);
				GetCharacter().pos.y -= std::cos(cameraAngle);
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
		if (!inputEvent || !isLMOpened)
			return;

		ReposCharacter();
		
		bool isDown = inputEvent->isDown == 1.0f;
		std::uint32_t keyCode{ inputEvent->keyMask };

		if (inputEvent->deviceType == ButtonEvent::kDeviceType_Gamepad) {

			if (keyCode == 64)
				joySwitch = isDown;
		}

		if (inputEvent->deviceType == ButtonEvent::kDeviceType_Mouse ||
			inputEvent->deviceType == ButtonEvent::kDeviceType_Keyboard) {

			auto cameraAngle = GetCameraAngle();

			switch (keyCode) {

			case 2048: // mouse wheel up

				if (enableZoom) {

					GetCharacter().pos.x += std::sin(cameraAngle);
					GetCharacter().pos.y += std::cos(cameraAngle);

					UpdateCharacter();
				}

				break;

			case 2304: // mouse wheel down

				if (enableZoom) {

					GetCharacter().pos.x -= std::sin(cameraAngle);
					GetCharacter().pos.y -= std::cos(cameraAngle);

					UpdateCharacter();
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
	}

	void Dispatcher::ReposCharacter()
	{
		if (reposChar) {

			auto cameraPos = GetCameraPos();
			auto cameraAngle = GetCameraAngle();

			GetCharacter().rot.z = cameraAngle + Pi;
			GetCharacter().pos.x = cameraPos.x + 100.0f * std::sin(cameraAngle);
			GetCharacter().pos.y = cameraPos.y + 100.0f * std::cos(cameraAngle);

			reposChar = false;
		}
	}

	void Dispatcher::UpdateCharacter()
	{
		if (lmActor && lmActor->middleProcess)
			CALL_MEMBER_FN(lmActor->middleProcess, UpdateEquipment)(lmActor, 0x11);
	}

	void Dispatcher::EnableCamera(bool enabled)
	{
		auto camera = (*g_playerCamera);

		if (camera && Settings::Ini::GetInstance().GetLockCamera()) {

			camera->SetEnabled(enabled);
			camera->Update();
		}
	}

	const float Dispatcher::GetCameraAngle() const
	{
		auto camera = (*g_playerCamera);
		return camera ? camera->unk194 : 0.0f;
	}

	const NiPoint3 Dispatcher::GetCameraPos() const 
	{
		auto camera = (*g_playerCamera);
		return camera && camera->cameraNode ? camera->cameraNode->m_localTransform.pos : NiPoint3();
	}

	void Dispatcher::ParseCmdLine()
	{
		TESForm* frm{ nullptr };

		lmActor = nullptr;

		if (!cmdLine.empty()) {

			std::stringstream ss{ cmdLine };

			std::string cmd;
			std::uint32_t id{ 0 };

			ss >> cmd >> std::hex >> id;

			if (id != 0)
				frm = LookupFormByID(id);

			if (frm)
				lmActor = DYNAMIC_CAST(frm, TESForm, Actor);
		}

		if (!lmActor)
			lmActor = (*g_player);

		_DMESSAGE("Selected Character ID: %.8X", lmActor->formID);

		cmdLine.clear();
	}

	bool Dispatcher::ShowLooksMenuNew(void* paramInfo, void* scriptData, TESObjectREFR* thisObj, void* containingObj, Script* scriptObj, void* locals, double* result, void* opcodeOffsetPtr)
	{
		cmdLine.clear();

		if (scriptObj)
			cmdLine = scriptObj->text;

		return ShowLooksMenuOld(paramInfo, scriptData, thisObj, containingObj, scriptObj, locals, result, opcodeOffsetPtr);
	}

	bool Dispatcher::HookShowLooksMenu()
	{
		for (ObScriptCommand* iter = g_firstObScriptCommand; iter->opcode < (kObScript_NumObScriptCommands + kObScript_ScriptOpBase); ++iter)
			if (_strcmpi(iter->longName, "ShowLooksMenu") == 0) {

				_DMESSAGE("ShowLooksMenu found at address : %016I64X", GetFnAddr(iter->execute));

				ShowLooksMenuOld = iter->execute;
				iter->execute = ShowLooksMenuNew;

				return true;
			}

		return false;
	}

	ObScript_Execute Dispatcher::ShowLooksMenuOld{ nullptr };

	std::string Dispatcher::cmdLine;
}
