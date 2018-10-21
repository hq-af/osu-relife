#ifndef _H_HOOKS
#define _H_HOOKS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <iostream>
#include <WbemCli.h>
#include <Wincrypt.h>
#include "../MinHook/MinHook.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "../MinHook/libMinHook.x86.lib")

typedef ULONG(__stdcall *GETADAPTERSADDRESSES) (ULONG, ULONG, PVOID, PIP_ADAPTER_ADDRESSES, PULONG);
typedef HRESULT(__stdcall* WMIGET)(void* pThis, LPCWSTR, LONG, VARIANTARG*, CIMTYPE, LONG);
typedef LSTATUS (__stdcall* REGQUERYVALUEEXW)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef INT32(__stdcall* GETADDRINFOW)(PCWSTR, PCWSTR, const ADDRINFOW*, PADDRINFOW*);

INT32 __stdcall DetourGetAddrInfoW(
	PCWSTR          pNodeName,
	PCWSTR          pServiceName,
	const ADDRINFOW *pHints,
	PADDRINFOW      *ppResult);

ULONG __stdcall DetourGetAdaptersAddresses(
	ULONG                 Family,
	ULONG                 Flags,
	PVOID                 Reserved,
	PIP_ADAPTER_ADDRESSES AdapterAddresses,
	PULONG                SizePointer);

HRESULT __stdcall DetourWMIGet(
	void* pThis,
	LPCWSTR wszName,
	LONG lFlags,
	VARIANTARG *pVal,
	CIMTYPE pvtType,
	LONG plFlavor);

LSTATUS __stdcall DetourRegQueryValueExW(
	HKEY    hKey,
	LPCSTR  lpValueName,
	LPDWORD lpReserved,
	LPDWORD lpType,
	LPBYTE  lpData,
	LPDWORD lpcbData
);


void Initialize_Hooks();
void MAC_Hook();
void WMIGet_Hook();
void RegQuery_Hook();
void AddrInfoW_Hook();


//------------------ PlayGround


//---------- s.H_CertFindCertificateInCRL
typedef BOOL(__stdcall* CERTFINDCERTIFICATEINCRL)(PCCERT_CONTEXT, PCCRL_CONTEXT, DWORD, void*, PCRL_ENTRY*);
BOOL __stdcall DetourCertFindCertificateInCRL(PCCERT_CONTEXT arg0, PCCRL_CONTEXT arg1, DWORD arg2, void* arg3, PCRL_ENTRY* arg4);
void Hook_CertFindCertificateInCRL();
//---------- e.H_CertFindCertificateInCRL



#endif