
// VT100Terminal.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include <memory>
#include <gdiplus.h>

#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include <afunix.h>
#include "VT100Terminal.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// updates its thread attribute list with the specified ConPTY handle
HRESULT InitializeStartupInfoAttachedToConPTY(STARTUPINFOEX* siEx, HPCON hPC)
{
	HRESULT hr = E_UNEXPECTED;
	size_t size;

	siEx->StartupInfo.cb = sizeof(STARTUPINFOEX);

	// Create the appropriately sized thread attribute list
	InitializeProcThreadAttributeList(NULL, 1, 0, &size);

	// Set startup info's attribute list & initialize it
	siEx->lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(malloc(size));
	bool fSuccess = InitializeProcThreadAttributeList( siEx->lpAttributeList, 1, 0, (PSIZE_T)& size);

	if (fSuccess)
	{
		// Set thread attribute list's Pseudo Console to the specified ConPTY
		fSuccess = UpdateProcThreadAttribute(
			siEx->lpAttributeList,
			0,
			PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
			hPC,
			sizeof(HPCON),
			NULL,
			NULL);
		return fSuccess ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	return hr;
}
HRESULT CreatePseudoConsoleAndPipes(HPCON* phPC,COORD consoleSize, HANDLE* phPipeIn, HANDLE* phPipeOut)
{
	HRESULT hr{ E_UNEXPECTED };
	HANDLE hPipePTYIn{ INVALID_HANDLE_VALUE };
	HANDLE hPipePTYOut{ INVALID_HANDLE_VALUE };

	// Create the pipes to which the ConPTY will connect
	if (CreatePipe(&hPipePTYIn, phPipeOut, NULL, 0) &&
		CreatePipe(phPipeIn, &hPipePTYOut, NULL, 0))
	{

		// Create the Pseudo Console of the required size, attached to the PTY-end of the pipes
		hr = CreatePseudoConsole(consoleSize, hPipePTYIn, hPipePTYOut, 0, phPC);

		// Note: We can close the handles to the PTY-end of the pipes here
		// because the handles are dup'ed into the ConHost and will be released
		// when the ConPTY is destroyed.
		if (INVALID_HANDLE_VALUE != hPipePTYOut) CloseHandle(hPipePTYOut);
		if (INVALID_HANDLE_VALUE != hPipePTYIn) CloseHandle(hPipePTYIn);
	}

	return hr;
}


// CVT100TerminalApp
// The one and only CVT100TerminalApp object
CVT100TerminalApp theApp;

BEGIN_MESSAGE_MAP(CVT100TerminalApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CVT100TerminalApp::OnAppAbout)
	ON_COMMAND(ID_LAUNCH, &CVT100TerminalApp::OnLaunch)
END_MESSAGE_MAP()


static ULONG_PTR GdiPlusToken;
using namespace Gdiplus;

// CVT100TerminalApp construction

CVT100TerminalApp::CVT100TerminalApp() noexcept
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("VT100Terminal.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	GdiplusStartupInput input = { 0 };
	input.GdiplusVersion = 1;
	GdiplusStartup(&GdiPlusToken,&input,NULL);
}
CVT100TerminalApp::~CVT100TerminalApp()
{
	GdiplusShutdown(GdiPlusToken);
}

void CVT100TerminalApp::SetTitle(libVT100::XtermDecoder&, libVT100::String& title)
{
	CMainFrame* frame = reinterpret_cast<CMainFrame*>(m_pMainWnd);
	CString* titleStr = new CString(title.c_str());
	frame->SetTitle(*titleStr);
	frame->OnUpdateFrameTitle(TRUE);
}
libVT100::String CVT100TerminalApp::GetDeviceCode(VT100Decoder& _decoder)
{
	throw std::logic_error("The method or operation is not implemented.");
}

libVT100::DeviceStatus CVT100TerminalApp::GetDeviceStatus(VT100Decoder& _decoder)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::ResizeWindow(VT100Decoder& _decoder, SIZE _size)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::MoveWindow(VT100Decoder& _decoder, libVT100::Point _position)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::Characters(AnsiDecoder& _sender, std::vector<WCHAR>& _chars) 
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::SaveCursor(AnsiDecoder& _sernder)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::MoveCursor(AnsiDecoder& _sender, Direction _direction, int _amount)
{
	throw std::logic_error("The method or operation is not implemented.");
}
void CVT100TerminalApp::MoveCursorToBeginningOfLineAbove(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::ClearScreen(AnsiDecoder& _sender, ClearDirection _direction)
{
	switch (_direction)
	{
		case ClearDirection::Both:
			m_pMainWnd->Invalidate();
			break;
		default:
			throw std::logic_error("The method or operation is not implemented.");
			break;
	}
}


void CVT100TerminalApp::ScrollPageUpwards(AnsiDecoder& _sender, int _linesToScroll)
{
	throw std::logic_error("The method or operation is not implemented.");
}
libVT100::Point CVT100TerminalApp::GetCursorPosition(AnsiDecoder& _sender)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::ScrollPageDownwards(AnsiDecoder& _sender, int _linesToScroll)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::SetGraphicRendition(AnsiDecoder& _sender, std::vector<GraphicRendition>& _commands)
{
	for (auto c : _commands)
	{
		switch (c)
		{
			case GraphicRendition::Reset:
				break;
			default:
				throw std::logic_error("The method or operation is not implemented.");
				break;
		}
	}
}

void CVT100TerminalApp::ModeChanged(AnsiDecoder& _sender, AnsiMode _mode)
{
	switch (_mode)
	{
		case AnsiMode::HideCursor:
			ShowCursor(FALSE);
			break;
		case AnsiMode::ShowCursor:
			ShowCursor(TRUE);
			break;
		default:
			throw std::logic_error("The method or operation is not implemented.");
			break;
	}
}

void CVT100TerminalApp::MoveCursorToColumn(AnsiDecoder& _sender, int _columnNumber)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::OneCharacter(AnsiDecoder& _sender, WCHAR _char)
{
	CMainFrame* frame = reinterpret_cast<CMainFrame*>(m_pMainWnd);
	frame->OneCharacter(_char);
	//throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::RestoreCursor(AnsiDecoder& _sender)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::MoveCursorToBeginningOfLineBelow(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::MoveCursorTo(AnsiDecoder& _sender, libVT100::Point _position)
{
	CMainFrame* frame = reinterpret_cast<CMainFrame*>(m_pMainWnd);
	frame->SetChildCursorPos(COORD{(SHORT) _position.x,(SHORT)_position.y });
}

void CVT100TerminalApp::ClearLine(AnsiDecoder& _sender, ClearDirection _direction)
{
	throw std::logic_error("The method or operation is not implemented.");
}

SIZE CVT100TerminalApp::GetSize(AnsiDecoder& _sender)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVT100TerminalApp::OnInputBytes(byte* array, int len)
{
	m_XtermDecoder->Input(array, len);
	delete[] array;
}

// CVT100TerminalApp initialization

BOOL CVT100TerminalApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	WSADATA wsd;
	WSAStartup(WINSOCK_VERSION, &wsd);

	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CFrameWnd* pFrame = new CMainFrame(this);
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);



	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

int CVT100TerminalApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	return CWinApp::ExitInstance();
}

// CVT100TerminalApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// App command to run the dialog
void CVT100TerminalApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CVT100TerminalApp::OnLaunch()
{
	HANDLE hPipeIn{ INVALID_HANDLE_VALUE };
	HANDLE hPipeOut{ INVALID_HANDLE_VALUE };
	const TCHAR szFilter[] = _T("Exe Files (*.exe)|*.exe|All Files (*.*)|*.*||");
	CString filePath;

	CFileDialog dlg(TRUE, _T("exe"), NULL,0, szFilter, this->m_pMainWnd);
	if (dlg.DoModal() == IDOK)
	{
		filePath = dlg.GetPathName();
		//filePath.Append(L" -l");
	}
	m_XtermDecoder = std::make_shared<XtermDecoder>();
	m_XtermDecoder->Subscribe(this);

	{
		RECT clientRect;
		HPCON hPC;
		STARTUPINFOEX siEx{};
		m_pMainWnd->GetClientRect(&clientRect);

		 // Create the Pseudo Console and pipes to it
		 CreatePseudoConsoleAndPipes(&hPC,{ (short)(clientRect.right - clientRect.left),(short)(clientRect.bottom - clientRect.top) }, &hPipeIn, &hPipeOut);

		InitializeStartupInfoAttachedToConPTY(&siEx, hPC);

		PROCESS_INFORMATION pi{};
		CreateProcessW(NULL,(wchar_t*)(LPCTSTR)filePath,
					   //NULL,
					   NULL,
					   NULL,
					   FALSE,
					   EXTENDED_STARTUPINFO_PRESENT,
					   NULL,
					   NULL,
					   &siEx.StartupInfo,
					   &pi);
	}

	std::thread t([this, hPipeIn]
				  {
					  while (true)
					  {
						  char* buf = new char[80];
						  DWORD bread;
						  BOOL rc = ReadFile(hPipeIn, buf, 80, &bread, NULL);
						  if (rc == FALSE)
						  {
							  delete[] buf;
							  break;
						  }
						  PostMessage(m_pMainWnd->GetSafeHwnd(), WM_INPUT_BYTES, (WPARAM)buf, (LPARAM)bread);
					  }
				});
	t.detach();
}
// CVT100TerminalApp message handlers


