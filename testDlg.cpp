// testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
//#include <math.h>
#include "dbw_rd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////


//#include <inttypes.h>
#include <string.h>
//#include <math.h>
//#include <stdlib.h>


#define FLAG_FDJ_BIT 0
#define FLAG_WQ_BIT 1
#define FLAG_QLJ_BIT 2
#define FLAG_JY_BIT 5


struct DbwInfo dbw_info;
char dbw_info_flag = 0x39;
		//now addr_   uint8_t INFO_ADDR[4]={0x04,0xE6,0x10};//291822  addr 3B
//char INFO_ADDR[4] = {0x04,0x73,0xEE,0x0};
char INFO_ADDR[4] = {0x02,0xF5,0x4C,0x0};
char RCV_CMD[255];
int cmd_rcv_flag;
int cmd_ch_num;

int rx_counts;
int timer_cnt_60ms = 0;




void parse_dbw_info_GM002(void);


void dbw_info_impl_init( void ) {
  dbw_info.msg_available = FALSE;
//  dbw_info.pos_available = FALSE;
//  dbw_info.gps_nb_ovrn = 0;
  dbw_info.msg_len = 0;
	dbw_info.set_len = DBW_MAXLEN-1;
	dbw_info.set_temp_len = DBW_MAXLEN-1;
}



/**
 * parse $TXXX MESSAGE stored in
 * dbw_info.msg_buf .
 */
void parse_dbw_info_GM002(void) {
  int i = 7;     // current position in the message, start after: TXXX,
	int j = 0;
	int temp_len = 255;


		//now addr_   uint8_t INFO_ADDR[4];//291822  addr 3B
	if(dbw_info.msg_buf[i] != INFO_ADDR[0])
	{
	return;
	}
	i++;
		if(dbw_info.msg_buf[i] != INFO_ADDR[1])
	{
	return;
	}
		i++;
		if(dbw_info.msg_buf[i] != INFO_ADDR[2])
	{
	return;
	}
	
///crc error	
	
	if(dbw_info.msg_buf[dbw_info.msg_len-2] != 0)
	{

    return;
	}
//	i+=6;
	i=16;
	temp_len = ((dbw_info.msg_buf[i] << 8) + dbw_info.msg_buf[i+1])/8;
	if(temp_len > dbw_info.msg_len-1-20 +1)
	{
		return;
	}
	//form 18 0xA4;
	for(j=0;j<(dbw_info.msg_len-2-19);j++)
	{
		RCV_CMD[j] = dbw_info.msg_buf[19+j];
	}
	RCV_CMD[j]='\0';
	cmd_ch_num=j;
	cmd_rcv_flag = 1;


	

//END parse GGA
}


/**
 * parse_nmea_char() has a complete line.
 * Find out what type of message it is and
 * hand it to the parser for that type.
 * received message format: $TXXX^.s�`.�55256.003413.508880N10852.685227E372.91303150.121,0007,C,9.4,5.25,27.55,�$
 * hex format: 24 54 58 58 58 00 5E 04 73 EE 60 04 E6 10 00 00 02 50 A4 30 36 30 30 30 33 2E 30 30 33 34 31 33 
 * 2E 34 39 39 34 34 30 4E 31 30 38 35 32 2E 36 39 31 33 39 34 45 33 39 35 2E 31 31 33 30 33 31 35 30 2E 39 36 
 * 37 2C 30 30 30 37 2C 43 2C 39 2E 34 2C 35 2E 32 35 2C 32 37 2E 35 35 2C 00 C7 
 */
void dbw_info_parse_msg( void ) {

//  if(dbw_info.msg_len > 5 && !strncmp(dbw_info.msg_buf , "GMING", 5)) {
	if(dbw_info.msg_len > 5 && !strncmp(dbw_info.msg_buf , "$TXXX", 5)) {
    dbw_info.msg_buf[dbw_info.msg_len] = 0;

		parse_dbw_info_GM002();
  //  parse_nmea_GPRMC();
  }
  else {
    
        dbw_info.msg_buf[dbw_info.msg_len] = 0;

      }

  // reset message-buffer
  dbw_info.msg_len = 0;
	dbw_info.set_len = DBW_MAXLEN-1;
	dbw_info.set_temp_len = DBW_MAXLEN-1;
}
/**
 * This is the actual parser.
 * It reads one character at a time
 * setting dbw_info.msg_available to TRUE
 * after a full line.
 */
