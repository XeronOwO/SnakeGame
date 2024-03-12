#include "pch.h"
#include "GameHandler.h"
#include "SnakeDlg.h"

CGameHandler CGameHandler::Instance;

BOOL CGameHandler::IsGameStarted()
{
	return m_isStarted;
}

VOID CGameHandler::StartGame(UINT width, UINT height, UINT originalTick, DOUBLE accelerationTick, UINT minTick)
{
	m_width = width;
	m_height = height;
	m_originalTick = originalTick;
	m_accelerationTick = accelerationTick;
	m_minTick = minTick;
	m_snakePointsList.RemoveAll();
	m_snakePointsList.AddHead(D2D1::Point2U(width / 2, height / 2));
	m_snakePointsList.AddHead(D2D1::Point2U(width / 2, height / 2));
	m_snakePointsList.AddHead(D2D1::Point2U(width / 2, height / 2));
	m_death = FALSE;
	if (m_pExitThread)
	{
		*m_pExitThread = TRUE;
	}
	m_direction = 0;
	m_score = 0;

	m_pExitThread = new BOOL;
	*m_pExitThread = FALSE;
	m_paused = FALSE;
	m_threadGame = AfxBeginThread(GameThread, m_pExitThread);
}

VOID CGameHandler::OnKey(WPARAM key)
{
	BOOL f = FALSE;
	switch (key)
	{
	case 'w':
	case 'W':
	case VK_UP:
		f = TRUE;
		if (m_direction != 2)
		{
			m_direction = 0;
		}
		break;
	case 'a':
	case 'A':
	case VK_LEFT:
		f = TRUE;
		if (m_direction != 3)
		{
			m_direction = 1;
		}
		break;
	case 's':
	case 'S':
	case VK_DOWN:
		f = TRUE;
		if (m_direction != 0)
		{
			m_direction = 2;
		}
		break;
	case 'd':
	case 'D':
	case VK_RIGHT:
		f = TRUE;
		if (m_direction != 1)
		{
			m_direction = 3;
		}
		break;
	default:
		break;
	}
	if (f)
	{
		if (!m_death)
		{
			if (m_paused)
			{
				Resume();
			}
		}
	}
}

CList<D2D1_POINT_2U>* CGameHandler::GetSnakePointsList()
{
	return &m_snakePointsList;
}

UINT CGameHandler::GetSizeX()
{
	return m_width;
}

UINT CGameHandler::GetSizeY()
{
	return m_height;
}

D2D1_POINT_2U CGameHandler::GetNewSnackPoint()
{
	D2D1_POINT_2U res;
	BOOL f = FALSE;
	srand(GetTickCount());
	while (!f)
	{
		res.x = rand() % m_width;
		res.y = rand() % m_height;
		f = TRUE;
		auto pos = CGameHandler::Instance.m_snakePointsList.GetHeadPosition();
		while (pos)
		{
			auto v = CGameHandler::Instance.m_snakePointsList.GetNext(pos);
			if (v.x == res.x && v.y == res.y)
			{
				f = FALSE;
				break;
			}
		}
	}
	return res;
}

D2D1_POINT_2U CGameHandler::GetSnackPoint()
{
	return m_snackPoint;
}

VOID CGameHandler::Exit()
{
	m_exit = TRUE;
	if (m_threadGame)
	{
		m_threadGame->ExitInstance();
	}
}

VOID CGameHandler::StopGame()
{
	return VOID();
}

BOOL CGameHandler::IsDead()
{
	return m_death;
}

UINT CGameHandler::GameThread(LPVOID lpParam)
{
	BOOL* pExitThread = (BOOL*)lpParam;
	CGameHandler::Instance.m_snackPoint = CGameHandler::Instance.GetNewSnackPoint();

	CGameHandler::Instance.m_currentTick = CGameHandler::Instance.m_originalTick;
	while (true)
	{
		Sleep((DWORD)CGameHandler::Instance.m_currentTick);
		if (*pExitThread)
		{
			break;
		}
		if (CGameHandler::Instance.m_exit)
		{
			break;
		}

		auto headPoint = CGameHandler::Instance.m_snakePointsList.GetHead();
		switch (CGameHandler::Instance.m_direction)
		{
		case 0:
			CGameHandler::Instance.m_snakePointsList.AddHead(D2D1::Point2U(headPoint.x, (headPoint.y + CGameHandler::Instance.m_height - 1) % CGameHandler::Instance.m_height));
			break;
		case 1:
			CGameHandler::Instance.m_snakePointsList.AddHead(D2D1::Point2U((headPoint.x + CGameHandler::Instance.m_width - 1) % CGameHandler::Instance.m_width, headPoint.y));
			break;
		case 2:
			CGameHandler::Instance.m_snakePointsList.AddHead(D2D1::Point2U(headPoint.x, (headPoint.y + CGameHandler::Instance.m_height + 1) % CGameHandler::Instance.m_height));
			break;
		case 3:
			CGameHandler::Instance.m_snakePointsList.AddHead(D2D1::Point2U((headPoint.x + CGameHandler::Instance.m_width + 1) % CGameHandler::Instance.m_width, headPoint.y));
			break;
		default:
			break;
		}
		auto pos = CGameHandler::Instance.m_snakePointsList.GetHeadPosition();
		headPoint = CGameHandler::Instance.m_snakePointsList.GetNext(pos);
		BOOL death = FALSE;
		while (pos)
		{
			auto v = CGameHandler::Instance.m_snakePointsList.GetNext(pos);
			if (v.x == headPoint.x && v.y == headPoint.y)
			{
				death = TRUE;
				break;
			}
		}
		if (death)
		{
			CGameHandler::Instance.m_death = TRUE;
			break;
		}
		if (headPoint.x == CGameHandler::Instance.m_snackPoint.x && headPoint.y == CGameHandler::Instance.m_snackPoint.y)
		{
			CGameHandler::Instance.m_snackPoint = CGameHandler::Instance.GetNewSnackPoint();
			CGameHandler::Instance.m_currentTick -= CGameHandler::Instance.m_accelerationTick;
			if (CGameHandler::Instance.m_currentTick < CGameHandler::Instance.m_minTick)
			{
				CGameHandler::Instance.m_currentTick = CGameHandler::Instance.m_minTick;
			}
			CGameHandler::Instance.m_score += 10;
			CSnakeDlg::Instance->FreshScore(CGameHandler::Instance.m_score);
		}
		else
		{
			CGameHandler::Instance.m_snakePointsList.RemoveTail();
		}
	}

	return 0;
}

UINT CGameHandler::GetWidth()
{
	return m_width;
}

UINT CGameHandler::GetHeight()
{
	return m_height;
}

UINT CGameHandler::GetOriginalTick()
{
	return m_originalTick;
}

DOUBLE CGameHandler::GetAccelerationTick()
{
	return m_accelerationTick;
}

UINT CGameHandler::GetCurrentScore()
{
	return m_score;
}

VOID CGameHandler::Pause()
{
	if (m_threadGame->m_hThread)
	{
		m_threadGame->SuspendThread();
	}
	m_paused = TRUE;
}

BOOL CGameHandler::IsPaused()
{
	return m_paused;
}


VOID CGameHandler::Resume()
{
	if (m_threadGame->m_hThread)
	{
		m_threadGame->ResumeThread();
	}
	m_paused = FALSE;
}

UINT CGameHandler::GetMinTick()
{
	return m_minTick;
}

DOUBLE CGameHandler::GetCurrentTick()
{
	return m_currentTick;
}
