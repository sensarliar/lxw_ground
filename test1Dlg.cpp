// testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
	m_espeed = _T("");
	m_gspeed = _T("");
	m_nspeed = _T("");
	m_speed = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Text(pDX, IDC_Espeed, m_espeed);
	DDX_Text(pDX, IDC_Gspeed, m_gspeed);
	DDX_Text(pDX, IDC_Nspeed, m_nspeed);
	DDX_Text(pDX, IDC_Sspeed, m_speed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ON_COM_RECEIVE, OnRecvComData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	InintCom(2);
	//m_com.write("11");
	//m_com.write("m_shuju=$GPZDA,032232.00,14,05,2009,00,00*67,$GPZDA");
	//OnRecvComData();
		return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
		OnRecvComData();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CTestDlg::InintCom(int port)
{
	if(!m_com.open(port))//打开串口2
	{
		MessageBox("串口打开失败", "COM2", MB_OK);
		return false;
	}
	
	else
		m_com.set_hwnd(m_hWnd);//设置窗口句柄
	return true;
}
char receive_buf[255] = {0};
//LRESULT CTestDlg::OnRecvComData(WPARAM wp, LPARAM lp)
BOOL CTestDlg::OnRecvComData(/*WPARAM wp, LPARAM lp*/)

{
	m_com.read((char *)&receive_buf, 255);
	//显示
	m_shuju=receive_buf;
	//m_shuju="$GPZDA,032232.00,14,05,2009,00,00*67,$GPZDA";
	nHeadPos=m_shuju.Find("$GPZDA,");//找帧头
	if(nHeadPos==-1)
	{
			return false;
	}
	/*else
	{
		nTailPos=m_shuju.Find("$GPZDA",nHeadPos+sizeof("$GPZDA,"));//找帧尾
		if(nTailPos==-1)
		{
			return false;
		}
	}*/
//strBuffer=m_shuju.Mid(nHeadPos,nTailPos-nHeadPos-1);//提取
	strBuffer=m_shuju.Mid(nHeadPos+sizeof("$GPZDA,"),50);//提取

	//return false;
	int k;
	int ID=0;
	//CString m_hour;
	char  m_hour[100];
	CString m_min;
	CString m_sec;
	CString m_day;
	CString m_month;
	CString m_year;
	//CString *i="$GPZDA,032232.00,14,05,2009,00,00*67";
	//len=strlen(i);
	for(k=0;k<strBuffer.GetLength();k++)
	{
		if(strBuffer.Mid(k,1).Compare(",")==0)
		{
			//str_buffer=strBuffer.Mid(k+1,nTailPos-nHeadPos-1-k);
			str_buffer=strBuffer.Mid(k,20);
				switch(ID)
				{
				case 0:
					
					hour=str_buffer.Left(2);
					//m_hour=(atoi(hour)+8)%24;
					itoa((atoi(hour)+8)%24,m_hour,10);
					SetDlgItemText(IDC_Hour,m_hour);
					m_min=str_buffer.Mid(2,2);
					SetDlgItemText(IDC_Min,m_min);
					m_sec=str_buffer.Mid(4,5);
					SetDlgItemText(IDC_Sec,m_sec);
					break;
					
				case 1:
					
					m_day=str_buffer.Left(2);
					SetDlgItemText(IDC_Day,m_day);
					//GetDlgItem(IDC_Day)->SetWindowText("m_day");
					break;
					
				case 2:
					
					m_month=str_buffer.Mid(0,2);
					SetDlgItemText(IDC_Month,m_month);
					break;
					
				case 3:
					
					m_year=str_buffer.Mid(0,4);
					SetDlgItemText(IDC_Year,m_year);
					break;
				default:
					   break;
				}
			ID++;
			
		}
		else
		{
		//str_buffer=strBuffer.Mid(k+1,k);
		
		}
		str_buffer.Empty();
		
	}
	
	return true;
}

void CTestDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	
}

void CTestDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	
}

void CTestDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	
}

void CTestDlg::OnButton5() 
{
	// TODO: Add your control notification handler code here
	
	
	/*
	nHeadPos=m_shuju.Find("$GPZDA,");//找帧头
	if(nHeadPos==-1)
	{
		return ;
	}
	else
	{
		nTailPos=m_shuju.Find("*",nHeadPos);//找帧尾
		if(nTailPos==-1)
		{
			return ;
		}
	}
	*/
	//strBuffer=m_shuju.Mid(nHeadPos,nTailPos);//提取
/*	m_shuju="$GPZDA,032232.00,14,05,2009,00,00*67";
	char m_hour[10];
	char m_min[10];
	char m_sec[10];
	char m_day[10];
	char m_month[10];
	char m_year[10];

	char temp[100];
	memset(temp,0,sizeof(temp));//memset函数使temp前sizeof(temp)长度的字符为0
	int length = m_shuju.GetLength();
	memcpy(temp,m_shuju.GetBuffer(0)+7,length-7);//将(m_shuju.GetBuffer(0)+7)的length-7的字符给temp
	char tail[10];
	memset(tail,0,sizeof(tail));
	//int year,month,day,hour,minute,second,msecond,temp2;
		int year,month,day,hour,minute,temp2;
		float  second;
	//sscanf(temp,"%2d%2d%2d.%2d,%d,%d,%d,%d,%s",&hour,&minute,&second,&msecond,&day,&month,&year,&temp2,tail);
	sscanf(temp,"%2d%2d%f,%d,%d,%d,%d,%s",&hour,&minute,&second,&day,&month,&year,&temp2,tail);
	
		
	itoa(hour+8,m_hour,10);
	itoa(minute,m_min,10);
	//itoa(second,m_sec,10);
	itoa(day,m_day,10);
	itoa(month,m_month,10);
	itoa(year,m_year,10);
	sprintf(m_sec,"%.2f",second);


	
	GetDlgItem(IDC_Hour)->SetWindowText(m_hour);
	SetDlgItemText(IDC_Min,m_min);
	SetDlgItemText(IDC_Sec,m_sec);
	SetDlgItemText(IDC_Day,m_day);
	SetDlgItemText(IDC_Month,m_month);
	SetDlgItemText(IDC_Year,m_year);*/
	

}