void dbw_info_parse_char( char c ) {
  //reject empty lines


  // fill the buffer, unless it's full
  if (dbw_info.msg_len < DBW_MAXLEN - 1) {

    // messages end with a linefeed
    //AD: TRUNK:       if (c == '\r' || c == '\n')
    if (dbw_info.msg_len < dbw_info.set_len-1) {
			dbw_info.msg_buf[dbw_info.msg_len] = c;
      dbw_info.msg_len ++;
    } else {
			dbw_info.msg_buf[dbw_info.msg_len] = c;
      dbw_info.msg_len ++;
			dbw_info.msg_available = TRUE;
    }
		if(dbw_info.msg_len==6)
		{
			dbw_info.set_temp_len = c;
		}
		if(dbw_info.msg_len==7)
		{
			dbw_info.set_temp_len = (dbw_info.set_temp_len << 8) + c;
			if((dbw_info.set_temp_len>=11)&&(dbw_info.set_temp_len<=230))
			{
				dbw_info.set_len = dbw_info.set_temp_len;
			}
		}
		if ( c == '$')
		{
      dbw_info.msg_buf[0] = c;
      dbw_info.msg_len =1;   
			dbw_info.set_len = DBW_MAXLEN-1;
			dbw_info.set_temp_len = DBW_MAXLEN-1;
		}

  }

  if (dbw_info.msg_len >= DBW_MAXLEN - 1)
  {
	   dbw_info.msg_available = TRUE;
  }
}




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
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_brush.CreateSolidBrush(RGB(255,255,255));
	m_brush.CreateSolidBrush(TRANSPARENT);
	
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_BUTTON1, m_com1);
	DDX_Control(pDX, IDC_BUTTON_FDJ1, m_button_fdj1);
	DDX_Control(pDX, IDC_BUTTON_FDJ2, m_button_fdj2);
	DDX_Control(pDX, IDC_BUTTON_DEV1, m_button_dev1);
	DDX_Control(pDX, IDC_BUTTON_DEV2, m_button_dev2);
	DDX_Control(pDX, IDC_BUTTON_DEV3, m_button_dev3);
	DDX_Control(pDX, IDC_BUTTON_STA_DEV1, m_button_sta_dev1);
	DDX_Control(pDX, IDC_BUTTON_STA_DEV2, m_button_sta_dev2);
	DDX_Control(pDX, IDC_BUTTON_STA_DEV3, m_button_sta_dev3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, OnSend)
	ON_BN_CLICKED(IDC_BUTTON1, OnOpen)
	ON_BN_CLICKED(IDC_BUTTON_DEV1, OnButtonDev1)
	ON_BN_CLICKED(IDC_BUTTON_DEV2, OnButtonDev2)
	ON_BN_CLICKED(IDC_BUTTON_DEV3, OnButtonDev3)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ON_COM_RECEIVE, OnRecvComData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers
//CFile file;



BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CEdit*pEdt=(CEdit*)GetDlgItem(IDC_DJS);
	m_font.CreatePointFont(400,"����_GB2312");
	pEdt->SetFont(&m_font);

	// Add "About..." menu item to system menu.
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM0");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM1");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM2");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM3");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM4");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM5");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM6");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM7");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM8");
	((CComboBox*)GetDlgItem(IDC_COMBO5))->AddString("COM9");
	
	

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

	Bytes = 0;
	rx_counts=0;
	// TODO: Add extra initialization here
	//InintCom(2);
	//m_com.write("TXXXX");
	//m_com.write("m_shuju=$GPZDA,032232.00,14,05,2009,00,00*67,$GPZDA");
	//OnRecvComData();
//	rx_counts=0;
	
	m_bitmap_fdj1.LoadBitmap(IDB_BITMAP_GREEN);
//	GetDlgItem(IDC_BUTTON_SEND)->SetBitmap(m_bitmap);
	m_button_fdj1.SetBitmap(m_bitmap_fdj1);


	m_bitmap_fdj2.LoadBitmap(IDB_BITMAP_GREEN);
