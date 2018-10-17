/// https://blogs.msdn.microsoft.com/oldnewthing/20131118-00/?p=2643

#include "Raymond.h"

class CCoInitialize {
public:
	CCoInitialize() : m_hr(CoInitialize(NULL)) { }
	~CCoInitialize() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
	operator HRESULT() const { return m_hr; }
	HRESULT m_hr;
};

void FindDesktopFolderView(REFIID riid, void **ppv)
{
	CComPtr<IShellWindows> spShellWindows;
	spShellWindows.CoCreateInstance(CLSID_ShellWindows);

	CComVariant vtLoc(CSIDL_DESKTOP);
	CComVariant vtEmpty;
	long lhwnd;
	CComPtr<IDispatch> spdisp;
	spShellWindows->FindWindowSW(
		&vtLoc, &vtEmpty,
		SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &spdisp);

	CComPtr<IShellBrowser> spBrowser;
	CComQIPtr<IServiceProvider>(spdisp)->
		QueryService(SID_STopLevelBrowser,
			IID_PPV_ARGS(&spBrowser));

	CComPtr<IShellView> spView;
	spBrowser->QueryActiveShellView(&spView);

	spView->QueryInterface(riid, ppv);
}

void GetDesktopAutomationObject(REFIID riid, void **ppv)
{
	CComPtr<IShellView> spsv;
	FindDesktopFolderView(IID_PPV_ARGS(&spsv));
	CComPtr<IDispatch> spdispView;
	spsv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&spdispView));
	spdispView->QueryInterface(riid, ppv);
}

void ShellExecuteFromExplorer(
	PCWSTR pszFile,
	PCWSTR pszParameters,
	PCWSTR pszDirectory,
	PCWSTR pszOperation,
	int nShowCmd)
{
	CCoInitialize init;

	CComPtr<IShellFolderViewDual> spFolderView;
	GetDesktopAutomationObject(IID_PPV_ARGS(&spFolderView));
	CComPtr<IDispatch> spdispShell;
	spFolderView->get_Application(&spdispShell);

	CComQIPtr<IShellDispatch2>(spdispShell)
		->ShellExecute(CComBSTR(pszFile),
			CComVariant(pszParameters ? pszParameters : L""),
			CComVariant(pszDirectory ? pszDirectory : L""),
			CComVariant(pszOperation ? pszOperation : L""),
			CComVariant(nShowCmd));
}

