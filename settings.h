#pragma once

#include "SimpleIni.h"

namespace Settings {

	class Ini {

	public:

		static Ini& GetInstance() { return instance; }

		void ReadSettings();

		[[nodiscard]] const float GetSensivity() const { return fSensivity; }
		[[nodiscard]] const bool GetLockCamera() const { return bLockCamera; }
		[[nodiscard]] const bool GetReposCharacter() const { return bReposCharacter; }

		[[nodiscard]] const std::uint32_t GetKeyRotate() const { return iKeyRotate; }
		[[nodiscard]] const std::uint32_t GetKeyLeftRight() const { return iKeyLeftRight; }
		[[nodiscard]] const std::uint32_t GetKeyUpDown() const { return iKeyUpDown; }
		[[nodiscard]] const std::uint32_t GetKeyZoom() const { return iKeyZoom; }

	private:

		Ini() {}
		~Ini(){}

		Ini(const Ini&) = delete;
		Ini(Ini&&) = delete;

		Ini& operator=(const Ini&) = delete;
		Ini& operator=(Ini&&) = delete;

		float fSensivity{ 0.1f };

		bool bLockCamera{ false };
		bool bReposCharacter{ true };

		std::uint32_t iKeyRotate{ 1 };
		std::uint32_t iKeyLeftRight{ 0 };
		std::uint32_t iKeyUpDown{ 2 };
		std::uint32_t iKeyZoom{ 160 };

		static Ini instance;
	};
}
