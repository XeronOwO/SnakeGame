#include "pch.h"
#include "Graphics.h"
#include "GameHandler.h"
#include "SnakeDlg.h"

CGraphics CGraphics::Instance;

HRESULT CGraphics::Initialize(CWnd* pWnd, UINT FPS)
{
	m_pWnd = pWnd;
	m_FPS = FPS;
	HRESULT hr;

	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]执行CoInitialize失败！"), hr);
		return hr;
	}

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1Factory失败！"), hr);
		return hr;
	}

	CRect rect;
	GetClientRect(m_pWnd->m_hWnd, &rect);
	m_size = D2D1::SizeU(rect.right, rect.bottom);
	hr = m_pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_pWnd->m_hWnd, m_size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&m_pRenderTarget);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1HwndRenderTarget失败！"), hr);
		return hr;
	}

	hr = CoCreateInstance(
		CLSID_WICImagingFactory1,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&m_pIWICFactory));
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建IWICImagingFactory失败！"), hr);
		return hr;
	}

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建IDWriteFactory失败！"), hr);
		return hr;
	}

	hr = m_pDWriteFactory->CreateTextFormat(
		L"微软雅黑",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f,
		L"zh-cn",
		&m_FPSTextFormat
	);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建IDWriteTextFormat失败！"), hr);
		return hr;
	}

	hr = m_pDWriteFactory->CreateTextFormat(
		L"微软雅黑",
		NULL,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"zh-cn",
		&m_scoreTextFormat
	);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建IDWriteTextFormat失败！"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhiteBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1SolidColorBrush失败！"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &m_pBackGroundBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1SolidColorBrush失败！"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen), &m_pSnakeBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1SolidColorBrush失败！"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pSnackBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1SolidColorBrush失败！"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1SolidColorBrush失败！"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), &m_pAquaBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]创建ID2D1SolidColorBrush失败！"), hr);
		return hr;
	}

	m_threadDraw = AfxBeginThread(ThreadDraw, (LPVOID)this);
	m_threadFPS = AfxBeginThread(ThreadFPS, (LPVOID)this);
	m_init = TRUE;

	return TRUE;
}

VOID CGraphics::SetFPS(UINT newFPS)
{
	m_FPS = newFPS;
}

VOID CGraphics::Exit()
{
	m_exit = TRUE;
	if (m_threadDraw)
	{
		m_threadDraw->ExitInstance();
	}
	if (m_threadFPS)
	{
		m_threadFPS->ExitInstance();
	}
}

VOID CGraphics::FreshSize(int cx, int cy)
{
	if (m_init)
	{
		m_size.width = cx;
		m_size.height = cy;
		m_resize = TRUE;
	}
}

VOID CGraphics::ShowError(CString szText, HRESULT hr)
{
	CString s;
	LPVOID pvMsgBuf = NULL;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, (DWORD)hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pvMsgBuf, 0, NULL))
	{
		s.Format(_T("%s\n错误代码：[HRESULT]%d\n错误信息：%s"), szText, hr, (LPTSTR)pvMsgBuf);
		m_pWnd->MessageBox(s, _T("错误"), MB_OK | MB_ICONERROR);
	}
}

