#include "Utils.h"

int GetProcessByName(PCSTR name)
{
	DWORD pid = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (std::string(process.szExeFile) == std::string(name))
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0)
		return pid;

	return 0;
}

LPSTR GetPathOfExe()
{
	char filename[MAX_PATH];
	DWORD result = ::GetModuleFileName(
		nullptr,
		(LPSTR)filename,
		_countof(filename)
	);

	return filename;
}

bool IsRunAsAdministrator()
{
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = NULL;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

Cleanup:
	if (pAdministratorsGroup)
	{
		FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = NULL;
	}

	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return fIsRunAsAdmin;
}

void ElevateNow(const char* params)
{
	BOOL bAlreadyRunningAsAdministrator = FALSE;
	try
	{
		bAlreadyRunningAsAdministrator = IsRunAsAdministrator();
	}
	catch (...) { }
	if (!bAlreadyRunningAsAdministrator)
	{
		wchar_t szPath[MAX_PATH];
		if (GetModuleFileName(NULL, (LPSTR)szPath, ARRAYSIZE(szPath)))
		{
			SHELLEXECUTEINFOA sei = { sizeof(sei) };
			sei.lpVerb = "runas";
			sei.lpFile = (LPCSTR)szPath;
			sei.hwnd = NULL;
			sei.nShow = SW_NORMAL;
			sei.lpParameters = (LPCSTR)params;

			if (!ShellExecuteEx(&sei))
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_CANCELLED)
					_exit(1);
			}
			else
				_exit(1);
		}
	}
}

int EnsureAdmin(int argc, char* argv[]) {
	if (!IsRunAsAdministrator()) {
		std::string strArgs("");
		for (int i = 1; i < argc; i++) {
			strArgs.append(argv[i]);
			if (i < argc - 1)
				strArgs.append(" ");
		}

		ElevateNow(strArgs.c_str());
	}

	return 0;
}

void RemoveKey() {
	RegDeleteKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\osu!.exe"));
}

void CreateKey() {
	HKEY createdKey = nullptr;
	RegCreateKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\osu!.exe"), &createdKey);

	char* path = GetPathOfExe();

	RegSetValueEx(createdKey, "Debugger", 0, REG_SZ, (BYTE*)path, strlen(path));

	RegCloseKey(createdKey);
}

bool KeyExists() {
	HKEY subKey = nullptr;
	auto result = RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\osu!.exe"), &subKey);
	RegCloseKey(subKey);

	return result == ERROR_SUCCESS;
}