#pragma once
#include "afxdialogex.h"


// COptionsDlg 对话框

class COptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptionsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlWidth;
	CEdit m_ctrlHeight;
	CEdit m_ctrlOriginalTick;
	CEdit m_ctrlAccelerationTick;
	CEdit m_ctrlMinTick;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	UINT m_width, m_height, m_originalTick, m_minTick;
	DOUBLE m_accelerationTick;
};
