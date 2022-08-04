
#include "settings.h"

namespace Settings {

	void Ini::ReadSettings()
	{
		CSimpleIni ini;

		ini.SetUnicode();
		SI_Error error = ini.LoadFile(R"(Data\F4SE\Plugins\LooksmenuPlayerRotation.ini)");

		if (error == 0) {

			fSensivity = std::stof(ini.GetValue("Main", "fSensivity", "0.1"));
			iMouseKey = std::stoi(ini.GetValue("Main", "iMouseKey", "1"));
		}
	}
}