//	GetDlgItem(IDC_BUTTON_SEND)->SetBitmap(m_bitmap);
	m_button_fdj2.SetBitmap(m_bitmap_fdj2);

	m_bitmap_dev1.LoadBitmap(IDB_BITMAP_CLOSE);
	m_button_dev1.SetBitmap(m_bitmap_dev1);

	m_bitmap_dev2.LoadBitmap(IDB_BITMAP_CLOSE);
	m_button_dev2.SetBitmap(m_bitmap_dev2);

	m_bitmap_dev3.LoadBitmap(IDB_BITMAP_CLOSE);
	m_button_dev3.SetBitmap(m_bitmap_dev3);

	m_bitmap_sta_dev1.LoadBitmap(IDB_BITMAP_GRAY);
	m_button_sta_dev1.SetBitmap(m_bitmap_sta_dev1);

	m_bitmap_sta_dev2.LoadBitmap(IDB_BITMAP_GRAY);
	m_button_sta_dev2.SetBitmap(m_bitmap_sta_dev2);

	m_bitmap_sta_dev3.LoadBitmap(IDB_BITMAP_GRAY);
	m_button_sta_dev3.SetBitmap(m_bitmap_sta_dev3);

	if(!file.Open("GroundRecord001.txt",CFile::modeCreate | CFile::modeWrite))
	{
		MessageBox("�ļ���ʧ��", "GroundRecord001.txt", MB_OK);
		return false;
	}

	
	SetDlgItemText(IDC_FILENAME,file.GetFileName());
//	SetDlgCheckBox;
	((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);

	


	dbw_info_impl_init();


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
		/*CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect,RGB(30,144,255));*/
		
		CDialog::OnPaint();
	}
		//OnRecvComData();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CTestDlg::InintCom(int port)
{
	
	if(!m_com.open(port))//�򿪴���2
	{
		MessageBox("���ڴ�ʧ��", "COM15", MB_OK);
		return false;
	}
	
	else
	{
		m_com.set_hwnd(m_hWnd);//���ô��ھ��
		((CComboBox*)GetDlgItem(IDC_COMBO5))->EnableWindow(FALSE);
	}

	return true;
}
char receive_buf[1024] = {0};
char buf_temp[512];

