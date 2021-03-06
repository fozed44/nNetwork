
// nNetworkTestRunnerDlg.h : header file
//

#pragma once


// CnNetworkTestRunnerDlg dialog
class CnNetworkTestRunnerDlg : public CDialog
{
// Construction
public:
	CnNetworkTestRunnerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NNETWORKTESTRUNNER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedStart();
};
