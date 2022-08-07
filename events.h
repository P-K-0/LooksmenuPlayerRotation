#pragma once

#include "f4se_libraries.h"
#include "std_libraries.h"

#include "Settings.h"
#include "f4se_plugin.h"

namespace events {

	static RelocPtr<std::uint8_t> CollisiontState(0x058D08B0);

	const double Pi = std::acos(-1);

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
		virtual void OnThumbstickEvent(ThumbstickEvent* inputEvent);

	private:

		Dispatcher(const Dispatcher&) = delete;
		Dispatcher(Dispatcher&&) = delete;

		Dispatcher& operator=(const Dispatcher&) = delete;
		Dispatcher& operator=(Dispatcher&&) = delete;

		Dispatcher() {}
		~Dispatcher() {}


		void EnableCamera(bool enabled);
		[[nodiscard]] const float GetCameraAngle() const;
		[[nodiscard]] const NiPoint3 GetCameraPos() const;

		void SetCollision(bool enabled) { *CollisiontState = enabled ? 0 : 1; }

		[[nodiscard]] Actor& GetCharacter() { return *lmActor; }
		[[nodiscard]] const bool IsPlayer(Actor* actor) const { return actor == (*g_player); }

		void ReposCharacter();
		void UpdateCharacter();

		void ParseCmdLine();

		static bool HookShowLooksMenu();
		static bool ShowLooksMenuNew(void* paramInfo, void* scriptData, TESObjectREFR* thisObj, void* containingObj, Script* scriptObj, void* locals, double* result, void* opcodeOffsetPtr);
		
		static ObScript_Execute ShowLooksMenuOld;

		static std::string cmdLine;

		Actor* lmActor{ nullptr };

		bool enableRotation{};
		bool enableLeftRight{};
		bool enableUpDown{};
		bool enableZoom{};

		bool joySwitch{};

		bool isLMOpened{};
		bool reposChar{};

		POINT prevPos{};

		NiPoint3 rot{};
		NiPoint3 pos{};
	};
}
