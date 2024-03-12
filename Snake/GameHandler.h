#pragma once
class CGameHandler
{
private:
	BOOL m_isStarted = FALSE;
	CList<D2D1_POINT_2U> m_snakePointsList;
	UINT m_width, m_height, m_direction = 0, m_score = 0;
	static UINT GameThread(LPVOID lpParam);
	CWinThread* m_threadGame;
	D2D1_POINT_2U m_snackPoint;
	BOOL m_exit = FALSE;
	BOOL m_death = TRUE;
	BOOL m_restart = FALSE;
	BOOL* m_pExitThread;
	UINT m_originalTick, m_minTick;
	DOUBLE m_accelerationTick, m_currentTick;
	BOOL m_paused;

public:
	static CGameHandler Instance;
	BOOL IsGameStarted();
	VOID StartGame(UINT width, UINT height, UINT originalTick, DOUBLE accelerationTick, UINT minTick);
	VOID OnKey(WPARAM key);
	CList<D2D1_POINT_2U>* GetSnakePointsList();
	UINT GetSizeX();
	UINT GetSizeY();
	D2D1_POINT_2U GetNewSnackPoint();
	D2D1_POINT_2U GetSnackPoint();
	VOID Exit();
	VOID StopGame();
	BOOL IsDead();
	UINT GetWidth();
	UINT GetHeight();
	UINT GetOriginalTick();
	DOUBLE GetAccelerationTick();
	DOUBLE GetCurrentTick();
	UINT GetMinTick();
	UINT GetCurrentScore();
	VOID Pause();
	VOID Resume();
	BOOL IsPaused();
};

