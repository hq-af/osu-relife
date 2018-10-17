#ifndef _H_UTILS
#define _H_UTILS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <shellapi.h>

/// PROC
int GetProcessByName(PCSTR name);
LPSTR GetPathOfExe();

/// UAC
bool IsRunAsAdministrator();
void ElevateNow(const char* params);
int EnsureAdmin(int argc, char* argv[]);

/// REG
bool KeyExists();
void RemoveKey();
void CreateKey();
#endif