CString Rx_win_text;
BOOL CTestDlg::OnRecvComData()
{
	int i;
	int bytes_temp = 1;
	bytes_temp = m_com.read((char *)&receive_buf, 1024);

	file.Write(receive_buf,bytes_temp);
//	Bytes += bytes_temp;
	if(bytes_temp <= 0)
	{
		return false;
	}
	//SetDlgItemText(IDC_DEBUG,receive_buf);
	
	i = 0;
	while(i<bytes_temp )
	{
		if(!receive_buf[i])
		{
			buf_temp[i]=' ';
		}else
		{
			buf_temp[i]=receive_buf[i];
		}
		i++;
	}
	buf_temp[i] = '\0';

	Rx_win_text += buf_temp;

	if(Rx_win_text.GetLength()>2048)
	{
		Rx_win_text="  ";
	}
//	GetDlgItem(IDC_DEBUG)->SetWindowText(Rx_win_text);

	i=0;
	bytes_temp++;
	while((bytes_temp--)&&!dbw_info.msg_available)
	{

		if(bytes_temp > 0)
		{
			dbw_info_parse_char(receive_buf[i++]);
		}
		
	}

                                                
    if (dbw_info.msg_available) {                     
      dbw_info_parse_msg();                                 
      dbw_info.msg_available = FALSE;                   
    }   




	//��ʾ
	if(cmd_rcv_flag)
	{
		strBuffer=RCV_CMD;

		char  m_hour[100];
		CString m_min;
		CString m_sec;
		CString m_day;
		CString m_month;
		CString m_year;
		CString m_speed;
		CString m_jihao;
		CString m_fadongji;
		char fadongji;
		char fadongji1;
		char fadongji2;

		char sta_dev1;
		char sta_dev2;
		char sta_dev3;

		CString fadongji1_str;
		CString fadongji2_str;

		CString m_weidu;
		CString m_weiduf;
		CString m_wei;
		CString m_jingdu;
		CString m_jingduf;
		CString m_jing;
		CString m_haiba;
		CString m_roll;
		CString m_pitch;
		CString m_yaw;


		//char ch0;

		int index_start =0;


		//ʱ����Ϣ
		hour=strBuffer.Mid(index_start,2);
		itoa((atoi(hour)+8)%24,m_hour,10);
		SetDlgItemText(IDC_Hour,m_hour);

		m_min=strBuffer.Mid(index_start+2,2);
		SetDlgItemText(IDC_Min,m_min);

		m_sec=strBuffer.Mid(index_start+4,2);
		SetDlgItemText(IDC_Sec,m_sec);

		
		//λ����Ϣ

		m_weidu=strBuffer.Mid(index_start+9,2);
		m_weiduf=strBuffer.Mid(index_start+11,7);
		SetDlgItemText(IDC_Weidu,m_weidu);
		SetDlgItemText(IDC_Weiduf,m_weiduf);
	//#if 0	
		m_wei=strBuffer.Mid(index_start+20,1);
/*
		if(m_wei.GetAt(0)=='N')
			m_wei="N";
		else m_wei="W";//��λ��Ϣ��ȡ
		*/
		SetDlgItemText(IDC_Wei,m_wei);
					
		
		m_jingdu=strBuffer.Mid(index_start+21,3);//������Ϣ��ȡ
		m_jingduf=strBuffer.Mid(index_start+24,7);
		SetDlgItemText(IDC_Jingdu,m_jingdu);
		SetDlgItemText(IDC_Jingduf,m_jingduf);
					
		
		m_jing=strBuffer.Mid(index_start+33,1);
		/*
		if(m_jing.GetAt(0)=='W')
			m_jing="W";
		else m_jing="����";//��λ��Ϣ��ȡ
		*/
		SetDlgItemText(IDC_Jing,m_jing);
//+1

		m_haiba=strBuffer.Mid(index_start+34,5);//������Ϣ��ȡ
		SetDlgItemText(IDC_Haiba,m_haiba);


		m_day=strBuffer.Mid(index_start+39,2);
		SetDlgItemText(IDC_Day,m_day);    //����
						
				
						
		m_month=strBuffer.Mid(index_start+41,2);
		SetDlgItemText(IDC_Month,m_month);
						
						
		m_year=strBuffer.Mid(index_start+43,2);
		m_year = "20"+m_year;
		SetDlgItemText(IDC_Year,m_year);
		
		int index_comma;
		int index_comma_pre;
		int index_comma_dot;

		index_comma = strBuffer.Find(',',index_start+45);
		index_comma_dot = strBuffer.Find('.',index_start+45);
		m_speed=strBuffer.Mid(index_start+45,index_comma_dot+2-index_start-45);   //�ٶ�
		SetDlgItemText(IDC_SPEED,m_speed);
		index_comma_pre=index_comma+1;

		index_comma = strBuffer.Find(',',index_comma_pre);
		m_jihao=strBuffer.Mid(index_comma_pre,index_comma-index_comma_pre);   //�ɻ���
		SetDlgItemText(IDC_JIHAO,m_jihao);
		index_comma_pre=index_comma+1;

/*		//������Ϣ
		m_jihao=strBuffer.Left(3);
		SetDlgItemText(IDC_JIHAO,m_jihao);
		
		fadongji=strBuffer.GetAt(3);
		fadongji1=((fadongji>>1)&0x1)+'0';
		fadongji1_str=CString(fadongji1);
		fadongji2=((fadongji>>5)&0x1)+'0';
		fadongji2_str=CString(fadongji2);
		SetDlgItemText(IDC_FDJ1,fadongji1_str);
		SetDlgItemText(IDC_FDJ2,fadongji2_str);
*/

		fadongji=strBuffer.GetAt(index_comma_pre);
		fadongji1=((fadongji>>1)&0x1);
		fadongji1_str=CString(fadongji1);
		fadongji2=((fadongji>>5)&0x1);
		fadongji2_str=CString(fadongji2);

	//	fadongji =0xff;
		sta_dev1=((fadongji>>0)&0x1);		
		sta_dev2=((fadongji>>2)&0x1);
		sta_dev3=((fadongji>>4)&0x1);

//		fadongji2=((fadongji>>2)&0x1)+'0';





		index_comma = strBuffer.Find(',',index_comma_pre);
		index_comma_pre=index_comma+1;


		index_comma = strBuffer.Find(',',index_comma_pre);
		m_roll=strBuffer.Mid(index_comma_pre,index_comma-index_comma_pre);
		SetDlgItemText(IDC_ROLL,m_roll);
		index_comma_pre=index_comma+1;

		index_comma = strBuffer.Find(',',index_comma_pre);
		m_pitch=strBuffer.Mid(index_comma_pre,index_comma-index_comma_pre);
		SetDlgItemText(IDC_PITCH,m_pitch);
		index_comma_pre=index_comma+1;

		index_comma = strBuffer.Find(',',index_comma_pre);
		m_yaw=strBuffer.Mid(index_comma_pre,index_comma-index_comma_pre);
		SetDlgItemText(IDC_YAW,m_yaw);						
	//#endif				
			strBuffer.Empty();
			char m_rx_counts[10];
			cmd_rcv_flag = 0;
			rx_counts++;
			itoa(rx_counts,m_rx_counts,10);
			SetDlgItemText(IDC_COUNT,m_rx_counts);

		if(!fadongji1)
		{
			m_bitmap_fdj1.Detach();
			m_bitmap_fdj1.LoadBitmap(IDB_BITMAP_GREEN);
		}else
		{
			m_bitmap_fdj1.Detach();
			m_bitmap_fdj1.LoadBitmap(IDB_BITMAP_RED);
		}
			m_button_fdj1.SetBitmap(m_bitmap_fdj1);

		if(!fadongji2)
		{
			m_bitmap_fdj2.Detach();
			m_bitmap_fdj2.LoadBitmap(IDB_BITMAP_GREEN);
		}else
		{
			m_bitmap_fdj2.Detach();
			m_bitmap_fdj2.LoadBitmap(IDB_BITMAP_RED);
		}
			m_button_fdj2.SetBitmap(m_bitmap_fdj2);
//		SetDlgItemText(IDC_FDJ1,fadongji1_str);
//		SetDlgItemText(IDC_FDJ2,fadongji2_str);

				if(!sta_dev1)
		{
			m_bitmap_sta_dev1.Detach();
			m_bitmap_sta_dev1.LoadBitmap(IDB_BITMAP_GRAY);
		}else
		{
			m_bitmap_sta_dev1.Detach();
			m_bitmap_sta_dev1.LoadBitmap(IDB_BITMAP_GREEN);
		}
			m_button_sta_dev1.SetBitmap(m_bitmap_sta_dev1);

		if(!sta_dev2)
		{
			m_bitmap_sta_dev2.Detach();
			m_bitmap_sta_dev2.LoadBitmap(IDB_BITMAP_GRAY);
		}else
		{
			m_bitmap_sta_dev2.Detach();
			m_bitmap_sta_dev2.LoadBitmap(IDB_BITMAP_GREEN);
		}
			m_button_sta_dev2.SetBitmap(m_bitmap_sta_dev2);


		if(!sta_dev3)
		{
			m_bitmap_sta_dev3.Detach();
			m_bitmap_sta_dev3.LoadBitmap(IDB_BITMAP_GRAY);
		}else
		{
			m_bitmap_sta_dev3.Detach();
			m_bitmap_sta_dev3.LoadBitmap(IDB_BITMAP_GREEN);
		}
			m_button_sta_dev3.SetBitmap(m_bitmap_sta_dev3);



	}


	return true;
}

