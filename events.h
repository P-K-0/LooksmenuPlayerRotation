#pragma once

#include "f4se_libraries.h"
#include "std_libraries.h"

#include "Settings.h"

namespace events {

	class Dispatcher :
		public BSTEventSink<TESFurnitureEvent>,
		public BSTEventSink<MenuOpenCloseEvent>,
		public PlayerInputHandler {

	public:

		static void Register();

		EventResult ReceiveEvent(TESFurnitureEvent* evn, void* dispatcher);
		EventResult ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher);

		virtual void OnCursorMoveEvent(CursorMoveEvent* inputEvent);
		virtual void OnButtonEvent(ButtonEvent* inputEvent);

	private:

		Dispatcher(const Dispatcher&) = delete;
		Dispatcher(Dispatcher&&) = delete;

		Dispatcher& operator=(const Dispatcher&) = delete;
		Dispatcher& operator=(Dispatcher&&) = delete;

		Dispatcher();
		~Dispatcher() {}

		bool enableRotation{};
		bool isLMOpened{};
		std::uint32_t prevPos{};
		NiPoint3 rot{};

		void EnableCamera(bool enabled);

		inline [[nodiscard]] PlayerCharacter& GetPlayer() { return *(*g_player); }
		inline [[nodiscard]] const bool IsPlayer(Actor* actor) const { return actor == (*g_player); }
	};
}
