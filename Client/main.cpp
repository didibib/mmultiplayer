#include <windows.h>

#include "debug.h"
#include "menu.h"
#include "settings.h"
#include "addon.h"

#include "addons/dolly.h"
#include "addons/client.h"
#include "addons/trainer.h"
#include "addons/misc.h"
#include "addons/logging.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
		Debug::Initialize();

		while (!GetModuleHandle(L"d3d9.dll")) {
			Sleep(100);
		}

		Settings::Load();

		//Addon *addons[] = {new Client(), new Trainer(), new Dolly(), new Misc(), new Log()};
		Addon *addons[] = {new Trainer(), new Logging()};

		if (!Engine::Initialize()) {
			MessageBoxA(0, "Failed to initialize engine", "Fatal", 0);
			return TRUE;
		}

		if (!Menu::Initialize()) {
			MessageBoxA(0, "Failed to initialize menu", "Fatal", 0);
			return TRUE;
		}
		
		for (auto &addon : addons) {
			if (!addon->Initialize()) {
				MessageBoxA(0, ("Failed to initialize \"" + addon->GetName() + "\"").c_str(), "Fatal", 0);
			}
		}
	}
	
	return TRUE;
}