char m_count_djs[10];

void CTestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 1)
	{
		OnRecvComData();
		
		if(timer_cnt_60ms>0)
		{
			timer_cnt_60ms--;
			if(timer_cnt_60ms==0)
			{
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
			}
		}
		
		itoa(int(timer_cnt_60ms*60/1000),m_count_djs,10);
		SetDlgItemText(IDC_DJS,m_count_djs);


	}

	CDialog::OnTimer(nIDEvent);
}


void CTestDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	
	
	int nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO5))->GetCurSel();
	
	InintCom(nIndex);
	SetTimer(1,60,0);
//	Delay(10);

	char ICJC_MSG[20]={0x24,0x49,0x43,0x4a,0x43,0x00,0x0c,0x00,0x00,0x00,0x00,0x2b,0x0d,0x0a};
	m_com.write(ICJC_MSG,14);



}


// Len of HoTT serial buffer
int hott_msg_len = 0;

//uint8_t MSG_TO_SEND[43]="WWW.UCORTEX.COM---gaoming,gaoming,nihao ma";
char INFO_HEAD[6]="$TXSQ";   //5B
char INFO_LEN[2]={0x33,0x34};  //2B
//char INFO_ADDR[4]={0x04,0xE6,0x10};//321040  addr 3B
//uint8_t MSG_TX_FLAG=0B01000110;     //1B
char MSG_TX_FLAG=0x46;     //1B
//uint8_t MSG_TX_ADDR[4]={0x04,0xE6,0x10};//321040  TX addr   //3B
//char MSG_TX_ADDR[4]={0x04,0xE6,0x10};//  TX addr   //3B
char MSG_TX_ADDR[4]={0x02,0xF5,0x48};
char MSG_TX_LEN[2]={0x00,0x00};      //2B
char MSG_TX_ACK=0x00;               //1B
char MSG_TX[211]="gaoming";    //1680bit maxium   NON-MIL 628BIT  78B   17B+MSG +1CRC
int msg_num=0;  //neirong



