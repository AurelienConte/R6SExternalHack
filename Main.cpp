#pragma once 
#include "Memory.h"
#include "Overlay.h"

using namespace std;

Memory Mem;


BOOL Is_PlayerValid(PlayerInfo Player, PlayerInfo LocalPlayer)
{
	if (!Player.Name.empty() && Player.Health > 0 && Player.Health < 200 && Player.w2sHead.z >= 1.0f && Player.Name != LocalPlayer.Name && Player.TeamId != LocalPlayer.TeamId)
		return true;
	return false;
}

void main()
{
	Mem.SetBaseAddress();

	printf("Hijacking notepadd window...\n");
	HWND hwnd = HiJackNotepadWindow();
	if (!hwnd) {
		cout << "Window HiJacking failed (use debugger to investigate why)" << endl;
		exit(84);
	}
	HDC hdc = GetDC(hwnd);

	// Getting settings of back buffer bitmap
	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
	BITMAPINFO backBufferBmpInfo;
	backBufferBmpInfo = { 0 };
	backBufferBmpInfo.bmiHeader.biBitCount = devMode.dmBitsPerPel;
	backBufferBmpInfo.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
	backBufferBmpInfo.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
	backBufferBmpInfo.bmiHeader.biPlanes = 1;
	backBufferBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	void* backBufferPixels = nullptr;
	POINT res = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	HBRUSH bgTransparencyColor = CreateSolidBrush(TRANSPARENCY_COLOR);
	HFONT hTitleFont = CreateFontA(50, 0, 0, 0, FW_DONTCARE, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
	HBRUSH hbrRed = CreateSolidBrush(RGB(255, 0, 0));
	HPEN hPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

	/*
	* LOOOP <3
	*/
	while (true) {
		if (!IsWindow(hwnd))
			exit(EXIT_SUCCESS);

		/*
		Updating Memory Address before doing anything...
		*/
		Mem.UpdateAddresses();

		/*
		Preparing the frame for the next draw
		*/
		HDC hdcBackBuffer = CreateCompatibleDC(hdc);
		SetBkMode(hdcBackBuffer, TRANSPARENT);
		HBITMAP hbmBackBuffer = CreateDIBSection(hdcBackBuffer, (BITMAPINFO *)&backBufferBmpInfo, DIB_RGB_COLORS, (void **)&backBufferPixels, NULL, 0);
		DeleteObject(SelectObject(hdcBackBuffer, hbmBackBuffer));
		DeleteObject(SelectObject(hdcBackBuffer, bgTransparencyColor));
		DeleteObject(SelectObject(hdcBackBuffer, hPen));
		Rectangle(hdcBackBuffer, 0, 0, res.x, res.y);

		//Looking each entity .... (0 to 12) i think is already too much for multiplayer entity
		for (int i = 0; i < 12; i++) {
			/*
			Gettin Entity and Information about
			*/
			DWORD_PTR Entity = Mem.GetEntity(i);
			PlayerInfo Player = Mem.GetAllEntityInfo(Entity);

			DWORD_PTR LocalEntity = Mem.GetLocalEntity();
			PlayerInfo LocalPlayer = Mem.GetAllEntityInfo(LocalEntity);

			//Checking if Entity is a valid entity
			if (Is_PlayerValid(Player, LocalPlayer)) {
				// Draw calls
				wstring text = L"Ennemy";
				text.assign(Player.Name.begin(), Player.Name.end());
				RECT textPos;

				//Not perfect box :/
				SelectObject(hdcBackBuffer, hbrRed);
				Rectangle(hdcBackBuffer, Player.w2sHead.x - 20, Player.w2sHead.y - 21, Player.w2sHead.x + 20, Player.w2sHead.y + 20);
				SelectObject(hdcBackBuffer, bgTransparencyColor);
				Rectangle(hdcBackBuffer, Player.w2sHead.x - 17, Player.w2sHead.y - 19, Player.w2sHead.x + 17, Player.w2sHead.y + 17);

				textPos.left = Player.w2s.x;
				textPos.right = Player.w2s.x;
				textPos.top = Player.w2s.y;
				textPos.bottom = Player.w2s.y;
				SetTextColor(hdcBackBuffer, RGB(255, 0, 0));
				DrawTextExW(hdcBackBuffer, (wchar_t*)text.c_str(), text.size(), &textPos, DT_CENTER | DT_NOCLIP | DT_NOPREFIX, NULL);	
			}
		}
		// Frame presentation
		BitBlt(hdc, 0, 0, res.x, res.y, hdcBackBuffer, 0, 0, SRCCOPY);

		// Cleanup
		DeleteDC(hdcBackBuffer); // Delete back buffer device context
		DeleteObject(hbmBackBuffer); // Delete back buffer bitmap
		backBufferPixels = nullptr;
	}
}