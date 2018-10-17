#define _CRT_SECURE_NO_WARNINGS
#include "ui.h"
#include "factory.h"
#include <CommCtrl.h>
#include <atlstr.h>
#define HINST_THISCOMPONENT GetModuleHandle(NULL)
#define LOAD_BTN 4800
#define ADD_BTN 4801
#define DEL_BTN 4802
#define PICKER_COMBO 4803
#define GENERATE_BTN 4804
#define NAME_INPUT 4805
#define NAME_BTN 4805




HINSTANCE  inj_hModule;          //Injected Modules Handle
HWND       prnt_hWnd;
HWND picker_combo;
HWND mac_input;
HWND hwid_input;
HWND uninstall_input;
HWND messageBox = NULL;
HWND name_input = NULL;

TCHAR** t_profiles = NULL;
TCHAR A[MAX_NAME];

void RefreshUI() {
	
	if (t_profiles != NULL) {
		for (int i = 0; i < profilesCount; i++) {
			free(t_profiles[i]);
		}
		free(t_profiles);
	}
	t_profiles = (TCHAR**)malloc(profilesCount * sizeof(TCHAR*));

	profile* curr = first_profile;
	int i = 0;
	while (curr) {
		t_profiles[i] = (TCHAR*)calloc(MAX_NAME, sizeof(TCHAR));
		wcscpy_s(t_profiles[i], sizeof(A) / sizeof(TCHAR), CA2W(curr->name));
		curr = curr->next;
		i++;
	}

	SendMessage(picker_combo, (UINT)CB_RESETCONTENT, 0, 0);

	int  k = 0;
	memset(&A, 0, sizeof(A));
	for (k = 0; k < profilesCount; k++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)t_profiles[k]);

		// Add string to combobox.
		SendMessage(picker_combo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}
	SendMessage(picker_combo, CB_SETCURSEL, (WPARAM)GetSelectedProfileIndex(), (LPARAM)0);
	SendMessage(mac_input, EM_SETREADONLY, 1, 0);
	SendMessage(hwid_input, EM_SETREADONLY, 1, 0);
	SendMessage(uninstall_input, EM_SETREADONLY, 1, 0);

	auto selected = GetSelectedProfile();
	SendMessage(mac_input, WM_SETTEXT, 0, (LPARAM)((TCHAR*)CA2W(selected->mac)));
	SendMessage(hwid_input, WM_SETTEXT, 0, (LPARAM)((TCHAR*)CA2W(selected->hwid)));
	SendMessage(uninstall_input, WM_SETTEXT, 0, (LPARAM)((TCHAR*)CA2W(selected->uninstallID)));

}

