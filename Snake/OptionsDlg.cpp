// OptionsDlg.cpp: 实现文件
//

#include "pch.h"
#include "Snake.h"
#include "afxdialogex.h"
#include "OptionsDlg.h"
#include "SnakeDlg.h"


// COptionsDlg 对话框

IMPLEMENT_DYNAMIC(COptionsDlg, CDialogEx)

COptionsDlg::COptionsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTIONS_DIALOG, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WIDTH, m_ctrlWidth);
	DDX_Control(pDX, IDC_HEIGHT, m_ctrlHeight);
	DDX_Control(pDX, IDC_ORIGINAL_TICK, m_ctrlOriginalTick);
	DDX_Control(pDX, IDC_ACCELERATION_TICK, m_ctrlAccelerationTick);
	DDX_Control(pDX, IDC_MIN_TICK, m_ctrlMinTick);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString str;
	str.Format(L"%d", m_width);
	m_ctrlWidth.SetWindowText(str);
	str.Format(L"%d", m_height);
	m_ctrlHeight.SetWindowText(str);
	str.Format(L"%d", m_originalTick);
	m_ctrlOriginalTick.SetWindowText(str);
	str.Format(L"%lf", m_accelerationTick);
	m_ctrlAccelerationTick.SetWindowText(str);
	str.Format(L"%d", m_minTick);
	m_ctrlMinTick.SetWindowText(str);

	SetIcon(CSnakeDlg::Instance->m_hIcon, FALSE);		// 设置小图标

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void COptionsDlg::OnOK()
{
	CString str;
	m_ctrlWidth.GetWindowText(str);
	m_width = _wtoi(str);
	m_ctrlHeight.GetWindowText(str);
	m_height = _wtoi(str);
	m_ctrlOriginalTick.GetWindowText(str);
	m_originalTick = _wtoi(str);
	m_ctrlAccelerationTick.GetWindowText(str);
	m_accelerationTick = _wtof(str);
	m_ctrlMinTick.GetWindowText(str);
	m_minTick = _wtoi(str);

	CDialogEx::OnOK();
}
