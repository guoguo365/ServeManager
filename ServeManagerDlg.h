
// ServeManagerDlg.h : header file
//

#pragma once

// WIN32程序服務
#define SERVICE_WIN32	(SERVICE_WIN32_OWN_PROCESS | SERVICE_WIN32_SHARE_PROCESS)

// 驅動程序服務
#define SERVICE_DRIVER	(SERVICE_KERNEL_DRIVER | SERVICE_FILE_SYSTEM_DRIVER | SERVICE_RECOGNIZER_DRIVER)

#define MAX_SERVICE_SIZE (1024 * 64)

// CServeManagerDlg dialog
class CServeManagerDlg : public CDialogEx
{
// Construction
public:
	CServeManagerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVEMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void InitServeList();
	afx_msg
	void ShowServeList(DWORD dwServiceType);
	void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_serve_list;
	afx_msg void OnBnClickedRadio1();
	CButton m_win32_radio;
	CButton m_driver_radio;
	CButton m_start_btn;
	CButton m_stop_btn;
	CButton m_quit_btn;
	afx_msg void OnLvnItemchangedServeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedStopBtn();
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnBnClickedDriverRadio();
	afx_msg void OnBnClickedQuitBtn();
};