LRESULT CALLBACK DLLWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{


	case WM_COMMAND:
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			SetSelected(ItemIndex);

			RefreshUI();
		}
		switch (wParam)
		{
		case (LOAD_BTN):
			SaveToDisk();
			PostQuitMessage(0);
			break;
		case (GENERATE_BTN):
			RandomizeSelected();
			RefreshUI();
			break;
		case (DEL_BTN):
			DeleteSelected();
			RefreshUI();
			break;
		case (ADD_BTN):
			ShowWindow(messageBox, SW_SHOW);
			break;
		case (NAME_BTN):

			char text[MAX_NAME];
			LRESULT result = SendMessage(name_input, WM_GETTEXT, sizeof(text), LPARAM(text));
			
			AddProfile(text);
			RefreshUI();
			ShowWindow(messageBox, SW_HIDE);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

BOOL RegisterDLLWindowClass(const wchar_t szClassName[])
{
	WNDCLASSEX wc;
	wc.hInstance = inj_hModule;
	wc.lpszClassName = (LPCWSTR)L"RelifePill";
	wc.lpszClassName = (LPCWSTR)szClassName;
	wc.lpfnWndProc = DLLWindowProc;
	wc.style = CS_DBLCLKS;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	if (!RegisterClassEx(&wc))
		return 0;
}

void Initialize_UI(HINSTANCE mod) {
	inj_hModule = mod;

	SetProcessDPIAware();
	MSG messages;
	RegisterDLLWindowClass(L"RelifePill");
	prnt_hWnd = FindWindow(L"Window Injected Into ClassName", L"Window Injected Into Caption");
	HWND hwnd = CreateWindowEx(0, L"RelifePill", L"Osu-Relife v0.2 by hq.af <github.com/hq-af>", WS_EX_PALETTEWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 250, prnt_hWnd, NULL, inj_hModule, NULL);
	ShowWindow(hwnd, SW_SHOWNORMAL);

	messageBox = CreateWindowEx(0, L"RelifePill", L"Add profile", WS_EX_PALETTEWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 150, hwnd, NULL, inj_hModule, NULL);
	name_input = CreateWindow(WC_EDIT, L"custom", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 20, 275, 25, messageBox, (HMENU)NAME_INPUT, HINST_THISCOMPONENT, 0);
	auto name_btn = CreateWindow(WC_BUTTON, L"Add", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 185, 70, 100, 30, messageBox, (HMENU)NAME_BTN, HINST_THISCOMPONENT, 0);
	ShowWindow(name_btn, SW_SHOW);
	ShowWindow(mac_input, SW_SHOW);

	

	HWND picker_label = CreateWindow(WC_STATIC, L"Profile :", WS_CHILD, 10, 20, 100, 18, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(picker_label, SW_SHOW);
	HWND mac_label = CreateWindow(WC_STATIC, L"MAC :", WS_CHILD, 10, 70, 100, 18, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(mac_label, SW_SHOW);
	HWND hwid_label = CreateWindow(WC_STATIC, L"HWID :", WS_CHILD, 10, 98, 100, 18, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(hwid_label, SW_SHOW);
	HWND uninstall_label = CreateWindow(WC_STATIC, L"UninstallID :", WS_CHILD, 10, 126, 100, 18, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(uninstall_label, SW_SHOW);

	mac_input = CreateWindow(WC_EDIT, 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 120, 66, 250, 25, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(mac_input, SW_SHOW);
	hwid_input = CreateWindow(WC_EDIT, 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 120, 94, 250, 25, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(hwid_input, SW_SHOW);
	uninstall_input = CreateWindow(WC_EDIT, 0, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 120, 124, 250, 25, hwnd, 0, HINST_THISCOMPONENT, 0);
	ShowWindow(uninstall_input, SW_SHOW);

	HWND generate_btn = CreateWindow(WC_BUTTON, L"Random", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 13, 170, 120, 30, hwnd, (HMENU)GENERATE_BTN, HINST_THISCOMPONENT, 0);
	ShowWindow(generate_btn, SW_SHOW);
	HWND load_btn = CreateWindow(WC_BUTTON, L"Load/Save", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 220, 170, 160, 30, hwnd, (HMENU)LOAD_BTN, HINST_THISCOMPONENT, 0);
	ShowWindow(load_btn, SW_SHOW);
	HWND add_btn = CreateWindow(WC_BUTTON, L"+", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 290, 17, 42, 25, hwnd, (HMENU)ADD_BTN, HINST_THISCOMPONENT, 0);
	ShowWindow(add_btn, SW_SHOW);
	HWND del_btn = CreateWindow(WC_BUTTON, L"x", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 337, 17, 42, 25, hwnd, (HMENU)DEL_BTN, HINST_THISCOMPONENT, 0);
	ShowWindow(del_btn, SW_SHOW);

	picker_combo = CreateWindow(WC_COMBOBOX, 0, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 100, 16, 180, 200, hwnd, (HMENU)PICKER_COMBO, HINST_THISCOMPONENT, NULL);
	ShowWindow(picker_combo, SW_SHOW);

	RefreshUI();
	
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}
}