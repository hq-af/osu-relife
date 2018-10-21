#include "hooks.h"
#include "factory.h"

GETADAPTERSADDRESSES fpsGetAdaptersAddresses = NULL;
WMIGET fpsWMIGet = NULL;
REGQUERYVALUEEXW fpsRegQuery = NULL;
GETADDRINFOW fpsGetAddrInfoW = NULL;

void Initialize_Hooks() {
	MH_Initialize();
	//std::cout << "hooks initialized" << std::endl;
}

void MAC_Hook() {

	auto target = GetProcAddress(LoadLibrary(L"IPHLPAPI.DLL"), "GetAdaptersAddresses");

	MH_CreateHook(target, &DetourGetAdaptersAddresses, (LPVOID*)&fpsGetAdaptersAddresses);
	MH_EnableHook(target);
	//std::cout << "created hook for 'iphlpapi.dll@GetAdaptersAddresses'" << std::endl;
}

ULONG __stdcall DetourGetAdaptersAddresses(
	ULONG                 Family,
	ULONG                 Flags,
	PVOID                 Reserved,
	PIP_ADAPTER_ADDRESSES AdapterAddresses,
	PULONG                SizePointer) {

	auto hRet = fpsGetAdaptersAddresses(Family, Flags, Reserved, AdapterAddresses, SizePointer);
	
	if (AdapterAddresses != NULL)
		*AdapterAddresses = *GetMAC();

	//std::cout << "called 'iphlpapi.dll@GetAdaptersAddresses'" << std::endl;
	
	return hRet;
}

void WMIGet_Hook() {
	auto target = GetProcAddress(LoadLibrary(L"fastprox.dll"), "?Get@CWbemObject@@UAGJPBGJPAUtagVARIANT@@PAJ2@Z");
	MH_CreateHook(target, &DetourWMIGet, (LPVOID*)&fpsWMIGet);
	MH_EnableHook(target);
	//std::cout << "created hook for 'fastprox.dll@Get'" << std::endl;
}

HRESULT __stdcall DetourWMIGet(
	void* pThis,
	LPCWSTR wszName,
	LONG lFlags,
	VARIANTARG *pVal,
	CIMTYPE pvtType,
	LONG plFlavor) {

	//std::wcout << "called 'fastprox.dll@Get' : " << wszName << std::endl;

	auto hRet = fpsWMIGet(pThis, wszName, lFlags, pVal, pvtType, plFlavor);

	if (wcscmp(wszName, L"Signature") == 0) {
		VariantClear(pVal);
	}
	else if (wcscmp(wszName, L"SerialNumber") == 0) {
		VariantClear(pVal);
		pVal->vt = VT_BSTR;
		pVal->bstrVal = GetSerialNumber();
	}

	return hRet;
}

void RegQuery_Hook() {
	auto target = GetProcAddress(LoadLibrary(L"ADVAPI32.DLL"), "RegQueryValueExW");

	MH_CreateHook(target, &DetourRegQueryValueExW, (LPVOID*)&fpsRegQuery);
	MH_EnableHook(target);
	//std::cout << "created hook for 'advapi32.dll@RegQueryValueExW'" << std::endl;
}

LSTATUS __stdcall DetourRegQueryValueExW(
	HKEY    hKey,
	LPCSTR  lpValueName,
	LPDWORD lpReserved,
	LPDWORD lpType,
	LPBYTE  lpData,
	LPDWORD lpcbData
) {

	auto hRet = fpsRegQuery(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

	//std::wcout << "called 'advapi32.dll@RegQueryValueExW'" << std::endl;
	
	if (lstrcmp((LPCWSTR)lpValueName, L"UninstallID") == 0 && lpData != NULL) {
		auto tab = GetUninstallID();

		int i = 0;
		while (lpData[i] != 0) {
			lpData[i] = tab[i / 2];
			i += 2;
		}
	}
	

	return hRet;
}

void AddrInfoW_Hook() {
	auto target = GetProcAddress(LoadLibrary(L"WS2_32.dll"), "GetAddrInfoW");
	MH_CreateHook(target, &DetourGetAddrInfoW, (LPVOID*)&fpsGetAddrInfoW);
	MH_EnableHook(target);
	//std::cout << "created hook for 'WS2_32.dll@GetAddrInfoW'" << std::endl;
}

INT32 __stdcall DetourGetAddrInfoW(
	PCWSTR          pNodeName,
	PCWSTR          pServiceName,
	const ADDRINFOW *pHints,
	PADDRINFOW      *ppResult) {

	//std::wcout << "called 'WS2_32.dll@GetAddrInfoW' : " << pNodeName << std::endl;

	//return fpsGetAddrInfoW(L"kawata.pw", pServiceName, pHints, ppResult);
	return fpsGetAddrInfoW(pNodeName, pServiceName, pHints, ppResult);
}


//----------- PlayGround

//---------- s.C_CertFindCertificateInCRL
CERTFINDCERTIFICATEINCRL fpsCertFindCertificateInCRL = NULL;

void Hook_CertFindCertificateInCRL() {
	auto target = GetProcAddress(LoadLibrary(L"crypt32.dll"), "CertFindCertificateInCRL");
	MH_CreateHook(target, &DetourCertFindCertificateInCRL, (LPVOID*)&fpsCertFindCertificateInCRL);
	MH_EnableHook(target);
	std::cout << "created hook for 'crypt32.dll@CertFindCertificateInCRL'" << std::endl;
}

BOOL __stdcall DetourCertFindCertificateInCRL(PCCERT_CONTEXT arg0, PCCRL_CONTEXT arg1, DWORD arg2, void* arg3, PCRL_ENTRY* arg4) {
	std::cout << "called 'crypt32.dll@CertFindCertificateInCRL'" << std::endl;

	return fpsCertFindCertificateInCRL(arg0, arg1, arg2, arg3, arg4);
}
//---------- e.C_CertFindCertificateInCRL
