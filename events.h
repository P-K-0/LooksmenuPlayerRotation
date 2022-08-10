#pragma once

#include "f4se_libraries.h"
#include "std_libraries.h"

#include "Settings.h"
#include "f4se_plugin.h"

namespace events {

	using _ShowRaceMenu = void (*)(TESObjectREFR* akMenuTarget, SInt32 uiMode, TESObjectREFR* akMenuSpouseFemale, TESObjectREFR* akMenuSpouseMale, TESObjectREFR* akVendor);

	static RelocAddr<_ShowRaceMenu> ShowRaceMenu(0xB42C40);

	static RelocPtr<std::uint8_t> CollisiontState(0x058D08B0);

	static _ShowRaceMenu ShowRaceMenuOriginal{ nullptr };

	const double Pi = std::acos(-1);

	enum class UiMode {

		StartOfGame = 0,
		Remake,
		HairCut,
		PlasticSurgery,
		FacePaint

	};

	class Dispatcher :
		public BSTEventSink<MenuOpenCloseEvent>,
		public PlayerInputHandler {

	public:

		static Dispatcher& GetInstance() { return instance; }

		void Register();

		void SetNewGame(bool newGame) { isNewGame = newGame; }

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
		bool IsCameraEnabled();

		[[nodiscard]] const float GetCameraAngle();
		[[nodiscard]] const NiPoint3 GetCameraPos() const;

		void SetCollision(bool enabled) { *CollisiontState = enabled ? 0 : 1; }

		[[nodiscard]] Actor& GetCharacter() { return *infoLM.actor; }
		[[nodiscard]] const UiMode& GetUIMode() const { return infoLM.mode; }

		[[nodiscard]] const bool IsPlayer() const;

		void ReposCharacter();
		void UpdateCharacter();

		bool IsApplyCameraNodeAnimations();

		static void ShowRaceMenuHook(TESObjectREFR* akMenuTarget, SInt32 uiMode, TESObjectREFR* akMenuSpouseFemale, TESObjectREFR* akMenuSpouseMale, TESObjectREFR* akVendor);

		bool HookShowRaceMenu();

		static ObScript_Execute ShowLooksMenuOld;

		static Dispatcher instance;

		struct InfoLooksMenu {

			Actor* actor;
			UiMode mode;
		};

		static InfoLooksMenu infoLM;

		bool enableRotation{};
		bool enableLeftRight{};
		bool enableUpDown{};
		bool enableZoom{};

		bool joySwitch{};

		bool isLMOpened{};
		bool isNewGame{};

		bool reposChar{};

		POINT prevPos{};

		NiPoint3 rot{};
		NiPoint3 pos{};

		NiPoint3 cameraPos{};
		float cameraAngle{};

		void* g_moduleHandle{ nullptr };
	};
}
