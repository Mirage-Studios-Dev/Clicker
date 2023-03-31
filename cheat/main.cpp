#include "gui.h"
#include "main.h"
#include "keybind.h"
#include<Windows.h>
#include<stdlib.h>
#include<iostream>
#include <thread>


void clicker() {
	while (true) {
		if (enabled) {
				int left_delay = (2000 / cps) / 1;
				if (GetAsyncKeyState(VK_LBUTTON)) {
					HWND Window = GetForegroundWindow(); // Get Window
					if (FindWindowA(("LWJGL"), nullptr) == GetForegroundWindow()) {
						SendMessageW(GetForegroundWindow(), WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
						SendMessageW(GetForegroundWindow(), WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
						Sleep(rand() % left_delay);
					}
				}
		}

		if (GetAsyncKeyState(toggleKey) & 0x8000 && toggleKey != 0 && (GetForegroundWindow() != FindWindowA(NULL, "PvPLab Clicker")))
		{
			enabled = !enabled;
			Sleep(400);
		}

		Sleep(1);
	}
}

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow)
{
	gui::CreateHWindow("PvPLab Clicker");
	gui::CreateDevice();
	gui::CreateImGui();

	std::thread t(clicker);

	while (gui::isRunning)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	// destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}
