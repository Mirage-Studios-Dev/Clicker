#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "main.h"
#include "keybind.h"
#include <stdlib.h>
#include <format>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		"class001",
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::CreateContext();

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.46f, 0.54f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.15f, 0.17f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.15f, 0.17f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.14f, 0.15f, 0.17f, 0.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.23f, 0.25f, 0.29f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.46f, 0.54f, 0.50f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.15f, 0.17f, 0.70f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.15f, 0.17f, 0.47f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.46f, 0.54f, 0.50f, 0.31f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.78f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.46f, 0.54f, 0.50f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 0.80f);
	colors[ImGuiCol_SliderGrab] = ImVec4(3.0f / 255.0f, 105.0f / 255.0f, 160.0f / 255.0f, 1.0f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.50f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.23f, 0.25f, 0.29f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.40f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.54f, 0.50f, 0.67f);
	colors[ImGuiCol_Header] = ImVec4(0.46f, 0.54f, 0.50f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.78f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.46f, 0.54f, 0.50f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.46f, 0.54f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.54f, 0.50f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.86f, 0.93f, 0.89f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.54f, 0.50f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.15f, 0.17f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.15f, 0.17f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.46f, 0.54f, 0.50f, 0.78f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.46f, 0.54f, 0.50f, 0.50f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.46f, 0.54f, 0.50f, 0.95f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.46f, 0.54f, 0.50f, 0.80f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.46f, 0.54f, 0.50f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	style.ChildRounding = 4.0f;
	style.PopupRounding = 4.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ItemSpacing = ImVec2(15, 15);

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 18.5f);
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 18.5f);
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void gui::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });


	ImGui::Begin(
		"PvPLab Clicker",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar
	);
	{
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		float posx = (WIDTH - ImGui::CalcTextSize("PVPLAB CLICKER").x) / 2.0f;
		ImGui::SetCursorPosX(posx);
		ImGui::Text("PVPLAB CLICKER");
		ImGui::PopFont();

		float posx2 = (WIDTH - 200) / 2.0f;
		ImGui::SetCursorPosX(posx2);
		ImGui::SliderInt("CPS", &cps, 1, 10, "%d", ImGuiSliderFlags_ClampOnInput);

		ImGui::SetCursorPosX(posx2);
		
		if (ImGui::Button("Disclaimer")) {
			const char* url = "https://github.com/PvPLab";
			ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
		}

		ImGui::SameLine();

		std::string bindString = "[" + get_key_name_by_id(toggleKey) + "]";
		if (ImGui::Button(bindString.c_str())) {
			keybind(toggleKey);
		}
	}
}