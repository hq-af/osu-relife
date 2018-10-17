#include "Modes.h"

int SwitchMode(int argc, char* argv[]) {
	EnsureAdmin(argc, argv);

	if (KeyExists()) {
		if (MessageBoxA(NULL, "Osu-Relife is already enabled, would you like to temporarily disable it ?", "Disable Osu-Relife", MB_YESNO | MB_ICONWARNING) != IDYES)
			return 0;

		RemoveKey();
		MessageBoxA(NULL, "Successfully disabled (relaunch to re-enable)", "Success", MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	CreateKey();
	MessageBoxA(NULL, "Successfully enabled", "Success", MB_OK | MB_ICONINFORMATION);

	return 0;
}

int InstallMode(int argc, char* argv[]) {
	CreateKey();
	MessageBoxA(NULL, "Successfully installed, enjoy !", "Success", MB_OK | MB_ICONINFORMATION);

	return 0;
}

int UninstallMode(int argc, char* argv[]) {
	RemoveKey();

	return 0;
}


int PrepareInjectMode(int argc, char* argv[]) {
	EnsureAdmin(argc, argv);

	// Compatibilty
	std::string real_path(GetPathOfExe());
	real_path = real_path.substr(0, real_path.rfind('\\'));

	std::string oldP;
	oldP += real_path;
	oldP += "\\profiles.ini";

	std::string newP;
	newP += getenv("LOCALAPPDATA");
	newP += "\\relife_profiles.ini";

	FILE* oldconfig = fopen(oldP.c_str(), "r");

	if (oldconfig) {

		std::ifstream source(oldconfig);
		FILE* newconfig = fopen(newP.c_str(), "w");
		std::ofstream dest(newconfig);

		dest << source.rdbuf();

		fclose(oldconfig);
		fclose(newconfig);

		remove(oldP.c_str());
	}

	RemoveKey();

	std::string file;
	std::string args;

	file += argv[0];
	args += "/inject ";

	for (int i = 1; i < argc; i++) {
		args += argv[i];
		args += " ";
	}


	std::wstring sargs = std::wstring(args.begin(), args.end());
	std::wstring sfile = std::wstring(file.begin(), file.end());

	ShellExecuteFromExplorer(sfile.c_str(), sargs.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

	auto time = std::chrono::milliseconds(50);

	while (!GetProcessByName("osu!.exe"))
		std::this_thread::sleep_for(time);

	CreateKey();

	return 0;
}

int InjectMode(int argc, char* argv[]) {

	std::string cmd;

	for (int i = 2; i < argc; i++) {
		cmd += argv[i];
		cmd += " ";
	}

	PROCESS_INFORMATION pi;
	STARTUPINFOA si{};
	si.cb = sizeof(STARTUPINFOA);

	std::string real_path_str(GetPathOfExe());

	CreateProcessA(
		0,
		(LPSTR)cmd.c_str(),
		0,
		0,
		false,
		CREATE_SUSPENDED,
		0,
		0,
		&si,
		&pi);


	// Injecting
	std::string lib_str(GetPathOfExe());
	auto index = lib_str.rfind("osu-relife.exe");
	const char* libName = "pill.dll";
	lib_str.replace(index, 8, "pill.dll");
	lib_str.resize(lib_str.size() - 6);

	auto lib = lib_str.c_str();

	auto addr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	auto arg = (LPVOID)VirtualAllocEx(pi.hProcess, NULL, strlen(lib), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(pi.hProcess, arg, lib, strlen(lib), NULL);
	auto threadID = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)addr, arg, NULL, NULL);

	ResumeThread(pi.hThread);

	return 0;
}

int ForwardToOsuMode(int argc, char* argv[]) {

	std::string cmd;

	for (int i = 1; i < argc; i++) {
		cmd += argv[i];
		cmd += " ";
	}

	PROCESS_INFORMATION pi;
	STARTUPINFOA si{};
	si.cb = sizeof(STARTUPINFOA);

	CreateProcessA(
		0,
		(LPSTR)cmd.c_str(),
		0,
		0,
		false,
		DEBUG_ONLY_THIS_PROCESS,
		0,
		0,
		&si,
		&pi);

	DEBUG_EVENT evnt;
	ZeroMemory(&evnt, sizeof(DEBUG_EVENT));

	for (;;) {
		if (!WaitForDebugEvent(&evnt, INFINITE))
			return 1;

		if (evnt.dwDebugEventCode == 5)
			_exit(0);

		ContinueDebugEvent(evnt.dwProcessId, evnt.dwThreadId, DBG_CONTINUE);
	}

	return 0;
}
