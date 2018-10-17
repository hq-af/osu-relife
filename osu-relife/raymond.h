/// https://blogs.msdn.microsoft.com/oldnewthing/20131118-00/?p=2643

#ifndef _H_RAYMOND
#define _H_RAYMOND
#include <windows.h>
#include <shldisp.h>
#include <shlobj.h>
#include <exdisp.h>
#include <atlbase.h>
#include <stdlib.h>

void ShellExecuteFromExplorer(
	PCWSTR pszFile,
	PCWSTR pszParameters,
	PCWSTR pszDirectory,
	PCWSTR pszOperation,
	int nShowCmd);

#endif
