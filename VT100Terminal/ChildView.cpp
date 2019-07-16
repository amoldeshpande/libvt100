
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "VT100Terminal.h"
#include "ChildView.h"
#include <gdiplusgraphics.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;

// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	Graphics graphics(::GetDC(this->GetSafeHwnd()));

	// Create a string.
	WCHAR string[] = L"Sample Text";

	// Initialize arguments.
	Gdiplus::Font myFont(L"Arial", 16);
	PointF origin(0.0f, 0.0f);
	SolidBrush blackBrush(Color(255, 0, 0, 0));

	// Draw string.
	graphics.DrawString( string, 11, &myFont, origin, &blackBrush);
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