char ALL_HEAD[18];

void init_msg_head(void){
			 int i,j;
			hott_msg_len=17+1+msg_num+1;
			INFO_LEN[1]=(char)(hott_msg_len);
//  	INFO_LEN[1]=(char)*(hott_msg_len&0xFF00);
			INFO_LEN[0]= hott_msg_len>>8;
			MSG_TX_LEN[1]=(msg_num+1)*8;
			MSG_TX_LEN[0]=(msg_num+1)*8 >> 8;
	
			for(i=0,j=0; i<5; i++,j++){
			ALL_HEAD[i]=INFO_HEAD[j];
			}
			for(j=0; j<2; i++,j++){
			ALL_HEAD[i]=INFO_LEN[j];
			}
			for(j=0; j<3; i++,j++){
			ALL_HEAD[i]=INFO_ADDR[j];
			}

			ALL_HEAD[i]=MSG_TX_FLAG;
			i++;
//			ALL_HEAD[i]=0x46;

			for(j=0; j<3; i++,j++){
			ALL_HEAD[i]=MSG_TX_ADDR[j];
			}			
			for(j=0; j<2; i++,j++){
			ALL_HEAD[i]=MSG_TX_LEN[j];
			}	
		
			ALL_HEAD[i]=MSG_TX_ACK;
			i++;	
			ALL_HEAD[i]=0xa4;
			i++;

			
}



//static void send_one_frame_data(void) {
void send_one_frame_data(void) {
    char msg_crc = 0;
//	int init_len;
	char *hott_msg_ptr = 0;
//	char temp;

//	char all_bytes[1024];

	/*
  if (hott_msg_len == 0) {
    hott_msg_ptr = 0;
//    hott_telemetry_is_sending = FALSE;
//    hott_telemetry_sendig_msgs_id = 0;
    msg_crc = 0;
  }
  else {
    --hott_msg_len;
    if (hott_msg_len != 0) {
      msg_crc += *hott_msg_ptr;
      uart_transmit(&uart1, *hott_msg_ptr++);
    } else
      uart_transmit(&uart1, (int8_t)msg_crc);
  }
	*/
	

/*
	init_len=hott_msg_len;
	while(hott_msg_len){
	    --hott_msg_len;
    if (hott_msg_len != 0) {
			if(hott_msg_len == init_len-19)
			{hott_msg_ptr = MSG_TX;}
			
//      msg_crc = msg_crc  *hott_msg_ptr;
			msg_crc ^= *hott_msg_ptr;

//      uart_transmit(&uart2, *hott_msg_ptr++);
				
		} 
	}
*/	
	msg_crc = 0;
	hott_msg_ptr = ALL_HEAD;

	int len=18;
	while(len)
	{
		msg_crc ^= *hott_msg_ptr;
		len--;
		hott_msg_ptr++;
	}

	len=msg_num;
	hott_msg_ptr = MSG_TX;
	while(len)
	{
		msg_crc ^= *hott_msg_ptr;
		len--;
		hott_msg_ptr++;
	}

	MSG_TX[msg_num]=msg_crc;
	MSG_TX[msg_num+1]=0x0d;
	MSG_TX[msg_num+2]=0x0a;


//	uart_transmit(&uart2, temp);	

}

