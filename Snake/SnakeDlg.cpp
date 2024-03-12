
// SnakeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Snake.h"
#include "SnakeDlg.h"
#include "afxdialogex.h"
//#include "json/json.h"
#include "cJSON.h"
#include "OptionsDlg.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib, "D:\\git\\jsoncpp-1.9.5\\build\\lib\\MinSizeRel\\jsoncpp.lib")

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSnakeDlg 对话框



CSnakeDlg::CSnakeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SNAKE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSnakeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSnakeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSnakeDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, &CSnakeDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_COMMAND(ID_HELP_ABOUT, &CSnakeDlg::OnHelpAbout)
	ON_COMMAND(ID_FPS_60, &CSnakeDlg::OnFps60)
	ON_COMMAND(ID_FPS_120, &CSnakeDlg::OnFps120)
	ON_COMMAND(ID_FPS_INF, &CSnakeDlg::OnFpsInf)
	ON_COMMAND(ID_SETTINGS_OPTIONS, &CSnakeDlg::OnOptions)
END_MESSAGE_MAP()


// CSnakeDlg 消息处理程序

BOOL CSnakeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CGraphics::Instance.Initialize(this, 60);
	Instance = this;

	LoadSettings();
	GetMenu()->GetSubMenu(1)->CheckMenuRadioItem(ID_FPS_60, ID_FPS_INF, ID_FPS_60 + m_selFPS, MF_BYCOMMAND);
	switch (m_selFPS)
	{
	case 0:
		OnFps60();
		break;
	case 1:
		OnFps120();
		break;
	case 2:
		OnFpsInf();
		break;
	default:
		break;
	}

	CGameHandler::Instance.StartGame(m_width, m_height, m_originalTick, m_accelerationTick, m_minTick);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSnakeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSnakeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSnakeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CSnakeDlg::LoadSettings()
{
	if (!PathFileExists(L"./Settings.json"))
	{
		SaveSettings();
	}
	CStdioFile file(L"./Settings.json", CStdioFile::modeRead);
	CString sJson = L"", str;
	while (file.ReadString(str))
	{
		sJson += str;
	}
	USES_CONVERSION;
	char* json = T2A(sJson);
	cJSON* root = cJSON_Parse(json);
	m_selFPS = (UINT)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "FPS"));
	m_highestScore = (UINT)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "HighestScore"));
	m_width = (UINT)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "Width"));
	m_height = (UINT)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "Height"));
	m_originalTick = (UINT)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "OriginalTick"));
	m_accelerationTick = cJSON_GetNumberValue(cJSON_GetObjectItem(root, "AccelerationTick"));
	m_minTick = (UINT)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "MinTick"));
	file.Close();
}

VOID CSnakeDlg::SaveSettings()
{
	CStdioFile file;
	file.Open(L"./Settings.json", CStdioFile::modeCreate | CStdioFile::modeWrite);
	cJSON* root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "FPS", (double)m_selFPS);
	cJSON_AddNumberToObject(root, "HighestScore", (double)m_highestScore);
	cJSON_AddNumberToObject(root, "Width", (double)m_width);
	cJSON_AddNumberToObject(root, "Height", (double)m_height);
	cJSON_AddNumberToObject(root, "OriginalTick", (double)m_originalTick);
	cJSON_AddNumberToObject(root, "AccelerationTick", m_accelerationTick);
	cJSON_AddNumberToObject(root, "MinTick", (double)m_minTick);
	file.WriteString(CString(cJSON_Print(root)));
	file.Close();
}

CSnakeDlg* CSnakeDlg::Instance;

void CSnakeDlg::OnBnClickedOk()
{
//	CDialogEx::OnOK();
}


BOOL CSnakeDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		else if (pMsg->wParam == 'r' || pMsg->wParam == 'R')
		{
			CGameHandler::Instance.StartGame(m_width, m_height, m_originalTick, m_accelerationTick, m_minTick);
			return TRUE;
		}
		else if (pMsg->wParam == 'p' || pMsg->wParam == 'P')
		{
			if (!CGameHandler::Instance.IsPaused())
			{
				CGameHandler::Instance.Pause();
			}
		}
		else
		{
			CGameHandler::Instance.OnKey(pMsg->wParam);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSnakeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}


void CSnakeDlg::OnBnClickedCancel()
{
	CGraphics::Instance.Exit();
	CDialogEx::OnCancel();
}


void CSnakeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CGraphics::Instance.FreshSize(cx, cy);
}


void CSnakeDlg::OnHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CSnakeDlg::OnFps60()
{
	GetMenu()->GetSubMenu(1)->CheckMenuRadioItem(ID_FPS_60, ID_FPS_INF, ID_FPS_60, MF_BYCOMMAND);
	CGraphics::Instance.SetFPS(60);
	m_selFPS = 0;
	SaveSettings();
}


void CSnakeDlg::OnFps120()
{
	GetMenu()->GetSubMenu(1)->CheckMenuRadioItem(ID_FPS_60, ID_FPS_INF, ID_FPS_120, MF_BYCOMMAND);
	CGraphics::Instance.SetFPS(120);
	m_selFPS = 1;
	SaveSettings();
}


void CSnakeDlg::OnFpsInf()
{
	GetMenu()->GetSubMenu(1)->CheckMenuRadioItem(ID_FPS_60, ID_FPS_INF, ID_FPS_INF, MF_BYCOMMAND);
	CGraphics::Instance.SetFPS(0);
	m_selFPS = 2;
	SaveSettings();
}

VOID CSnakeDlg::FreshScore(UINT score)
{
	if (score > m_highestScore)
	{
		m_highestScore = score;
		SaveSettings();
	}
}

UINT CSnakeDlg::GetHighestScore()
{
	return m_highestScore;
}


void CSnakeDlg::OnOptions()
{
	if (!CGameHandler::Instance.IsPaused())
	{
		CGameHandler::Instance.Pause();
	}

	COptionsDlg dlg;
	dlg.m_width = m_width;
	dlg.m_height = m_height;
	dlg.m_originalTick = m_originalTick;
	dlg.m_accelerationTick = m_accelerationTick;
	dlg.m_minTick = m_minTick;

	if (IDOK == dlg.DoModal())
	{
		m_width = dlg.m_width;
		m_height = dlg.m_height;
		m_originalTick = dlg.m_originalTick;
		m_accelerationTick = dlg.m_accelerationTick;
		m_minTick = dlg.m_minTick;

		SaveSettings();
	}
}
