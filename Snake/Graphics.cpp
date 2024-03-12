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
		ShowError(_T("[CGraphics::Initialize]ִ��CoInitializeʧ�ܣ�"), hr);
		return hr;
	}

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1Factoryʧ�ܣ�"), hr);
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
		ShowError(_T("[CGraphics::Initialize]����ID2D1HwndRenderTargetʧ�ܣ�"), hr);
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
		ShowError(_T("[CGraphics::Initialize]����IWICImagingFactoryʧ�ܣ�"), hr);
		return hr;
	}

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����IDWriteFactoryʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pDWriteFactory->CreateTextFormat(
		L"΢���ź�",
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
		ShowError(_T("[CGraphics::Initialize]����IDWriteTextFormatʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pDWriteFactory->CreateTextFormat(
		L"΢���ź�",
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
		ShowError(_T("[CGraphics::Initialize]����IDWriteTextFormatʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhiteBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1SolidColorBrushʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &m_pBackGroundBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1SolidColorBrushʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen), &m_pSnakeBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1SolidColorBrushʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pSnackBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1SolidColorBrushʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1SolidColorBrushʧ�ܣ�"), hr);
		return hr;
	}

	hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), &m_pAquaBrush);
	if (FAILED(hr))
	{
		ShowError(_T("[CGraphics::Initialize]����ID2D1SolidColorBrushʧ�ܣ�"), hr);
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
		s.Format(_T("%s\n������룺[HRESULT]%d\n������Ϣ��%s"), szText, hr, (LPTSTR)pvMsgBuf);
		m_pWnd->MessageBox(s, _T("����"), MB_OK | MB_ICONERROR);
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
			// ����
//			str.Format(L"��ǰ������%d\n��ʷ��߷֣�%d", (pList->GetCount() - 3) * 10, CSnakeDlg::Instance->GetHighestScore());
//			pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.top - 60, rect.left + 200, rect.top + 30), pGraphics->m_pWhiteBrush);
			// ��ͣ��ʾ
			if (!CGameHandler::Instance.IsDead())
			{
				str = CGameHandler::Instance.IsPaused() ? L"�����ⷽ�������" : L"��P����ͣ";
				pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.bottom + 30, rect.left + 200, rect.bottom + 80), pGraphics->m_pAquaBrush);
			}
			// ��Ϸ��Ϣ & ��ǰ��Ϸ����
			str.Format(L"��Ϸ��Ϣ��\n  ��ǰ������%d\n  ��ʷ��߷֣�%d\n  ��ǰTick��%.2lf\n\n��ǰ��Ϸ���ã�\n  ��ȣ�%d\n  �߶ȣ�%d\n  ��ʼTick��%d\n  ����Tick��%.2lf\n  ��СTick��%d",
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

			// ̰����
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

			// ��ʳ
			auto v = CGameHandler::Instance.GetSnackPoint();
			pGraphics->m_pRenderTarget->FillRectangle(D2D1::RectF(
				rect.left + v.x * pGraphics->m_unitSize,
				rect.top + v.y * pGraphics->m_unitSize,
				rect.left + (v.x + 1) * pGraphics->m_unitSize,
				rect.top + (v.y + 1) * pGraphics->m_unitSize
			), pGraphics->m_pSnackBrush);

			// ������Ϣ
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
					str = "�����ˣ���R�����档";
					pGraphics->m_pRenderTarget->DrawText(str, str.GetLength(), pGraphics->m_FPSTextFormat, D2D1::RectF(rect.left, rect.bottom + 10, rect.left + 200, rect.bottom + 60), pGraphics->m_pSnackBrush);
				}
			}
			else
			{
				str = "��R������";
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