UINT CGraphics::ThreadDraw(LPVOID lpParam)
{
	CGraphics* pGraphics = (CGraphics*)lpParam;

	DWORD64 oneSecondStart = GetTickCount64();
	auto pList = CGameHandler::Instance.GetSnakePointsList();
	UINT FPSNow = 1;
	while (true)
	{
		if (pGraphics->m_FPS != 0)
		{
			if (FPSNow == pGraphics->m_FPS)
			{
				oneSecondStart = GetTickCount64();
				FPSNow = 1;
			}
		}

		if (pGraphics->m_resize)
		{
			pGraphics->m_resize = FALSE;
			pGraphics->m_pRenderTarget->Resize(pGraphics->m_size);
		}

		if (!pGraphics->m_exit)
		{
			pGraphics->m_pRenderTarget->BeginDraw();
			pGraphics->m_pRenderTarget->Clear(D2D1::ColorF(0, 0, 0));

			// FPS
			CString str;
			str.Format(_T("FPS: %d"), pGraphics->m_actualFPS);
			D2D1_RECT_F rect;
			UINT sizeX = CGameHandler::Instance.GetSizeX();
			UINT sizeY = CGameHandler::Instance.GetSizeY();
			rect.left = (FLOAT)(pGraphics->m_size.width / 2 - pGraphics->m_unitSize * sizeX / 2);
			rect.top = (FLOAT)(pGraphics->m_size.height / 2 - pGraphics->m_unitSize * sizeY / 2);
			rect.right = (FLOAT)(pGraphics->m_size.width / 2 + pGraphics->m_unitSize * sizeX / 2);
			rect.bottom = (FLOAT)(pGraphics->m_size.height / 2 + pGraphics->m_unitSize * sizeY / 2);
			pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(200, 200), pGraphics->m_pWhiteBrush);
			// 分数
//			str.Format(L"当前分数：%d\n历史最高分：%d", (pList->GetCount() - 3) * 10, CSnakeDlg::Instance->GetHighestScore());
//			pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.top - 60, rect.left + 200, rect.top + 30), pGraphics->m_pWhiteBrush);
			// 暂停提示
			if (!CGameHandler::Instance.IsDead())
			{
				str = CGameHandler::Instance.IsPaused() ? L"按任意方向键继续" : L"按P键暂停";
				pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.bottom + 30, rect.left + 200, rect.bottom + 80), pGraphics->m_pAquaBrush);
			}
			// 游戏信息 & 当前游戏设置
			str.Format(L"游戏信息：\n  当前分数：%d\n  历史最高分：%d\n  当前Tick：%.2lf\n\n当前游戏设置：\n  宽度：%d\n  高度：%d\n  初始Tick：%d\n  加速Tick：%.2lf\n  最小Tick：%d",
				CGameHandler::Instance.GetCurrentScore(),
				CSnakeDlg::Instance->GetHighestScore(),
				CGameHandler::Instance.GetCurrentTick(),
				CGameHandler::Instance.GetWidth(),
				CGameHandler::Instance.GetHeight(),
				CGameHandler::Instance.GetOriginalTick(),
				CGameHandler::Instance.GetAccelerationTick(),
				CGameHandler::Instance.GetMinTick()
			);
			pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left - 150, rect.top, rect.left, rect.top + 200), pGraphics->m_pWhiteBrush);

			// 贪吃蛇
			pGraphics->m_pRenderTarget->FillRectangle(rect, pGraphics->m_pBackGroundBrush);
			POSITION pos = pList->GetHeadPosition();
			while (pos)
			{
				auto v = pList->GetNext(pos);
				pGraphics->m_pRenderTarget->FillRectangle(D2D1::RectF(
					rect.left + v.x * pGraphics->m_unitSize,
					rect.top + v.y * pGraphics->m_unitSize,
					rect.left + (v.x + 1) * pGraphics->m_unitSize,
					rect.top + (v.y + 1) * pGraphics->m_unitSize
				), pGraphics->m_pSnakeBrush);
			}

			// 零食
			auto v = CGameHandler::Instance.GetSnackPoint();
			pGraphics->m_pRenderTarget->FillRectangle(D2D1::RectF(
				rect.left + v.x * pGraphics->m_unitSize,
				rect.top + v.y * pGraphics->m_unitSize,
				rect.left + (v.x + 1) * pGraphics->m_unitSize,
				rect.top + (v.y + 1) * pGraphics->m_unitSize
			), pGraphics->m_pSnackBrush);

			// 死亡信息
			if (CGameHandler::Instance.IsDead())
			{
				if (pList->GetCount() > 0)
				{
					v = pList->GetHead();
					pGraphics->m_pRenderTarget->FillRectangle(D2D1::RectF(
						rect.left + v.x * pGraphics->m_unitSize,
						rect.top + v.y * pGraphics->m_unitSize,
						rect.left + (v.x + 1) * pGraphics->m_unitSize,
						rect.top + (v.y + 1) * pGraphics->m_unitSize
					), pGraphics->m_pYellowBrush);
					str = "你死了，按R键重玩。";
					pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.bottom + 10, rect.left + 200, rect.bottom + 60), pGraphics->m_pSnackBrush);
				}
			}
			else
			{
				str = "按R键重玩";
				pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.bottom + 10, rect.left + 200, rect.bottom + 60), pGraphics->m_pAquaBrush);
			}

			str = "Game Author: Sun Jiahao";
			pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(
				(FLOAT)CGraphics::Instance.m_size.width / 2 - 100,
				(FLOAT)CGraphics::Instance.m_size.height - 30,
				(FLOAT)CGraphics::Instance.m_size.width / 2 + 100,
				(FLOAT)CGraphics::Instance.m_size.height),
				pGraphics->m_pYellowBrush
			);

			pGraphics->m_pRenderTarget->EndDraw();
		}
		else
		{
			break;
		}

		++pGraphics->m_frames;
		if (pGraphics->m_FPS != 0)
		{
			int sleepTick = (int)(oneSecondStart + (DWORD64)(++FPSNow * 1000.0f / pGraphics->m_FPS));
			sleepTick -= (int)GetTickCount64();
			if (sleepTick > 0)
			{
				Sleep(sleepTick);
			}
		}
	}

	return 0;
}

UINT CGraphics::ThreadFPS(LPVOID lpParam)
{
	CGraphics* pGraphics = (CGraphics*)lpParam;

	while (true)
	{
		Sleep(1000);
		
		pGraphics->m_actualFPS = pGraphics->m_frames - pGraphics->m_lastFrames;
		pGraphics->m_lastFrames = pGraphics->m_frames;
	}

	return 0;
}