bool dev1_state = 1;
bool dev2_state = 1;
bool dev3_state = 1;


void CTestDlg::OnSend() 
{
	// TODO: Add your control notification handler code here
	CString text;
	GetDlgItem(IDC_SEND)->GetWindowText(text);

   // int bytes_temp=1;
	//bytes_temp = m_com.write((char *)&text, 1024);
	CString phone_num_str;
	int phone_num;
	GetDlgItem(IDC_EDIT_PHONE_NUM)->GetWindowText(phone_num_str);
/*
	if(phone_num_str)
	{
	phone_num=atoi(phone_num_str);
	MSG_TX_ADDR[2]=phone_num;
	MSG_TX_ADDR[1]=phone_num>>8;
	MSG_TX_ADDR[0]=phone_num>>8;


	}
	*/
	char trans_dev_sta = 0x88;

	if(!dev1_state)
	{
		trans_dev_sta |= 1<<0;
	}else
	{
		trans_dev_sta &= ~(1<<0);
	}


	if(!dev2_state)
	{
		trans_dev_sta |= 1<<1;
	}else
	{
		trans_dev_sta &= ~(1<<1);
	}

	if(!dev3_state)
	{
		trans_dev_sta |= 1<<2;
	}else
	{
		trans_dev_sta &= ~(1<<2);
	}

	MSG_TX[0] = trans_dev_sta;
	
	char * text_tx;
	text_tx= text.GetBuffer(0);
	int i=1;
	while(*text_tx)
	{
		MSG_TX[i++]=*text_tx;
		text_tx++;
	}
	msg_num = i;
	
	init_msg_head();
	send_one_frame_data();
	m_com.write(ALL_HEAD,18);
	m_com.write(MSG_TX,msg_num+3);

//	timer_cnt_60ms=1000;
	timer_cnt_60ms=1016;
//	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);


//	m_com.write(&text_tx_p);
//	int bytes_temp=1;
//	bytes_temp  = m_com.write((char *)&text_tx);
}



void CTestDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	
	
}


void CTestDlg::OnButtonDev1() 
{
	// TODO: Add your control notification handler code here
	dev1_state=!dev1_state;

			if(!dev1_state)
		{
			m_bitmap_dev1.Detach();
			m_bitmap_dev1.LoadBitmap(IDB_BITMAP_OPEN);
		}else
		{
			m_bitmap_dev1.Detach();
			m_bitmap_dev1.LoadBitmap(IDB_BITMAP_CLOSE);
		}
			m_button_dev1.SetBitmap(m_bitmap_dev1);
	
}

void CTestDlg::OnButtonDev2() 
{
	// TODO: Add your control notification handler code here
		dev2_state=!dev2_state;

			if(!dev2_state)
		{
			m_bitmap_dev2.Detach();
			m_bitmap_dev2.LoadBitmap(IDB_BITMAP_OPEN);
		}else
		{
			m_bitmap_dev2.Detach();
			m_bitmap_dev2.LoadBitmap(IDB_BITMAP_CLOSE);
		}
			m_button_dev2.SetBitmap(m_bitmap_dev2);
	
}

void CTestDlg::OnButtonDev3() 
{
	// TODO: Add your control notification handler code here
		dev3_state=!dev3_state;

			if(!dev3_state)
		{
			m_bitmap_dev3.Detach();
			m_bitmap_dev3.LoadBitmap(IDB_BITMAP_OPEN);
		}else
		{
			m_bitmap_dev3.Detach();
			m_bitmap_dev3.LoadBitmap(IDB_BITMAP_CLOSE);
		}
			m_button_dev3.SetBitmap(m_bitmap_dev3);
	
}
