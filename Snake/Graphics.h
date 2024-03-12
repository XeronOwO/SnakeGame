#pragma once

#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class CGraphics
{
private:
	CWnd* m_pWnd;
	UINT m_FPS;
	IDWriteTextFormat* m_FPSTextFormat = NULL, * m_scoreTextFormat, * m_deathTextFormat;
	ID2D1SolidColorBrush* m_pWhiteBrush = NULL, * m_pBackGroundBrush, * m_pSnakeBrush, * m_pSnackBrush, * m_pYellowBrush, * m_pAquaBrush;
	ID2D1Factory* m_pFactory = NULL;
	ID2D1HwndRenderTarget* m_pRenderTarget = NULL;
	IWICImagingFactory* m_pIWICFactory = NULL;
	IDWriteFactory* m_pDWriteFactory = NULL;
	VOID ShowError(CString szText, HRESULT hr);
	static UINT ThreadDraw(LPVOID lpParam);
	static UINT ThreadFPS(LPVOID lpParam);
	CWinThread* m_threadDraw;
	CWinThread* m_threadFPS;
	UINT m_actualFPS = 0;
	UINT m_frames = 0;
	UINT m_lastFrames = 0;
	BOOL m_exit = FALSE;
	BOOL m_init = FALSE;
	UINT m_unitSize = 10;
	D2D1_SIZE_U m_size;
	BOOL m_resize = FALSE;

public:
	static CGraphics Instance;
	HRESULT Initialize(CWnd* pWnd, UINT FPS);
	VOID SetFPS(UINT newFPS);
	VOID Exit();
	VOID FreshSize(int cx, int cy);
};

