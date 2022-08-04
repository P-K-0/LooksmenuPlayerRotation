
#include "events.h"

namespace events {

	Dispatcher::Dispatcher()
		: enableRotation{ false }, isLMOpened{ false }, prevPos{ 0 }
	{ 
		rot = GetPlayer().rot; 
	}

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

		_DMESSAGE("Events registered successfully!");

		isRegistered = true;
	}

	EventResult Dispatcher::ReceiveEvent(TESFurnitureEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->actor)
			return kEvent_Continue;

		if (isLMOpened && IsPlayer(evn->actor)) {

			GetPlayer().rot = rot;

			EnableCamera(true);
		}

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher)
	{ 
		if (!evn) 
			return kEvent_Continue;

		if (evn->menuName == "LooksMenu") {

			isLMOpened = evn->isOpen;

			if (isLMOpened) {

				rot = GetPlayer().rot;
			}
			else {

				GetPlayer().rot = rot;

				EnableCamera(true);
			}
		}

		return kEvent_Continue; 
	}

	void Dispatcher::OnCursorMoveEvent(CursorMoveEvent* inputEvent)
	{
		if (isLMOpened && enableRotation) {

			std::int32_t delta = inputEvent->unk28[2] - prevPos;

			prevPos = inputEvent->unk28[2];

			float fSensivity = Settings::Ini::GetInstance().GetSensivity();

			if (delta > 0) {

				GetPlayer().rot.z -= fSensivity;
			}
			else if (delta < 0) {

				GetPlayer().rot.z += fSensivity;
			}
		}
	}

	void Dispatcher::OnButtonEvent(ButtonEvent* inputEvent)
	{
		if (isLMOpened && inputEvent->deviceType == ButtonEvent::kDeviceType_Mouse && inputEvent->keyMask == Settings::Ini::GetInstance().GetMouseKey()) {

			enableRotation = inputEvent->isDown == 1.0f;

			if (enableRotation) 
				EnableCamera(false);
		}
	}

	void Dispatcher::EnableCamera(bool enabled)
	{
		auto camera = (*g_playerCamera);

		if (camera) {

			camera->SetEnabled(enabled);
			camera->Update();
		}
	}
}
