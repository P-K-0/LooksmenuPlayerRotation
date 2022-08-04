#pragma once

#include "SimpleIni.h"

namespace Settings {

	class Ini {

	public:

		static Ini& GetInstance() 
		{
			static Ini instance;
			return instance;
		}

		void ReadSettings();

		[[nodiscard]] const float GetSensivity() const { return fSensivity; }
		[[nodiscard]] const std::uint32_t GetMouseKey() const { return iMouseKey; }

	private:

		Ini() {}
		~Ini()	{}

		Ini(const Ini&) = delete;
		Ini(Ini&&) = delete;

		Ini& operator=(const Ini&) = delete;
		Ini& operator=(Ini&&) = delete;

		float fSensivity{ 0.1f };
		std::uint32_t iMouseKey{ 1 };
	};
}