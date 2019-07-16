
// VT100Terminal.h : main header file for the VT100Terminal application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CVT100TerminalApp:
// See VT100Terminal.cpp for the implementation of this class
//

using namespace libVT100;
class CVT100TerminalApp : public CWinApp, public IXtermDecoderClient
{
public:
	CVT100TerminalApp() noexcept;
	virtual ~CVT100TerminalApp();


	virtual void CVT100TerminalApp::SetTitle(libVT100::XtermDecoder&, libVT100::String& title) override;

	virtual String GetDeviceCode(VT100Decoder& _decoder) override;


	virtual DeviceStatus GetDeviceStatus(VT100Decoder& _decoder) override;

	virtual void ResizeWindow(VT100Decoder& _decoder, SIZE _size) override;

	virtual void MoveWindow(VT100Decoder& _decoder, Point _position) override;

	virtual void Characters(AnsiDecoder& _sender, std::vector<WCHAR>& _chars) override;

	virtual void SaveCursor(AnsiDecoder& _sernder) override;

	virtual void OneCharacter(AnsiDecoder& _sender, WCHAR _chars) override;

	virtual void RestoreCursor(AnsiDecoder& _sender) override;

	virtual SIZE GetSize(AnsiDecoder& _sender) override;

	virtual void MoveCursor(AnsiDecoder& _sender, Direction _direction, int _amount) override;

	virtual void MoveCursorToBeginningOfLineBelow(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine) override;

	virtual void MoveCursorToBeginningOfLineAbove(AnsiDecoder& _sender, int _lineNumberRelativeToCurrentLine) override;

	virtual void MoveCursorToColumn(AnsiDecoder& _sender, int _columnNumber) override;

	virtual void MoveCursorTo(AnsiDecoder& _sender, Point _position) override;

	virtual void ClearScreen(AnsiDecoder& _sender, ClearDirection _direction) override;

	virtual void ClearLine(AnsiDecoder& _sender, ClearDirection _direction) override;

	virtual void ScrollPageUpwards(AnsiDecoder& _sender, int _linesToScroll) override;

	virtual void ScrollPageDownwards(AnsiDecoder& _sender, int _linesToScroll) override;

	virtual Point GetCursorPosition(AnsiDecoder& _sender) override;

	virtual void ModeChanged(AnsiDecoder& _sender, AnsiMode _mode) override;

	virtual void SetGraphicRendition(AnsiDecoder& _sender, std::vector<GraphicRendition>& _commands) override;

	std::shared_ptr<XtermDecoder> m_XtermDecoder;

	void OnInputBytes(byte* array, int len);
	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	afx_msg void OnLaunch();
	DECLARE_MESSAGE_MAP()
};

extern CVT100TerminalApp theApp;
