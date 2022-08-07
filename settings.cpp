
#include "settings.h"

namespace Settings {

	void Ini::ReadSettings()
	{
		CSimpleIni ini;

		ini.SetUnicode();
		SI_Error error = ini.LoadFile(R"(Data\F4SE\Plugins\LooksmenuPlayerRotation.ini)");

		if (error == 0) {

			fSensivity = std::stof(ini.GetValue("Main", "fSensivity", "0.1"));
			bLockCamera = std::stoi(ini.GetValue("Main", "bLockCamera", "0"));
			bReposCharacter = std::stoi(ini.GetValue("Main", "bReposCharacter", "1"));

			iKeyRotate = std::stoi(ini.GetValue("Keys", "iKeyRotate", "1"));
			iKeyLeftRight = std::stoi(ini.GetValue("Keys", "iKeyLeftRight", "0"));
			iKeyUpDown = std::stoi(ini.GetValue("Keys", "iKeyUpDown", "2"));
			iKeyZoom = std::stoi(ini.GetValue("Keys", "iKeyZoom", "160"));
		}
	}

	Ini Ini::instance;
}
