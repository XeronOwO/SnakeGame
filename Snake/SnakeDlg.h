
// SnakeDlg.h: 头文件
//

#pragma once

#include "Graphics.h"
#include "GameHandler.h"

// CSnakeDlg 对话框
class CSnakeDlg : public CDialogEx
{
// 构造
public:
	CSnakeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SNAKE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	UINT m_selFPS = 0, m_highestScore = 0;
	UINT m_width = 20, m_height = 20, m_originalTick = 100, m_minTick = 50;
	DOUBLE m_accelerationTick = 1;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	VOID LoadSettings();
	VOID SaveSettings();
	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpAbout();
	afx_msg void OnFps60();
	afx_msg void OnFps120();
	afx_msg void OnFpsInf();
	VOID FreshScore(UINT score);
	UINT GetHighestScore();
	static CSnakeDlg* Instance;
	afx_msg void OnOptions();
};
