// testDlg.h : header file
//

#if !defined(AFX_TESTDLG_H__0F0EE599_B281_4CA3_BADF_4A55FBDDBD64__INCLUDED_)
#define AFX_TESTDLG_H__0F0EE599_B281_4CA3_BADF_4A55FBDDBD64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

class CTestDlg : public CDialog
{
// Construction
public:
	BOOL OnRecvComData();
	CString hour;
	//CString str_buffer;
	//int nTailPos;
	CString strBuffer;
	//int nMidPos;
	int nHeadPos;
	
	bool InintCom(int port);//初始化串口,参数设置串口通道
	CTestDlg(CWnd* pParent = NULL);	// standard constructor
	CString m_shuju;
	int Bytes;
//	int rx_counts;
	_thread_com m_com;//串口变量
	//CString m_shuju1;
	//int ID;

	CBitmap m_bitmap_fdj1;
	CBitmap m_bitmap_fdj2;
	CBitmap m_bitmap_dev1;
	CBitmap m_bitmap_dev2;
	CBitmap m_bitmap_dev3;
	CButton m_button_fdj1;
	CButton m_button_fdj2;
	CButton m_button_dev1;
	CButton m_button_dev2;
	CButton m_button_dev3;

	CBitmap m_bitmap_sta_dev1;
	CBitmap m_bitmap_sta_dev2;
	CBitmap m_bitmap_sta_dev3;
	CButton m_button_sta_dev1;
	CButton m_button_sta_dev2;
	CButton m_button_sta_dev3;
	CFile file;

   //int GetStringCount(CString trTemp, CString gGPS);

// Dialog Data
	//{{AFX_DATA(CTestDlg)
	enum { IDD = IDD_TEST_DIALOG };
	CButton	m_com1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSend();
	afx_msg void OnOpen();
	afx_msg void OnButton4();
	afx_msg void OnButtonDev1();
	afx_msg void OnButtonDev2();
	afx_msg void OnButtonDev3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CFont m_font;
	CBrush m_brush;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__0F0EE599_B281_4CA3_BADF_4A55FBDDBD64__INCLUDED_)
