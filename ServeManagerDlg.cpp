
// ServeManagerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ServeManager.h"
#include "ServeManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CServeManagerDlg dialog



CServeManagerDlg::CServeManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVEMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServeManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ID_SERVE_LIST, m_serve_list);
	DDX_Control(pDX, ID_WIN32_RADIO, m_win32_radio);
	DDX_Control(pDX, ID_DRIVER_RADIO, m_driver_radio);
	DDX_Control(pDX, ID_START_BTN, m_start_btn);
	DDX_Control(pDX, ID_STOP_BTN, m_stop_btn);
	DDX_Control(pDX, ID_QUIT_BTN, m_quit_btn);
}

BEGIN_MESSAGE_MAP(CServeManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_WIN32_RADIO, &CServeManagerDlg::OnBnClickedRadio1)
	ON_NOTIFY(LVN_ITEMCHANGED, ID_SERVE_LIST, &CServeManagerDlg::OnLvnItemchangedServeList)
	ON_BN_CLICKED(ID_STOP_BTN, &CServeManagerDlg::OnBnClickedStopBtn)
	ON_BN_CLICKED(ID_START_BTN, &CServeManagerDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(ID_DRIVER_RADIO, &CServeManagerDlg::OnBnClickedDriverRadio)
	ON_BN_CLICKED(ID_QUIT_BTN, &CServeManagerDlg::OnBnClickedQuitBtn)
END_MESSAGE_MAP()


// CServeManagerDlg message handlers

BOOL CServeManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// 列表初始化
	InitServeList();

	// radio初始化
	CButton* radio = (CButton*) GetDlgItem(ID_WIN32_RADIO);
	radio->SetCheck(1);

	// 顯示服務
	ShowServeList(SERVICE_WIN32);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// 初始化服務列表
void CServeManagerDlg::InitServeList() {

	// 设置样式
	m_serve_list.SetExtendedStyle(
		m_serve_list.GetExtendedStyle()
		| LVS_EX_GRIDLINES					// 有网格
		| LVS_EX_FULLROWSELECT				// 选择单行
	);

	// 设置表头
	m_serve_list.InsertColumn(0, "序号");
	m_serve_list.InsertColumn(1, "服務名");
	m_serve_list.InsertColumn(2, "顯示名");
	m_serve_list.InsertColumn(3, "狀態");

	m_serve_list.SetColumnWidth(0, 50);
	m_serve_list.SetColumnWidth(1, 200);
	m_serve_list.SetColumnWidth(2, 450);
	m_serve_list.SetColumnWidth(3, 100);

}

// 顯示服務列表
void CServeManagerDlg::ShowServeList(DWORD dwServiceType) {
	/*
	dwServiceType: 服務類型，可以是一個或多個
	SERVICE_KERNEL_DRIVER和SERVICE_FILE_SYSTEM_DRIVER	驅動程序
	SERVICE_FILE_SYSTEM_DRIVER							文件系統驅動服務
	SERVICE_KERNEL_DRIVER								司機服務
	SERVICE_WIN32										WIN32服務
	SERVICE_WIN32_OWN_PROCESS							在自己的進程中運行的程序
	SERVICE_WIN32_SHARE_PROCESS							一個或多個其他服務共享進程服務
	*/

	// 清空列表
	m_serve_list.DeleteAllItems();

	// 服務句柄
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM) {
		AfxMessageBox("OpenSCManager Error");
		return;
	}

	DWORD dwByteNeeded, dwServicesReturned, lpResumeHandle = 0;
	LPENUM_SERVICE_STATUS serviceStatus = (LPENUM_SERVICE_STATUS) LocalAlloc(LPTR, MAX_SERVICE_SIZE);

	// 枚举指定服务控制管理器数据库中的服务。提供了每项服务的名称和状态。
	BOOL bRet = EnumServicesStatus(
		hSCM,					// 服務管理器句柄
		dwServiceType,			// 服務類型
		SERVICE_STATE_ALL,		// 要枚舉的服務狀態，這裏是全部狀態
		serviceStatus,			// NULL, 要確定所需的大小
		MAX_SERVICE_SIZE,				// 指向緩衝區的大小
		&dwByteNeeded,			// out，如果緩衝區太小，則指向一個變量指針，接收返回剩餘服務條目所需的字節數
		&dwServicesReturned,	// out, 指向接收返回的服務條目數的變量指針
		&lpResumeHandle			// int, out 指向變量的指針，該變量在輸入時指定枚舉的起點，第一次調用必須設爲0，輸出時，如果成功則此值為0，
		);
	if (FALSE == bRet) {
		AfxMessageBox("EnumServiceStatus Error");
		return;
	}

	// 遍歷服務，顯示服務
	for (DWORD i = 0; i < dwServicesReturned; i++) {
		CString str;
		str.Format("%d", i);
		m_serve_list.InsertItem(i, str);
		m_serve_list.SetItemText(i, 1, serviceStatus[i].lpServiceName);
		m_serve_list.SetItemText(i, 2, serviceStatus[i].lpDisplayName);

		switch (serviceStatus[i].ServiceStatus.dwCurrentState) {
		case SERVICE_PAUSED:
		{
			m_serve_list.SetItemText(i, 3, "暫停");
			break;
		}
		case SERVICE_STOPPED:
		{
			m_serve_list.SetItemText(i, 3, "停止");
			break;
		}
		case SERVICE_RUNNING:
		{
			m_serve_list.SetItemText(i, 3, "運行");
			break;
		}
		default:
		{
			m_serve_list.SetItemText(i, 3, "其他");
			break;
		}
		}

	}

	// 關閉服務句柄
	CloseServiceHandle(hSCM);

}

void CServeManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServeManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServeManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServeManagerDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	ShowServeList(SERVICE_WIN32);
}


void CServeManagerDlg::OnLvnItemchangedServeList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

// 服務停止
void CServeManagerDlg::OnBnClickedStopBtn()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_serve_list.GetFirstSelectedItemPosition();
	int nSel = -1;
	while (pos) {
		nSel = m_serve_list.GetNextSelectedItem(pos);
	}
	if (-1 == nSel) {
		AfxMessageBox("請選擇要啓動的服務");
		return;
	}

	// 獲取選中服務的服務名
	char szServiceName[MAXBYTE] = { 0 };
	m_serve_list.GetItemText(nSel, 1, szServiceName, MAXBYTE);

	// 獲取服務管理器句柄
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == hSCM) {
		AfxMessageBox("OpenSCManager Error");
		return;
	}

	// 獲取指定服務句柄
	SC_HANDLE hSCService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
	SERVICE_STATUS serviceStatus;

	// 停止服務
	BOOL bRet = ControlService(hSCService, SERVICE_CONTROL_STOP, &serviceStatus);
	if (FALSE == bRet) {
		AfxMessageBox("停止服務失敗");
		return;
	}
	m_serve_list.SetItemText(nSel, 3, "停止");

	CloseServiceHandle(hSCService);
	CloseServiceHandle(hSCM);
}

// 啓動服務
void CServeManagerDlg::OnBnClickedStartBtn()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_serve_list.GetFirstSelectedItemPosition();
	int nSel = -1;
	while (pos) {
		nSel = m_serve_list.GetNextSelectedItem(pos);
	}
	if (-1 == nSel) {
		AfxMessageBox("請選擇要啓動的服務");
		return;
	}

	// 獲取選中服務的服務名
	char szServiceName[MAXBYTE] = { 0 };
	m_serve_list.GetItemText(nSel, 1, szServiceName, MAXBYTE);

	// 獲取服務管理器句柄
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == hSCM) {
		AfxMessageBox("OpenSCManager Error");
		return;
	}

	// 獲取指定服務句柄
	SC_HANDLE hSCService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);

	// 啓動服務
	BOOL bRet = StartService(hSCService, 0, NULL);
	if (FALSE == bRet) {
		AfxMessageBox("啓動服務失敗");
		return;
	}
	m_serve_list.SetItemText(nSel, 3, "運行");

	CloseServiceHandle(hSCService);
	CloseServiceHandle(hSCM);

}


void CServeManagerDlg::OnBnClickedDriverRadio()
{
	// TODO: Add your control notification handler code here
	ShowServeList(SERVICE_DRIVER);
}


void CServeManagerDlg::OnBnClickedQuitBtn()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}
